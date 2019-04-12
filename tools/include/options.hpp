#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include <string>
#include <vector>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

#include "platon/utils.hpp"

#ifdef ONLY_LD
#define LD_CAT PlatonLdToolCategory
#else
#define LD_CAT PlatonCompilerToolCategory
#endif

#ifdef ONLY_LD
static llvm::cl::OptionCategory PlatonLdToolCategory("ld options");
#else
static llvm::cl::OptionCategory PlatonCompilerToolCategory("compiler options");
#endif

// ld options
#ifdef ONLY_LD
static llvm::cl::list<std::string> L_opt(
    "L", llvm::cl::desc("Add directory to library search path"),
    llvm::cl::cat(LD_CAT), llvm::cl::Prefix, llvm::cl::ZeroOrMore);
static llvm::cl::list<std::string> l_opt(
    "l", llvm::cl::desc("Root name of library to link"), llvm::cl::cat(LD_CAT),
    llvm::cl::Prefix, llvm::cl::ZeroOrMore);
static llvm::cl::list<std::string> export_opt(
    "export", llvm::cl::desc("Force a symbol to be exported"),
    llvm::cl::cat(LD_CAT), llvm::cl::Prefix, llvm::cl::ZeroOrMore);
#endif

static llvm::cl::opt<std::string> o_opt(
    "o", llvm::cl::desc("Write output to <file>"), llvm::cl::cat(LD_CAT));
static llvm::cl::list<std::string> input_filename_opt(
    llvm::cl::Positional, llvm::cl::desc("<input file> ..."),
    llvm::cl::cat(LD_CAT), llvm::cl::OneOrMore);
#ifndef ONLY_LD
static llvm::cl::opt<bool> abigen_opt("abigen",
                                      llvm::cl::desc("Generate abi file"),
                                      llvm::cl::cat(LD_CAT));
#endif

struct Options {
  std::string output_filename;
  std::vector<std::string> inputs;
  bool link;
  bool abigen;
  std::vector<std::string> compiler_opts;
  std::vector<std::string> ld_opts;
  std::vector<std::string> abigen_opts;
  std::string abi_filename;
  std::string exports_filename;
};

static void GetCompilerOptDefaults(std::vector<std::string>& opts) {
  opts.emplace_back("-std=c++17");
  opts.emplace_back("-emit-llvm");
  opts.emplace_back("-O3");
  opts.emplace_back("--target=wasm32");
  opts.emplace_back("-ffreestanding");
  opts.emplace_back("-nostdlib");
  opts.emplace_back("-fno-builtin");
  opts.emplace_back("-fno-threadsafe-statics");
  opts.emplace_back("-fno-exceptions");
  opts.emplace_back("-fno-rtti");
  opts.emplace_back("-fmodules-ts");
  opts.emplace_back("-DBOOST_DISABLE_ASSERTS");
  opts.emplace_back("-DBOOST_EXCEPTION_DISABLE");
  opts.emplace_back("-DWASM_COMPILE");
  opts.emplace_back("-DRAPIDJSON_64BIT=1");
}

#ifdef ONLY_LD
static void GetLdOptDefaults(std::vector<std::string>& opts) {
  opts.emplace_back("-O3");
  opts.emplace_back("--lto-O3");
  opts.emplace_back("--gc-sections");
  opts.emplace_back("--strip-all");
  opts.emplace_back("--merge-data-segments");
  opts.emplace_back("--allow-undefined");
  opts.emplace_back("--no-entry");
  opts.emplace_back("-lc++ -lc");
}
#endif

