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

  string binPath = llvm::sys::fs::getMainExecutable(argv[0], nullptr);
  SmallString<128> bindir0(binPath);
  llvm::sys::path::remove_filename(bindir0);
  bindir = bindir0.c_str();

  unsigned MissingArgIndex, MissingArgCount;
  unique_ptr<OptTable> clangOpts = clang::driver::createDriverOptTable();

  InputArgList Args = clangOpts->ParseArgs(
    makeArrayRef(argv + 1, argc - 1),
    MissingArgIndex, MissingArgCount);

  Help = false;
  OutputIR = false;
  bool NoStdlib = false;

  for (const Arg *A : Args) {
    const Option &Option = A->getOption();
    if (Option.matches(clang::driver::options::OPT_INPUT))
      InputFiles.push_back(A->getValue());
    else if(Option.matches(clang::driver::options::OPT_help))
      Help = true;
    else if(Option.matches(clang::driver::options::OPT_S))
      OutputIR = true;
    else if(Option.matches(clang::driver::options::OPT_nostdlib))
      NoStdlib = true;
    else if(Option.matches(clang::driver::options::OPT_o))
      Output = A->getValue();
    else if(Option.matches(clang::driver::options::OPT_L)) {
      ldArgs.push_back("-L");
      ldArgs.push_back(A->getValue());
    } else if(Option.matches(clang::driver::options::OPT_l)) {
      ldArgs.push_back("-l");
      ldArgs.push_back(A->getValue());
    } else if(Option.matches(clang::driver::options::OPT_Wl_COMMA)) {
      for(unsigned i=0; i<A->getNumValues(); i++) {
        ldArgs.push_back(A->getValue(i));
      }
    } else {
      ArgStringList ASL;
      A->render(Args, ASL);
      for (auto it : ASL)
        clangArgs.push_back(it);
    }
  }

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

  AdjustClangArgs(NoStdlib);
  AdjustLLDArgs(NoStdlib);
  return true;
}

void PCCOption::AdjustClangArgs(bool NoStdlib){
  clangArgs.push_back("--target=wasm32-wasm");
  clangArgs.push_back("-fno-rtti");
  clangArgs.push_back("-fno-exceptions");
  clangArgs.push_back("-std=c++17");
  clangArgs.push_back("-g");
  clangArgs.push_back("-O0");

  if(!NoStdlib){
    string includedir = bindir + "/../platon.cdt/include/";
    clangArgs.push_back("-I");
    clangArgs.push_back(includedir+"libcxx");
    clangArgs.push_back("-I");
    clangArgs.push_back(includedir+"libc");
    clangArgs.push_back("-I");
    clangArgs.push_back(includedir);
  }
}

void PCCOption::AdjustLLDArgs(bool NoStdlib){
  string libdir = bindir + "/../platon.cdt/lib/";
  string ExternSymbolFile = libdir + "extern_symbol";
  
  if(!NoStdlib) {
    ldArgs.push_back("-L");
    ldArgs.push_back(libdir);
    ldArgs.push_back("-lc");
    ldArgs.push_back("-lc++");
    ldArgs.push_back("-lbuiltins");
    ldArgs.push_back("-lplatonlib");
  }

  if(llvm::sys::fs::exists(ExternSymbolFile.c_str())) {
    ldArgs.push_back("--allow-undefined-file="+ExternSymbolFile);
  }
}
