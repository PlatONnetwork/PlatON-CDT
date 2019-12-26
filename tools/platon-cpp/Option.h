#include <string>
#include <vector>

class PCCOption {
public:
  std::vector<std::string> InputFiles;
  std::string Output;
  bool Help;
  bool OutputIR;
  std::vector<std::string> clangArgs;

  bool ParseArgs(int, char**);
};
