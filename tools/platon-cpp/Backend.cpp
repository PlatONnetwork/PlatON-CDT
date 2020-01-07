
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/AutoUpgrade.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Support/Path.h"
#include "lld/Common/Driver.h"
#include <memory>
#include <string>
#include "Option.h"

using namespace llvm;

// General options for llc.  Other pass-specific options are specified
// within the corresponding llc passes, and target-specific options
// and back-end code generation options are specified with the target machine.
//
SmallString<128> TempFilename;
char CompileBin[] = "platon-cpp";


namespace {
static ManagedStatic<std::vector<std::string>> RunPassNames;

struct RunPassOption {
  void operator=(const std::string &Val) const {
    if (Val.empty())
      return;
    SmallVector<StringRef, 8> PassNames;
    StringRef(Val).split(PassNames, ',', -1, false);
    for (auto PassName : PassNames)
      RunPassNames->push_back(PassName);
  }
};
}

static RunPassOption RunPassOpt;

static cl::opt<RunPassOption, true, cl::parser<std::string>> RunPass(
    "run-pass",
    cl::desc("Run compiler only for specified passes (comma separated list)"),
    cl::value_desc("pass-name"), cl::ZeroOrMore, cl::location(RunPassOpt));


static std::unique_ptr<ToolOutputFile> GetOutputStream(const char *TargetName,
                                                       Triple::OSType OS,
                                                       const char *ProgName) {

  // Open the file.
  std::error_code EC;
  sys::fs::OpenFlags OpenFlags = sys::fs::F_None;
  auto FDOut = llvm::make_unique<ToolOutputFile>(TempFilename, EC, OpenFlags);
  if (EC) {
    WithColor::error() << EC.message() << '\n';
    return nullptr;
  }

  return FDOut;
}

int init(){
  LLVMInitializeWebAssemblyTargetInfo();
  LLVMInitializeWebAssemblyTarget();
  LLVMInitializeWebAssemblyTargetMC();
  LLVMInitializeWebAssemblyAsmPrinter();

  // Register the target printer for --version.
  cl::AddExtraVersionPrinter(TargetRegistry::printRegisteredTargetsForVersion);
  return 0;
}

static bool addPass(PassManagerBase &PM, const char *_CompileBin,
                    StringRef PassName, TargetPassConfig &TPC) {
  if (PassName == "none")
    return false;

  const PassRegistry *PR = PassRegistry::getPassRegistry();
  const PassInfo *PI = PR->getPassInfo(PassName);
  if (!PI) {
    WithColor::error(errs(), _CompileBin)
        << "run-pass " << PassName << " is not registered.\n";
    return true;
  }

  Pass *P;
  if (PI->getNormalCtor())
    P = PI->getNormalCtor()();
  else {
    WithColor::error(errs(), _CompileBin)
        << "cannot create pass: " << PI->getPassName() << "\n";
    return true;
  }
  std::string Banner = std::string("After ") + std::string(P->getPassName());
  PM.add(P);
  TPC.printAndVerify(Banner);

  return false;
}

TargetOptions InitTargetOptions(){
  TargetOptions Options;
  Options.StackAlignmentOverride = 0;
  Options.GlobalISelAbort = llvm::GlobalISelAbortMode::Enable;
  Options.UseInitArray = 1;
  Options.CompressDebugSections = llvm::DebugCompressionType::None;
  Options.FloatABIType = FloatABI::Default;
  Options.MCOptions.AsmVerbose = true;
  return Options;
}

