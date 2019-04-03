#ifndef __UTILS_H_
#define __UTILS_H_

#include <string>
#include <vector>
#include <sstream>

#include "boost/dll/runtime_symbol_info.hpp"
#include "boost/filesystem.hpp"
#include "llvm/Support/Program.h"

namespace platon {
namespace cdt {
namespace utils {
static std::string where() {
  auto path = boost::dll::program_location().parent_path();
  return std::string(path.string());
}

static std::string pwd() {
    auto path = boost::filesystem::current_path();
    return std::string(path.string());
}
} // namespace utils

namespace runtime {
static bool exec_subprogram(const std::string prog,
                            std::vector<std::string> options,
                            bool root = false) {
  std::stringstream args;
  for (const auto& s : options) {
    args << s << " ";
  }
  std::string find_path = utils::where();
  if (root) find_path = "/usr/bin";
  if (auto path = llvm::sys::findProgramByName(prog.c_str(), {find_path})) {
    std::system((*path + " " + args.str()).c_str());
  } else {
    return false;
  }
  return true;
}

}  // namespace runtime
}  // namespace cdt
}  // namespace platon

#endif  // __UTILS_H_
