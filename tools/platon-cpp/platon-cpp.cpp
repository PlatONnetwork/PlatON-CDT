#include <vector>
#include <string>

#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/Signals.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CompilationDatabase.h"

#include "Option.h"

using namespace std;
using namespace llvm;
using namespace clang;
using namespace tooling;


bool ParseArgs(int, char *[], PCCOption &);
int GenerateWASM(char* clangPath, vector<string> &ldArgs, const char* output, llvm::Module*);

//void PCCPass(llvm::Module &);
//bool GenFiles(llvm::Module*, PCCOption &Option);

class BuilderAction : public ToolAction {
public:
  LLVMContext &llvmContext;
  std::unique_ptr<llvm::Module> Mod;

  BuilderAction(LLVMContext &llvmContext): llvmContext(llvmContext) {
    Mod = nullptr;
  }
  bool runInvocation(std::shared_ptr<CompilerInvocation> Invocation,
                     FileManager *Files,
                     std::shared_ptr<PCHContainerOperations> PCHContainerOps,
                     DiagnosticConsumer *DiagConsumer) override {

    Invocation->getHeaderSearchOpts().UseStandardSystemIncludes = 0;
    Invocation->getHeaderSearchOpts().UseBuiltinIncludes = 0;

    Invocation->getCodeGenOpts().setDebugInfo(clang::codegenoptions::LimitedDebugInfo);
    IntrusiveRefCntPtr<clang::DiagnosticsEngine> Diags =
        CompilerInstance::createDiagnostics(&Invocation->getDiagnosticOpts(),
                                            DiagConsumer, false);

#ifdef __WINDOWS_
    Invocation->getFrontendOpts().OutputFile = "NUL";
#else
    Invocation->getFrontendOpts().OutputFile = "/dev/null";
#endif


    CompilerInstance Compiler;
    Compiler.setInvocation(std::move(Invocation));
    Compiler.createDiagnostics();

    EmitLLVMAction Act(&llvmContext);

    bool succ = Compiler.ExecuteAction(Act);
    
    if(!succ)return false;
   
    if(Mod == nullptr){
      Mod = std::move(Act.takeModule());
      return true;
    } else {
      bool Err = Linker::linkModules(*Mod, std::move(Act.takeModule()));
      return !Err;
    }
  }
};

void OutputIRFile(llvm::Module* M, string OutputIRPath) {
  std::error_code EC;
  ToolOutputFile Out(OutputIRPath, EC, sys::fs::F_None);
  if (EC) {
    errs() << EC.message() << '\n';
    return;
  }
  M->print(Out.os(), nullptr, false);
  Out.keep();
}

int main(int argc, char **argv) {
  sys::PrintStackTraceOnErrorSignal(argv[0]);
  llvm::PrettyStackTraceProgram X(argc, argv);

  PCCOption Option;

  if(!Option.ParseArgs(argc, argv))
    return 0;

  FixedCompilationDatabase Compilations(".", Option.clangArgs);

  ClangTool Tool(Compilations, Option.InputFiles);

  LLVMContext Context;
  BuilderAction Builder(Context);

  if(Tool.run(&Builder))return 0;

  std::unique_ptr<llvm::Module> M = std::move(Builder.Mod);
  if(Option.OutputIR){
    OutputIRFile(M.get(), Option.Output);
    return 0;
  }
//  PCCPass(*M);

	//link lib
    
  GenerateWASM(argv[0], Option.ldArgs, Option.Output.data(), M.get());
  return 0;
}
