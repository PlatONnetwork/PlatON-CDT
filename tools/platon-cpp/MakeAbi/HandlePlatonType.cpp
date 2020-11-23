
#include "MakeAbi.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace llvm;

bool isVectorRef(DICompositeType* CT) {
  return CT->getIdentifier().startswith("_ZTSN6platon10vector_refIKhEE");
  
}

bool isFixedHash(DICompositeType* CT) {
  return CT->getIdentifier().startswith("_ZTSN6platon9FixedHash");
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
