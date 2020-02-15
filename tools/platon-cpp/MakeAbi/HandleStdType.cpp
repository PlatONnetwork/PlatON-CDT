
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

bool isString(DICompositeType *CT){
  return CT->getIdentifier() == "_ZTSNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE";
}

bool isVector(DICompositeType* CT){
  return CT->getIdentifier().startswith("_ZTSNSt3__16vector");
}

bool isPair(DICompositeType* CT){
  return CT->getIdentifier().startswith("_ZTSNSt3__14pair");
}

bool isSet(DICompositeType* CT){
  return CT->getIdentifier().startswith("_ZTSNSt3__13set");
}

bool isArray(DICompositeType* CT){
  return CT->getIdentifier().startswith("_ZTSNSt3__15array");
}

bool isMap(DICompositeType* CT){
  return CT->getIdentifier().startswith("_ZTSNSt3__13map");
}

bool isList(DICompositeType* CT){
  return CT->getIdentifier().startswith("_ZTSNSt3__14list");
}

DIType* getTypeParam(DICompositeType* CT, unsigned i){
  if(CT->getTemplateParams().get() == nullptr)
    report_fatal_error("has not type params");

  const MDOperand &Op = CT->getTemplateParams()->getOperand(i);
  const DITemplateTypeParameter* TP = cast<const DITemplateTypeParameter>(Op);

  return TP->getType().resolve();
}

Value* getValueParam(DICompositeType* CT, unsigned i){
  if(CT->getTemplateParams().get() == nullptr)
    report_fatal_error("has not value params");

  const MDOperand &Op = CT->getTemplateParams()->getOperand(i);
  const DITemplateValueParameter* VP = cast<const DITemplateValueParameter>(Op);
  ValueAsMetadata* MV = cast<ValueAsMetadata>(VP->getValue());
  return MV->getValue();
}


StringRef MakeAbi::handleVector(DINode* Node, DICompositeType* CT){

  DIType* T = getTypeParam(CT, 0);
  StringRef s = handleType(Node, T);

  std::string str;
  raw_string_ostream OS(str);
  OS << format("%s[]", s.data());
  OS.flush();
  return SSaver.save(str);
}

StringRef MakeAbi::handleArray(DINode* Node, DICompositeType* CT){

  DIType* T = getTypeParam(CT, 0);
  StringRef s = handleType(Node, T);

  Value* V  = getValueParam(CT, 1);
  ConstantInt* CI = cast<ConstantInt>(V);

  std::string str;
  raw_string_ostream OS(str);
  OS << format("%s[%llu]", s.data(), CI->getZExtValue());
  OS.flush();
  return SSaver.save(str);
}

StringRef MakeAbi::handleStd1(DINode* Node, DICompositeType* CT, char* name){

  DIType* T = getTypeParam(CT, 0);
  StringRef s = handleType(Node, T);

  std::string str;
  raw_string_ostream OS(str);
  OS << format("%s<%s>", name, s.data());
  OS.flush();

  return SSaver.save(str);
}

StringRef MakeAbi::handleStd2(DINode* Node, DICompositeType* CT, char* name){

  DIType* T0 = getTypeParam(CT, 0);
  StringRef s0 = handleType(Node, T0);

  DIType* T1 = getTypeParam(CT, 1);
  StringRef s1 = handleType(Node, T1);

  std::string str;
  raw_string_ostream OS(str);
  OS << format("%s<%s,%s>", name, s0.data(), s1.data());
  OS.flush();
  return SSaver.save(str);
}
