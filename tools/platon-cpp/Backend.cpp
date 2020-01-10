
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/AutoUpgrade.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/Path.h"
#include "llvm/Target/TargetMachine.h"
#include "lld/Common/Driver.h"
#include <string>
#include "Option.h"

using namespace llvm;

int init(){
  LLVMInitializeWebAssemblyTargetInfo();
  LLVMInitializeWebAssemblyTarget();
  LLVMInitializeWebAssemblyTargetMC();
  LLVMInitializeWebAssemblyAsmPrinter();

  // Register the target printer for --version.
  cl::AddExtraVersionPrinter(TargetRegistry::printRegisteredTargetsForVersion);
  return 0;
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

int compileModule(Module* M, SmallString<128> &TempPath){

  const char CompileBin[] = "platon-cpp";

  // Load the module to be compiled...
  SMDiagnostic Err;

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

  std::error_code EC;
  ToolOutputFile Out(TempPath, EC, sys::fs::F_None);
  if (EC) {
    errs() << EC.message() << '\n';
    return 1;
  }

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
    raw_pwrite_stream *OS = &Out.os();

    LLVMTargetMachine &LLVMTM = static_cast<LLVMTargetMachine&>(*Target);
    MachineModuleInfo *MMI = new MachineModuleInfo(&LLVMTM);

    if (Target->addPassesToEmitFile(PM, *OS, nullptr,
                                           TargetMachine::CGFT_ObjectFile, false, MMI)) {
      WithColor::warning(errs(), CompileBin)
          << "target does not support generation of this"
          << " file type!\n";
      return 1;
    }

    // Before executing passes, print the final values of the LLVM options.
    cl::PrintOptionValues();
    PM.run(*M);
  }

  // Declare success.
  Out.keep();
  return 0;
}

int GenerateWASM(PCCOption &Option, llvm::Module* M){

  SmallString<128> TempPath;
  llvm::sys::fs::createTemporaryFile("platon-cpp", "wasm", TempPath);

  if(compileModule(M, TempPath))
    return 1;

  std::vector<const char*> lldArgs;
  lldArgs.push_back("platon-cpp");
  lldArgs.push_back(TempPath.data());
  lldArgs.push_back("-L.");

  for(unsigned i=0; i<Option.ldArgs.size(); i++){
    lldArgs.push_back(Option.ldArgs[i].data());
  }

  lldArgs.push_back("--strip-all");
  lldArgs.push_back("--no-threads");
  lldArgs.push_back("--entry");
  lldArgs.push_back("invoke");
  lldArgs.push_back("-o");
  lldArgs.push_back(Option.Output.data());

  return !lld::wasm::link(lldArgs, true);
}
