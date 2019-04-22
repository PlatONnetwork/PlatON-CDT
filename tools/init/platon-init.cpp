#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

#include "platon/utils.hpp"

class Project final {
 public:
  Project(const std::string& project_name, const std::string& path, bool bare)
      : project_name_(project_name), path_(path), bare_(bare) {
    llvm::sys::fs::create_directory(path_);
    if (!bare) {
      llvm::sys::fs::create_directory(path_ + "/src");
      llvm::sys::fs::create_directory(path_ + "/include");
      llvm::sys::fs::create_directory(path_ + "/build");
    }
  }

  ~Project() = default;

  void WriteCpp() {
    std::ofstream cpp_out;
    if (bare_) {
      cpp_out.open(path_ + "/" + project_name_ + ".cpp");
    } else {
      cpp_out.open(path_ + "/src/" + project_name_ + ".cpp");
    }
    cpp_out << ReplaceIncludeType(ReplaceName(kCpp));
  }

  void WriteAbiCpp() {
    if (!bare_) {
      std::ofstream abi_cpp_out(path_ + "/src/" + project_name_ + "_abi.cpp");
      abi_cpp_out << ReplaceName(kAbiCpp);
    }
  }

  void WriteHpp() {
    std::ofstream hpp_out;
    if (bare_) {
      hpp_out.open(path_ + "/" + project_name_ + ".hpp");
    } else {
      hpp_out.open(path_ + "/include/" + project_name_ + ".hpp");
    }
    hpp_out << ReplaceName(kHpp);
  }

  void WriteCMake() {
    if (!bare_) {
      std::ofstream cmake_out(path_ + "/src/CMakeLists.txt");
      cmake_out << ReplaceName(kCMake);
    }
  }

  void WriteCMakeExtern() {
    if (!bare_) {
      std::ofstream cmake_out(path_ + "/CMakeLists.txt");
      cmake_out << ReplaceName(kCMakeExtern);
    }
  }

  void WriteReadme() {
    std::ofstream readme_out(path_ + "/README.txt");
    if (bare_) {
      readme_out << ReplaceName(kReadme);
    } else {
      readme_out << ReplaceName(kReadmeCMake);
    }
  }

 private:
  std::string ReplaceName(const std::string& in) {
    std::stringstream ss;
    for (char c : in) {
      if (c == '@') {
        ss << project_name_;
      } else {
        ss << c;
      }
    }
    return ss.str();
  }

  std::string ReplaceIncludeType(const std::string& in) {
    std::stringstream ss;
    for (char c : in) {
      if ((c == '<' || c == '>') && bare_) {
        ss << '"';
      } else {
        ss << c;
      }
    }
    return ss.str();
  }

  std::string project_name_;
  std::string path_;
  bool bare_;

  const std::string kCpp =
      "#include <@.hpp>\n\n"
      "void @::init() {}\n\n"
      "CONSTANT void @::hello(const char* name) {\n"
      "  println(\"hello\", name);\n"
      "}\n\n";

  const std::string kAbiCpp =
      "// This file auto generated, do not modify it!!!\n"
      "#include <@.hpp>\n\n";

  const std::string kHpp =
      "#pragma once\n"
      "#include <platon/platon.hpp>\n"
      "using namespace platon;\n\n"
      "class @ : public Contract {\n"
      " public: \n"
      "  void init();\n\n"
      "  CONSTANT void hello(const char* name);\n"
      "};\n\n"
      "// You must define ABI here.\n"
      "PLATON_ABI(@, init);\n"
      "PLATON_ABI(@, hello);\n\n";

  const std::string kCMake =
      "project(@)\n\n"
      "if (PLATON_CDT_ROOT STREQUAL \"\" OR NOT PLATON_CDT_ROOT)\n"
      "  find_package(platon.cdt)\n"
      "else()\n"
      "  list(APPEND CMAKE_MODULE_PATH "
      "${PLATON_CDT_ROOT}/lib/cmake/platon.cdt)\n"
      "  include(PlatonWasmToolchain)\n"
      "  include(PlatonCDTMacros)\n"
      "endif()\n\n"
      "add_contract(@ @ @.cpp @_abi.cpp)\n"
      "target_include_directories(@ PUBLIC ${CMAKE_SOURCE_DIR}/../include)\n";

