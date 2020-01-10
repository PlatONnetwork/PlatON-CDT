
#include "llvm/IR/Type.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/JSON.h"
#include <set>
#include <map>
#include <vector>

using namespace llvm;
using namespace json;
using namespace std;

json::Value handleType(DIType* DT);

bool isString(DIDerivedType* DerT){
  if (DerT->getTag() == llvm::dwarf::DW_TAG_typedef) {
    if (DerT->getName() == "string") {
      if (DICompositeType *CT = dyn_cast<DICompositeType>(DerT->getBaseType().resolve())) {
        if (CT->getIdentifier() == "_ZTSNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE"){
          return true;
        }
      }
    }
  }
  return false;
}

bool isVector(DICompositeType* CT){
  if (CT->getTemplateParams().get() != nullptr) {
    if (CT->getElements().size() > 1) {
      const MDOperand &Op = CT->getElements()->getOperand(1);
      if (const DISubprogram *SP = dyn_cast<DISubprogram>(Op.get())) {
        if (SP->getName() == "vector") {
          return true;
        }
      }
    }
  }
  return false;
}

DIType* getVectorParam(DICompositeType* CT){
  const MDOperand &Op = CT->getTemplateParams()->getOperand(0);
  const DITemplateTypeParameter* TP = cast<const DITemplateTypeParameter>(Op);
  return TP->getType().resolve();
}

json::Value handleElem(StringRef Name, DIType* DT){
  json::Value v = handleType(DT);
  llvm::json::Object vv = *v.getAsObject();
  vv["name"] = Name;
  return vv;
}

json::Value handleBasicType_(DIBasicType* BT){
  unsigned size = BT->getSizeInBits();
  switch(BT->getEncoding()){
    case llvm::dwarf::DW_ATE_float:
      report_fatal_error("can not support float type");

    case llvm::dwarf::DW_ATE_boolean:
      return "bool";

    case llvm::dwarf::DW_ATE_unsigned_char:
    case llvm::dwarf::DW_ATE_unsigned:
      return "uint<" + to_string(size) + ">";

    case llvm::dwarf::DW_ATE_signed_char:
    case llvm::dwarf::DW_ATE_signed:
      return "int<" + to_string(size) + ">";

    default:
      report_fatal_error("unknown base type");
  }
}

json::Value handleBasicType(DIBasicType* BT){
  return Object{{"type", handleBasicType_(BT)}};
}

json::Value handleDerivedType(DIDerivedType* DevT){

  if(isString(DevT))
    return Object{{"type", "bytes"}};

  DIType* BT = DevT->getBaseType().resolve();

  switch(DevT->getTag()){

    case dwarf::DW_TAG_reference_type:
    case dwarf::DW_TAG_typedef:
    case dwarf::DW_TAG_const_type:
      return handleType(BT);

    case dwarf::DW_TAG_member:
      return handleElem(DevT->getName(), BT);
    case dwarf::DW_TAG_inheritance:
      return handleElem("BaseClass", BT);

    case dwarf::DW_TAG_pointer_type:
      DevT->print(llvm::errs());
      report_fatal_error("can not handle Pointer Type");
    default:
      DevT->print(llvm::errs());
      report_fatal_error("can not handle DIDerivedType Tag");
  }
}

json::Value handleCompositeType(DICompositeType* CT){

  if(isVector(CT)){
    DIType* T = getVectorParam(CT);
    json::Value v = handleType(T);
    llvm::json::Object vv = *v.getAsObject();
    llvm::Optional<StringRef> OStr = vv["type"].getAsString();
    vv["type"] = OStr->str() + "[]";
    return vv;

  } else if(CT->getTag() == dwarf::DW_TAG_structure_type || CT->getTag() == dwarf::DW_TAG_class_type){
    json::Value Elems = {};
    for(DINode* DN : CT->getElements()){
      if(DIDerivedType* Elem = dyn_cast<DIDerivedType>(DN)){
        Elems.getAsArray()->push_back(handleDerivedType(Elem));
      }
    }
    return Object{
      {"type",       "tuple"},
      {"components", Elems}};

  } else {
    CT->print(llvm::errs());
    report_fatal_error("have not handle DICompositeType Tag");
  }
}

json::Value handleType(DIType* DT){
  if(DIBasicType* BT = dyn_cast<DIBasicType>(DT)){
    return handleBasicType(BT);
  } else if(DICompositeType* CT = dyn_cast<DICompositeType>(DT)){
    return handleCompositeType(CT);
  } else if(DIDerivedType* DevT = dyn_cast<DIDerivedType>(DT)){
    return handleDerivedType(DevT);
  } else if(isa<DISubroutineType>(DT)){
    report_fatal_error("can not pass function type");
  } else {
    llvm_unreachable("have not DIType SubClass");
  }
}