static Options CreateOptions() {
  Options opts;
  opts.link = true;

  GetCompilerOptDefaults(opts.compiler_opts);
#ifdef ONLY_LD
  GetLdOptDefaults(opts.ld_opts);
#endif

#ifndef ONLY_LD
  if (abigen_opt) {
    opts.abigen = true;
    opts.abigen_opts.emplace_back("-extra-arg=-std=c++17");
    opts.abigen_opts.emplace_back("-extra-arg=--target=wasm32");
    opts.abigen_opts.emplace_back("-extra-arg=-w");
    opts.abigen_opts.emplace_back("-extra-arg=-I.");
    opts.abigen_opts.emplace_back(
        "-extra-arg=-I" + platon::cdt::utils::where() + "/../include/libcxx");
    opts.abigen_opts.emplace_back(
        "-extra-arg=-I" + platon::cdt::utils::where() + "/../include/libc");
    opts.abigen_opts.emplace_back("-extra-arg=-I" +
                                  platon::cdt::utils::where() + "/../include");
  }
#endif

#ifdef ONLY_LD
  opts.ld_opts.emplace_back("-L" + platon::cdt::utils::where() + "/../lib");
#endif

  opts.compiler_opts.emplace_back("-I.");
  opts.compiler_opts.emplace_back("-I" + platon::cdt::utils::where() +
                                  "/../include/libcxx");
  opts.compiler_opts.emplace_back("-I" + platon::cdt::utils::where() +
                                  "/../include/libc");
  opts.compiler_opts.emplace_back("-I" + platon::cdt::utils::where() +
                                  "/../include");
  opts.compiler_opts.emplace_back("--sysroot=" + platon::cdt::utils::where() +
                                  "/../");
  opts.ld_opts.emplace_back("-L" + platon::cdt::utils::where() + "/../lib");
  opts.compiler_opts.emplace_back("-c");

  for (const auto& f : input_filename_opt) {
#ifdef ONLY_LD
    opts.ld_opts.push_back(f);
#endif
    opts.inputs.push_back(f);
  }

#ifndef ONLY_LD
  if (abigen_opt && opts.inputs.size() > 0) {
    opts.abigen_opts.push_back(opts.inputs[0]);
    llvm::SmallString<256> fn = llvm::sys::path::filename(opts.inputs[0]);
    opts.abi_filename =
        platon::cdt::utils::pwd() + "/" + std::string(fn.str()) + ".abi.json";
    opts.exports_filename =
        platon::cdt::utils::pwd() + "/" + std::string(fn.str()) + ".exports";
    opts.abigen_opts.emplace_back("-outpath=" + platon::cdt::utils::pwd());
    opts.abigen_opts.emplace_back("--");
    opts.abigen_opts.emplace_back("-w");
  }
#endif

#ifdef ONLY_LD
  for (const auto& opt : l_opt) {
    opts.ld_opts.emplace_back("-l" + opt);
  }
  for (const auto& opt : L_opt) {
    opts.ld_opts.emplace_back("-L" + opt);
  }
  for (const auto& opt : export_opt) {
    opts.ld_opts.emplace_back("--export " + opt);
  }
#endif

  if (o_opt.empty()) {
#ifndef ONLY_LD
    if (opts.inputs.size() == 1) {
      llvm::SmallString<256> fn = llvm::sys::path::filename(opts.inputs[0]);
      llvm::SmallString<256> fn2 = fn;
      llvm::sys::path::replace_extension(fn, ".wasm");
      opts.output_filename = fn.str();
      llvm::SmallString<256> res;
      llvm::sys::path::system_temp_directory(true, res);
      opts.ld_opts.emplace_back(std::string(std::string(res.str()) + "/" +
                                            std::string(fn2.str()) + ".o"));
    } else {
      opts.ld_opts.emplace_back("a.out");
    }
#endif

    if (opts.inputs.size() == 1) {
      llvm::SmallString<256> fn = llvm::sys::path::filename(opts.inputs[0]);
      llvm::sys::path::replace_extension(fn, ".wasm");
      opts.ld_opts.emplace_back("-o " + std::string(fn.str()));
      opts.output_filename = fn.str();
    } else {
      opts.ld_opts.emplace_back("-o a.out");
      opts.output_filename = "a.out";
    }
  } else {
    opts.ld_opts.emplace_back("-o " + o_opt);
    opts.output_filename = o_opt;
  }
  return opts;
}

#endif  // __OPTIONS_H_
