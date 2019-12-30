#include <vector>

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "clang/Driver/Options.h"

#include "Option.h"

using namespace std;
using namespace llvm;
using namespace opt;

bool PCCOption::ParseArgs(int argc, char** argv) {

  unsigned MissingArgIndex, MissingArgCount;
  unique_ptr<OptTable> clangOpts = clang::driver::createDriverOptTable();

  InputArgList Args = clangOpts->ParseArgs(
    makeArrayRef(argv + 1, argc - 1),
    MissingArgIndex, MissingArgCount);

  Help = false;
  OutputIR = false;

  for (const Arg *A : Args) {
    const Option &Option = A->getOption();
    if (Option.matches(clang::driver::options::OPT_INPUT))
      InputFiles.push_back(A->getValue());
    else if(Option.matches(clang::driver::options::OPT_help))
      Help = true;
    else if(Option.matches(clang::driver::options::OPT_S))
      OutputIR = true;
    else if(Option.matches(clang::driver::options::OPT_o))
      Output = A->getValue();
    else if(Option.matches(clang::driver::options::OPT_Wl_COMMA)){
      for(unsigned i=0; i<A->getNumValues(); i++){
        ldArgs.push_back(A->getValue(i));
      }

    } else {
      ArgStringList ASL;
      A->render(Args, ASL);
      for (auto it : ASL)
        clangArgs.push_back(it);
    }
  }
  clangArgs.push_back("--target=wasm32-wasm");
  // clangArgs.push_back("-O2");

  if(Help){
    clangOpts->PrintHelp(
        llvm::outs(), "platon-cpp [clang args]",
        "PlatON C++ WASM Compiler",
        0, 0, false);
    return false;
  }

  if(InputFiles.empty()){
    llvm::outs() << "error: no input files\n";
    return false;
  }

  if(Output.empty()){
    StringRef suffix = OutputIR?".ll":".wasm";
    StringRef prefix =
      InputFiles.size()==1?
      llvm::sys::path::stem(InputFiles[0]):
      "link";
    Output = (prefix + suffix).str();
  }

  return true;
}