int compileModule(Module* M) {
  // Load the module to be compiled...
  SMDiagnostic Err;
  std::unique_ptr<MIRParser> MIR;

  init();
  Triple TheTriple = Triple(M->getTargetTriple());

  if (TheTriple.getTriple().empty())
    TheTriple.setTriple(sys::getDefaultTargetTriple());

  // Get the target specific parser.
  std::string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget("", TheTriple,
                                                         Error);
  if (!TheTarget) {
    WithColor::error(errs(), CompileBin) << Error;
    return 1;
  }

  std::string CPUStr = "", FeaturesStr = "";

  TargetOptions Options = InitTargetOptions();

  std::unique_ptr<TargetMachine> Target(TheTarget->createTargetMachine(
      TheTriple.getTriple(), CPUStr, FeaturesStr, Options, None,
      None, CodeGenOpt::Default));

  assert(Target && "Could not allocate target machine!");

  assert(M && "Should have exited if we didn't have a module!");

  // Figure out where we are going to send the output.
  std::unique_ptr<ToolOutputFile> Out =
      GetOutputStream(TheTarget->getName(), TheTriple.getOS(), CompileBin);
  if (!Out) return 1;

  std::unique_ptr<ToolOutputFile> DwoOut;

  // Build up all of the passes that we want to do to the module.
  legacy::PassManager PM;

  // Add an appropriate TargetLibraryInfo pass for the module's triple.
  TargetLibraryInfoImpl TLII(Triple(M->getTargetTriple()));

  // The -disable-simplify-libcalls flag actually disables all builtin optzns.
  PM.add(new TargetLibraryInfoWrapperPass(TLII));

  // Add the target data from the target machine, if it exists, or the module.
  M->setDataLayout(Target->createDataLayout());

  // This needs to be done after setting datalayout since it calls verifier
  // to check debug info whereas verifier relies on correct datalayout.
  UpgradeDebugInfo(*M);

  {
    raw_pwrite_stream *OS = &Out->os();

    // Manually do the buffering rather than using buffer_ostream,
    SmallVector<char, 0> Buffer;
    std::unique_ptr<raw_svector_ostream> BOS;
    if (!Out->os().supportsSeeking()) {
      BOS = make_unique<raw_svector_ostream>(Buffer);
      OS = BOS.get();
    }

    const char *_CompileBin = CompileBin;
    LLVMTargetMachine &LLVMTM = static_cast<LLVMTargetMachine&>(*Target);
    MachineModuleInfo *MMI = new MachineModuleInfo(&LLVMTM);

    // Construct a custom pass pipeline that starts after instruction
    // selection.
    if (!RunPassNames->empty()) {
      if (!MIR) {
        WithColor::warning(errs(), _CompileBin)
            << "run-pass is for .mir file only.\n";
        return 1;
      }
      TargetPassConfig &TPC = *LLVMTM.createPassConfig(PM);

      if (TPC.hasLimitedCodeGenPipeline()) {
        WithColor::warning(errs(), _CompileBin)
            << "run-pass cannot be used with "
            << TPC.getLimitedCodeGenPipelineReason(" and ") << ".\n";
        return 1;
      }

      TPC.setDisableVerify(true);
      PM.add(&TPC);
      PM.add(MMI);
      TPC.printAndVerify("");
      for (const std::string &RunPassName : *RunPassNames) {
        if (addPass(PM, _CompileBin, RunPassName, TPC))
          return 1;
      }
      TPC.setInitialized();
      PM.add(createPrintMIRPass(*OS));
      PM.add(createFreeMachineFunctionPass());
    } else if (Target->addPassesToEmitFile(PM, *OS,
                                           DwoOut ? &DwoOut->os() : nullptr,
                                           TargetMachine::CGFT_ObjectFile, false, MMI)) {
      WithColor::warning(errs(), _CompileBin)
          << "target does not support generation of this"
          << " file type!\n";
      return 1;
    }

    if (MIR) {
      assert(MMI && "Forgot to create MMI?");
      if (MIR->parseMachineFunctions(*M, *MMI))
        return 1;
    }

    // Before executing passes, print the final values of the LLVM options.
    cl::PrintOptionValues();

    // If requested, run the pass manager over the same module again,
    // to catch any bugs due to persistent state in the passes. Note that
    // opt has the same functionality, so it may be worth abstracting this out
    // in the future.
    SmallVector<char, 0> CompileTwiceBuffer;


    PM.run(*M);

    /*
    auto HasError = Context.getDiagHandlerPtr()))->HasError;
    if (*HasError)
      return 1;
      */


    if (BOS) {
      Out->os() << Buffer;
    }
  }

  // Declare success.
  Out->keep();
  if (DwoOut)
    DwoOut->keep();

  return 0;
}

int GenerateWASM(PCCOption &Option, llvm::Module* M){

  llvm::sys::fs::createTemporaryFile("platon-cpp", "wasm", TempFilename);
  compileModule(M);

  std::vector<const char*> lldArgs;
  lldArgs.push_back("platon-cpp");
  lldArgs.push_back(TempFilename.data());
  lldArgs.push_back("-L.");

  for(unsigned i=0; i<Option.ldArgs.size(); i++){
    lldArgs.push_back(Option.ldArgs[i].data());
  }

  lldArgs.push_back("--entry");
  lldArgs.push_back("invoke");
  lldArgs.push_back("-o");
  lldArgs.push_back(Option.Output.data());

  return !lld::wasm::link(lldArgs, true);
}