  const std::string kCMakeExtern =
      "include(ExternalProject)\n"
      "# if no cdt root is given use default path\n"
      "if (PLATON_CDT_ROOT STREQUAL \"\" OR NOT PLATON_CDT_ROOT)\n"
      "  find_package(platon.cdt)\n"
      "endif()\n\n"
      "ExternalProject_Add(\n"
      "  @_project\n"
      "  SOURCE_DIR ${CMAKE_SOURCE_DIR}/src\n"
      "  BINARY_DIR ${CMAKE_BINARY_DIR}/@\n"
      "  CMAKE_ARGS "
      "-DCMAKE_TOOLCHAIN_FILE=${PLATON_CDT_ROOT}/lib/cmake/platon.cdt/"
      "PlatonWasmToolchain.cmake -DPLATON_CDT_ROOT=${PLATON_CDT_ROOT}\n"
      "  UPDATE_COMMAND \"\"\n"
      "  PATCH_COMMAND \"\"\n"
      "  TEST_COMMAND \"\"\n"
      "  INSTALL_COMMAND \"\"\n"
      "  BUILD_ALWAYS 1\n"
      ")";

  const std::string kReadmeCMake =
      "--- @ Porject ---\n\n"
      " - How to Build\n"
      "   - cd to 'build' directory\n"
      "   - run the command 'cmake ..'\n"
      "   - run the command 'make' \n\n"
      " - After build - \n"
      "   - The built smart contract is under the '@' directory in the 'build' "
      "directory\n"
      "   - You can then use 'ctool' to deploy contract\n\n"
      " - Additions to CMake should be done to CMakeLists.tx in the './src' "
      "directory and not in the top level CMakeLists.txt";

  const std::string kReadme =
      "--- @ Project ---\n\n"
      "- How to Build -\n"
      "  - run the command 'platon-cpp -o @.wasm @.cpp -abigen'\n";
};

int main(int argc, const char** argv) {
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream& os) {
    os << "platon-init version "
       << "@VERSION_FULL@" << '\n';
  });
  llvm::cl::OptionCategory cat("platon-init",
                               "generates an platon smart contract project");

  llvm::cl::opt<bool> bare_opt(
      "bare",
      llvm::cl::desc(
          "produces only a skeleton smart contract without CMake support"),
      llvm::cl::cat(cat));
  llvm::cl::opt<std::string> project_name(
      "project", llvm::cl::desc("output project name"), llvm::cl::Required,
      llvm::cl::cat(cat));
  llvm::cl::opt<std::string> output_dir(
      "path", llvm::cl::desc("directory to place the project"),
      llvm::cl::cat(cat));

  llvm::cl::ParseCommandLineOptions(argc, argv, std::string("platon-proj"));
  try {
    if (!std::regex_match(project_name,
                          std::regex("^[_a-zA-Z][_a-zA-Z0-9]*$"))) {
      throw std::runtime_error("ERROR: invalid identifier: " + project_name +
                               " (ensure that it is a valid C++ identifier)");
    }

    llvm::SmallString<128> rp;
    std::string path = output_dir;
    if (path.empty()) {
      path = ".";
    }
    llvm::sys::fs::real_path(path, rp, false);
    path = rp.str();
    if (path.empty()) {
      llvm::outs() << output_dir + " path not found\n";
      return -1;
    }
    path += "/" + project_name;

    Project project(project_name, path, bare_opt);
    project.WriteCpp();
    project.WriteAbiCpp();
    project.WriteHpp();
    project.WriteCMake();
    project.WriteCMakeExtern();
    project.WriteReadme();
  } catch (std::exception& e) {
    std::cout << e.what() << "\n";
    return -1;
  }
  return 0;
}
