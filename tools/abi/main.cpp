#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#define RAPIDJSON_HAS_STDSTRING 1

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Driver/Options.h"
#include "clang/Lex/Preprocessor.h"

#include "boost/filesystem.hpp"
#include "boost/random.hpp"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/QualTypeNames.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/Sema.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

#include "AbiDef.h"
#include "AbiGenerator.h"
#include "AbiJson.h"
#include "AbiMacro.h"
#include "Exception.h"
#include "Log.h"
#include "StringUtil.h"
#include "Template.h"

using namespace std;
namespace cl = llvm::cl;
namespace fs = boost::filesystem;
namespace br = boost::random;

using namespace clang;
using namespace platon;

static cl::OptionCategory abiGeneratorOptions("ABI generator options");

static cl::opt<std::string> logPath(
    "log-path", cl::desc("log path, default = /tmp/[randomdir]/platonabi.log"),
    cl::cat(abiGeneratorOptions));

static cl::opt<std::string> logLevel("log-level",
                                     cl::desc("log level, default = debug"),
                                     cl::cat(abiGeneratorOptions),
                                     cl::init("debug"));

static cl::opt<bool> verbose("verbose", cl::desc("log on-off, default = true"),
                             cl::cat(abiGeneratorOptions), cl::init(true));

static cl::opt<std::string> abigen_output_opt(
    "abigen_output", cl::desc("ABIGEN output, default = ./<filename>.abi.json"),
    cl::cat(abiGeneratorOptions));

static cl::opt<std::string> exports_output_opt("exports_output",
                                               cl::desc("exports output"),
                                               cl::cat(abiGeneratorOptions));

std::unique_ptr<tooling::FrontendActionFactory> CreateFindMacroFactory(
    ContractDef &contractDef, vector<string> &actions) {
  struct FrontendMacroActionFactory : public tooling::FrontendActionFactory {
    ContractDef &contractDef;
    vector<string> &actions;

    FrontendMacroActionFactory(ContractDef &contractDef,
                               vector<string> &actions)
        : contractDef(contractDef), actions(actions) {}

    clang::FrontendAction *create() override {
      return new FindMacroAction(contractDef, actions);
    }
  };

  LOGDEBUG << "start create_find_macro_factory";
  return std::unique_ptr<tooling::FrontendActionFactory>(
      new FrontendMacroActionFactory(contractDef, actions));
}

std::unique_ptr<tooling::FrontendActionFactory> createFactory(
    const string &contract, const vector<string> &actions, ABIDef &abiDef) {
  struct ABIFrontendActionFactory : public tooling::FrontendActionFactory {
    const string &contract;
    const vector<string> &actions;
    ABIDef &abiDef;

    ABIFrontendActionFactory(const string &contract,
                             const vector<string> &actions, ABIDef &abiDef)
        : contract(contract), actions(actions), abiDef(abiDef) {}

    clang::FrontendAction *create() override {
      return new ABIAction(contract, actions, abiDef);
    }
  };

  return std::unique_ptr<tooling::FrontendActionFactory>(
      new ABIFrontendActionFactory(contract, actions, abiDef));
}

void createJsonAbi(const ABIDef &abiDef, const ContractDef &contractDef,
                   const string &srcFile, const string &destPath,
                   fs::path &randomDir) {
  fs::path current = fs::current_path();
  fs::path outPath(destPath);

  if (srcFile.empty()) {
    throw Exception() << ErrStr("src name is empty");
  }
  string fileName = srcFile + ".abi.json";
  if (destPath.empty()) {
    outPath = current / fileName;
  }
  fs::path tmpFile = randomDir / fileName;

  LOGERROR << "random:[" << randomDir.string() << "] fileName:" << fileName;

  LOGDEBUG << "tmpFile:[" << tmpFile.string() << "]";

  std::ofstream fs(tmpFile.string().c_str());

  if (!fs.is_open()) {
    throw Exception() << ErrStr("fs is not open:") << ErrStr(strerror(errno));
  }
  outputJsonABI(abiDef, contractDef, fs);
  fs.close();

  LOGDEBUG << "copy ::" << tmpFile.string() << " to ::" << outPath.string();
  fs::copy_file(tmpFile, outPath, fs::copy_option::overwrite_if_exists);
}

