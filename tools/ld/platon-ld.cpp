#include <iostream>
#include <string>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

#include "platon/utils.hpp"

#define ONLY_LD 1
#include "options.hpp"

const std::string kLdName = "platon-ld";

int main(int argc, const char** argv) {
  for (auto i = 0; i < argc; i++) {
    if (argv[i] == std::string("-v")) {
      platon::cdt::runtime::exec_subprogram("wasm-ld", {"-v"});
      return 0;
    }
  }

  llvm::cl::SetVersionPrinter([](llvm::raw_ostream& os) {
    os << kLdName << " version "
       << "${VERSION_FULL}"
       << "\n";
  });
  llvm::cl::ParseCommandLineOptions(argc, argv,
                                    kLdName + " (WebAssembly linker)");
  auto opts = CreateOptions();

  if (!platon::cdt::runtime::exec_subprogram("wasm-ld", opts.ld_opts)) {
    return -1;
  }

  if (!llvm::sys::fs::exists(opts.output_filename)) {
    return -1;
  }
  return 0;
}
