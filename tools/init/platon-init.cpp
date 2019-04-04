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
  Project(const std::string& project_name, const std::string& path)
      : project_name_(project_name), path_(path) {
    llvm::sys::fs::create_directory(path_);
  }

  ~Project() = default;

  void WriteCpp() {
    std::ofstream cpp_out;
    cpp_out.open(path_ + "/" + project_name_ + ".cpp");
    cpp_out << ReplaceName(kCpp);
  }

  void WriteReadme() {
    std::ofstream readme_out;
    readme_out.open(path_ + "/README.txt");
    readme_out << ReplaceName(kReadme);
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

  std::string project_name_;
  std::string path_;

  const std::string kCpp =
      "#include <platon/platon.hpp>\n"
      "using namespace platon;\n\n"
      "class @ : public Contract {\n"
      " public:\n"
      "  virtual void init() override {}\n\n"
      "  [[platon::constant]]\n"
      "  void hello(const char* name) {\n"
      "    println(\"hello\", name);\n"
      "  }\n"
      "};\n\n"
      "PLATON_ABI(@, hello);\n\n";

  const std::string kReadme =
      "--- @ Project ---\n\n"
      "- How to Build -\n"
      "  - run the command 'platon-cpp -o @.wasm @.cpp -abigen'\n";
};

int main(int argc, const char** argv) {
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream& os) {
    os << "platon-init version "
       << "${VERSION_FULL}" << '\n';
  });
  llvm::cl::OptionCategory cat("platon-init",
                               "generates an platon smart contract project");

  llvm::cl::opt<std::string> project_name(
      "project", llvm::cl::desc("output project name"), llvm::cl::Required,
      llvm::cl::cat(cat));
  llvm::cl::opt<std::string> output_dir(
      "path", llvm::cl::desc("directory to place the project"));

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

    Project project(project_name, path);
    project.WriteCpp();
    project.WriteReadme();
  } catch (std::exception& e) {
    std::cout << e.what() << "\n";
    return -1;
  }
  return 0;
}
