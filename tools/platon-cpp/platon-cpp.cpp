#include <vector>
#include <string>
#include <cstdlib>

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
int GenerateABI(std::string &, llvm::Module*);
int GenerateWASM(PCCOption &, llvm::Module*);
void PCCPass(llvm::Module &);

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
      Mod = Act.takeModule();
      return true;
    } else {
      bool Err = Linker::linkModules(*Mod, Act.takeModule());
      return !Err;
    }
  }
};

int OutputIRFile(llvm::Module* M, string OutputIRPath) {
  std::error_code EC;
  ToolOutputFile Out(OutputIRPath, EC, sys::fs::F_None);
  if (EC) {
    errs() << EC.message() << '\n';
    return 1;
  }
  M->print(Out.os(), nullptr, false);
  Out.keep();
  return 0;
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

  if(Tool.run(&Builder))return 1;

  std::unique_ptr<llvm::Module> M = std::move(Builder.Mod);

  if(Option.OutputIR){
    return OutputIRFile(M.get(), Option.Output);
  }
    
  if(!Option.NoABI)
    GenerateABI(Option.Output, M.get());

  PCCPass(*M);

  int result = GenerateWASM(Option, M.get());
  if (0 == result) {
    std::string command =
        "wasm-opt -O2 -o " + Option.Output + " " + Option.Output;
    system(command.c_str());
  }

  return result;
}
