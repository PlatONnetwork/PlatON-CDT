
#include "llvm/IR/Type.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"
#include "MakeAbi.h"

#include <string>

using namespace llvm;

bool isFixedHash(DICompositeType *CT){
  return CT->getIdentifier().startswith("_ZTSN6platon9FixedHash");
}

bool isWideInteger(DICompositeType* CT){
  return CT->getIdentifier().startswith("_ZTSSt12wide_integer");
}

DIType* getTypeParam(DICompositeType* CT, unsigned i);
Value* getValueParam(DICompositeType* CT, unsigned i);


StringRef MakeAbi::handleFixedHash(DINode* Node, DICompositeType* CT){
  Value* V  = getValueParam(CT, 0);
  ConstantInt* CI = cast<ConstantInt>(V);

  std::string str;
  raw_string_ostream OS(str);
  OS << format("FixedHash<%llu>", CI->getZExtValue());
  OS.flush();

  return SSaver.save(str);
}

StringRef MakeAbi::handleWideInteger(DINode* Node, DICompositeType* CT){
  Value* V  = getValueParam(CT, 0);
  ConstantInt* CI = cast<ConstantInt>(V);

  DIType* T = getTypeParam(CT, 1);
  StringRef s = handleType(Node, T);

  std::string str;
  raw_string_ostream OS(str);
  OS << format("wide_integer<%llu,%s>", CI->getZExtValue(), s.data());
  OS.flush();

  return SSaver.save(str);
}
