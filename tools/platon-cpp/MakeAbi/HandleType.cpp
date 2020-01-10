
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

json::Value handleType(DINode* Node, DIType* DT);

bool isString(DIDerivedType* DerT){
  if (DerT->getTag() == llvm::dwarf::DW_TAG_typedef)
    if (DerT->getName() == "string") 
      if(auto M = DerT->getBaseType().resolve())
        if (DICompositeType *CT = dyn_cast<DICompositeType>(M)) 
          if (CT->getIdentifier() == "_ZTSNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE")
            return true;
  return false;
}

bool isVector(DICompositeType* CT){
  if (CT->getTemplateParams().get() != nullptr) {
    if (CT->getElements().size() > 1) {
      const MDOperand &Op = CT->getElements()->getOperand(1);
      if (const DISubprogram *SP = dyn_cast<DISubprogram>(Op.get()))
        if (SP->getName() == "vector")
          return true;
    }
  }
  return false;
}

DIType* getVectorParam(DICompositeType* CT){
  const MDOperand &Op = CT->getTemplateParams()->getOperand(0);
  const DITemplateTypeParameter* TP = cast<const DITemplateTypeParameter>(Op);
  return TP->getType().resolve();
}

StringRef getName(DINode* Node){
  if(DILocalVariable* LV = dyn_cast<DILocalVariable>(Node)){ 
    return LV->getName();
  } else if(DIDerivedType* DerT = dyn_cast<DIDerivedType>(Node)){
    return DerT->getName();
  } else {
    llvm_unreachable("unknown variable");
  }
}

void report_error(DINode* Node){
  StringRef Name;
  DIFile* File;
  unsigned Line;

  if(DILocalVariable* LV = dyn_cast<DILocalVariable>(Node)){ 
    Name = LV->getName();
    File = LV->getFile();
    Line = LV->getLine();
  } else if(DIDerivedType* DerT = dyn_cast<DIDerivedType>(Node)){
    Name = DerT->getName();
    File = DerT->getFile();
    Line = DerT->getLine();
  } else if(DISubprogram* SP = dyn_cast<DISubprogram>(Node)){
    Name = SP->getName();
    File = SP->getFile();
    Line = SP->getLine();
    llvm::errs() << "function return type can not support\n";
  }
  llvm::outs() << File->getFilename() << ":" << Line << "\n";
  llvm::outs() << "Variable: " << Name << "\n";
}

json::Value handleElem(DINode* Node, DIType* DT){
  json::Value v = handleType(Node, DT);
  llvm::json::Object vv = *v.getAsObject();
  vv["name"] = getName(Node);
  return vv;
}

json::Value handleBasicType_(DINode* Node, DIBasicType* BT){
  unsigned size = BT->getSizeInBits();
  switch(BT->getEncoding()){

    case llvm::dwarf::DW_ATE_boolean:
      return "bool";

    case llvm::dwarf::DW_ATE_unsigned_char:
    case llvm::dwarf::DW_ATE_unsigned:
      return "uint<" + to_string(size) + ">";

    case llvm::dwarf::DW_ATE_signed_char:
    case llvm::dwarf::DW_ATE_signed:
      return "int<" + to_string(size) + ">";

    case llvm::dwarf::DW_ATE_float:
      report_error(Node);
      report_fatal_error("can not support float type");

    default:
      report_error(Node);
      report_fatal_error("unknown base type");
  }
}

json::Value handleBasicType(DINode* Node, DIBasicType* BT){
  return Object{{"type", handleBasicType_(Node, BT)}};
}

json::Value handleDerivedType(DINode* Node, DIDerivedType* DevT){

  if(isString(DevT))
    return Object{{"type", "bytes"}};

  DIType* BT = DevT->getBaseType().resolve();

  switch(DevT->getTag()){

    case dwarf::DW_TAG_reference_type:
    case dwarf::DW_TAG_typedef:
    case dwarf::DW_TAG_const_type:
      return handleType(Node, BT);

    case dwarf::DW_TAG_member:
      return handleElem(DevT, BT);
    case dwarf::DW_TAG_inheritance:
      return handleElem(DevT, BT);

    case dwarf::DW_TAG_pointer_type:
      report_error(Node);
      report_fatal_error("can not handle Pointer Type");
    default:
      report_error(Node);
      report_fatal_error("can not handle DIDerivedType Tag");
  }
}

json::Value handleCompositeType(DINode* Node, DICompositeType* CT){

  if(isVector(CT)){
    DIType* T = getVectorParam(CT);
    json::Value v = handleType(Node, T);
    llvm::json::Object vv = *v.getAsObject();
    llvm::Optional<StringRef> OStr = vv["type"].getAsString();
    vv["type"] = OStr->str() + "[]";
    return vv;

  } else if(CT->getTag() == dwarf::DW_TAG_structure_type || CT->getTag() == dwarf::DW_TAG_class_type){
    json::Value Elems = {};
    for(DINode* DN : CT->getElements()){
      if(DIDerivedType* Elem = dyn_cast<DIDerivedType>(DN)){
        json::Value v = handleDerivedType(Elem, Elem);
        Elems.getAsArray()->push_back(v);
      }
    }
    return Object{
      {"type",       "tuple"},
      {"components", Elems}};

  } else {
    report_error(Node);
    report_fatal_error("have not handle DICompositeType Tag");
  }
}

json::Value handleType(DINode* Node, DIType* DT){
  if(DIBasicType* BT = dyn_cast<DIBasicType>(DT)){
    return handleBasicType(Node, BT);
  } else if(DICompositeType* CT = dyn_cast<DICompositeType>(DT)){
    return handleCompositeType(Node, CT);
  } else if(DIDerivedType* DevT = dyn_cast<DIDerivedType>(DT)){
    return handleDerivedType(Node, DevT);
  } else if(isa<DISubroutineType>(DT)){
    report_error(Node);
    report_fatal_error("can not pass function type");
  } else {
    llvm_unreachable("have not DIType SubClass");
  }
}

