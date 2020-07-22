#include <string>
#include <vector>

class PCCOption {
public:
  std::string bindir;
  std::vector<std::string> InputFiles;
  std::string Output;
  bool NoABI;
  bool Help;
  bool OutputIR;
  std::vector<std::string> ldArgs;
  std::vector<std::string> clangUserArgs;
  std::vector<std::string> clangArgs;

  bool ParseArgs(int, char**);
  void AdjustClangArgs(bool);
  void AdjustLLDArgs(bool);
};