void createExportsFile(const ABIDef &abiDef, const string &srcFile,
                       const string &destPath, fs::path &randomDir) {
  fs::path current = fs::current_path();
  fs::path outPath(destPath);

  if (srcFile.empty()) {
    throw Exception() << ErrStr("src name is empty");
  }
  string fileName = srcFile + ".exports";
  if (destPath.empty()) {
    outPath = current / fileName;
  }
  fs::path tmpFile = randomDir / fileName;

  LOGERROR << "random:[" << randomDir.string() << "] fileName:" << fileName;

  LOGDEBUG << "tmpFile:[" << tmpFile.string() << "]";

  std::ofstream fs(tmpFile.string().c_str());

  if (!fs.is_open()) {
    throw Exception() << ErrStr("fs is not open:") << ErrStr(strerror(errno));
  }

  fs << "init" << std::endl;
  for (size_t i = 0; i < abiDef.abis.size(); i++) {
    fs << abiDef.abis[i].methodName << std::endl;
  }
  fs.close();

  LOGDEBUG << "copy ::" << tmpFile.string() << " to ::" << outPath.string();
  fs::copy_file(tmpFile, outPath, fs::copy_option::overwrite_if_exists);
}

void createContractFile(fs::path &randomDir, const string &srcPath,
                        const string &filename, const string &externC) {
  fs::path tmpFile = randomDir / filename;
  std::ofstream tmpStream(tmpFile.string().c_str());
  if (!tmpStream.is_open()) {
    throw Exception() << ErrStr("tmp src file is not open:")
                      << ErrStr(strerror(errno));
  }

  std::ifstream srcStream(srcPath);

  if (!srcStream.is_open()) {
    throw Exception() << ErrStr("src is not open:") << ErrStr(strerror(errno));
  }

  string line;
  bool skip = false;
  while (getline(srcStream, line)) {
    if (line.find("platon autogen end") != string::npos) {
      skip = false;
      continue;
    }
    if (line.find("platon autogen begin") != string::npos) {
      skip = true;
      continue;
    }
    if (!skip) {
      tmpStream << line << "\n";
    }
    line.clear();
  }

  tmpStream << externC;
  tmpStream.close();
  srcStream.close();

  LOGDEBUG << "swap src::" << srcPath << " tmp::" << tmpFile.string();
  // fs::remove(srcPath);
  llvm::SmallString<64> res;
  llvm::sys::path::system_temp_directory(true, res);
  std::string dst(std::string(res.c_str()) + "/" + filename);
  fs::copy_file(tmpFile, dst, fs::copy_option::overwrite_if_exists);
}
void prepareFile(const string &filename) {
  fstream fs(filename);
  string line;

  auto cur = fs.tellg();

  //    auto begin = fstream::pos_type(-1);
  //    auto end = fstream::pos_type(-1);

  vector<tuple<fstream::pos_type, string>> replaceText;
  string tailText;
  bool skip = false;
  bool append = false;
  while (getline(fs, line)) {
    if (append) {
      tailText.append(line);
    }
    if (line.find("platon autogen end") != string::npos) {
      // end = cur + fstream::pos_type(line.length());
      replaceText.push_back(tuple<fstream::pos_type, string>(cur, line));
      skip = false;
      append = true;
    }
    if (line.find("platon autogen begin") != string::npos) {
      // begin = cur;
      skip = true;
    }
    if (skip) {
      replaceText.push_back(tuple<fstream::pos_type, string>(cur, line));
    }
    line.clear();
    cur = fs.tellg();
  }

  if (replaceText.empty()) {
    return;
  };
  fs.close();

  fstream writeAnno(filename);
  if (!writeAnno.is_open()) {
    throw Exception() << ErrStr("open file failed");
  }
  string rt = "//";
  writeAnno.seekp(get<0>(replaceText[0]));
  for (int i = 0; i < replaceText.size(); i++) {
    writeAnno << rt << get<1>(replaceText[i]) << '\n';
  }
  writeAnno << tailText;
  //    writeAnno.seekp(end);
  //    writeAnno << "*/";
  //    writeAnno.seekp(begin);
  //    writeAnno << "/*//platon autogen begin";
  writeAnno.close();
}
int main(int argc, const char **argv) {
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &os) {
    os << "platon-abigen version "
       << "${VERSION_FULL}"
       << "\n";
  });
  try {
    tooling::CommonOptionsParser op(argc, argv, abiGeneratorOptions);

    if (op.getSourcePathList().size() > 1) {
      //throw Exception() << ErrStr("only one contract file");
      return -1;
    }
    prepareFile(op.getSourcePathList()[0]);

    tooling::ClangTool Tool(op.getCompilations(), op.getSourcePathList());

    llvm::SmallString<64> res;
    llvm::sys::path::system_temp_directory(true, res);
    fs::path randomDir(std::string(res.c_str()));
    randomDir = randomDir /
                randomString(20)
                    .c_str();  // "57298a3ci7d0g504ha00";// randomString(20);

    if (!fs::create_directories(randomDir)) {
      throw Exception() << ErrStr("create dir failed:")
                        << ErrStr(strerror(errno));
    }

    if (verbose) {
      if (logPath.empty()) {
        logPath = randomDir.string();
      }
      initLog(logPath, logLevel);
    }

    vector<string> action;
    ContractDef contractDef;

    LOGDEBUG << "start run";
    int result = Tool.run(CreateFindMacroFactory(contractDef, action).get());

    if (result != 0 || contractDef.name.empty()) {
      //throw Exception() << ErrStr("find macro PLATON_ABI failed");
      return -1;
    }

    LOGDEBUG << "result:" << result << "contract fullname:["
             << contractDef.fullName << "]";

    for (const string &act : action) {
      LOGDEBUG << "contract:" << contractDef.name << "  action:" << act;
    }

    ABIDef abiDef;
    result = Tool.run(createFactory(contractDef.name, action, abiDef).get());

    if (result != 0) {
      //throw Exception() << ErrStr("find contract abi defined failed");
      return -1;
    }

    LOGINFO << "find method success"
            << "find abi size:" << abiDef.abis.size();
    for (size_t i = 0; i < abiDef.abis.size(); ++i) {
      LOGDEBUG << "methodName:" << abiDef.abis[i].methodName << " args:(";
      for (size_t j = 0; j < abiDef.abis[i].args.size(); ++j) {
        LOGDEBUG << "name:" << abiDef.abis[i].args[j]
                 << ", typeName:" << abiDef.abis[i].types[j].typeName
                 << " realTypeName:" << abiDef.abis[i].types[j].realTypeName;
      }
      LOGDEBUG << ")";
      LOGDEBUG << "return typeName:" << abiDef.abis[i].returnType.typeName
               << " realTypeName:" << abiDef.abis[i].returnType.realTypeName;
    }

    LOGDEBUG << "start create abi json";

    string srcFilename =
        fs::path(op.getSourcePathList()[0]).filename().string();

    createJsonAbi(abiDef, contractDef, srcFilename, abigen_output_opt,
                  randomDir);
    createExportsFile(abiDef, srcFilename, exports_output_opt, randomDir);

    ABI initAbi;
    initAbi.methodName = "init";
    initAbi.returnType.realTypeName = "void";
    abiDef.abis.push_back(initAbi);

    string externC = generateAbiCPlusPlus(contractDef, abiDef);

    createContractFile(randomDir, op.getSourcePathList()[0], srcFilename,
                       externC);

  } catch (Exception e) {
    cerr << *boost::get_error_info<ErrStr>(e) << endl;
    cerr << e.what() << endl;
  }
  return 0;
}
