
#include <string>
#include "MakeAbi.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

bool isVectorRef(DICompositeType* CT) {
  return CT->getIdentifier().startswith("_ZTSN6platon10vector_refIKhEE");
}

bool isFixedHash(DICompositeType* CT) {
  return CT->getIdentifier().startswith("_ZTSN6platon9FixedHash");
}

bool isBigInt(DICompositeType* CT) {
  return CT->getIdentifier().startswith("_ZTSSt11WideInteger");
}

DIType* getTypeParam(DICompositeType* CT, unsigned i);
Value* getValueParam(DICompositeType* CT, unsigned i);

StringRef MakeAbi::handleFixedHash(DINode* Node, DICompositeType* CT) {
  Value* V = getValueParam(CT, 0);
  ConstantInt* CI = cast<ConstantInt>(V);

  std::string str;
  raw_string_ostream OS(str);
  OS << format("FixedHash<%llu>", CI->getZExtValue());
  OS.flush();

  return SSaver.save(str);
}

StringRef MakeAbi::handleBigint(DINode* Node, DICompositeType* CT) {
  ConstantInt* bits = cast<ConstantInt>(getValueParam(CT, 0));
  ConstantInt* sign = cast<ConstantInt>(getValueParam(CT, 1));
  if (bits->getZExtValue() != 256 && bits->getZExtValue() != 512) {
    char error[100];
    snprintf(error, sizeof(error), "does not support %lu-bit big int",
             bits->getZExtValue());
    report_fatal_error(error);
  }
  std::string str;
  raw_string_ostream OS(str);
  OS << format("%s%llu", sign->getZExtValue() == 0 ? "uint" : "int",
               bits->getZExtValue());
  OS.flush();

  return SSaver.save(str);
}
