
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

json::Value handleElem(StringRef Name, DIType* DT);


json::Value handleBasicType(DIBasicType* BT){
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


json::Value handleDerivedType(DIDerivedType* DevT){
  switch(DevT->getTag()){
    case dwarf::DW_TAG_member:
      return handleElem(DevT->getName(), DevT->getBaseType().resolve());

    case dwarf::DW_TAG_typedef:
      //TODO

    case dwarf::DW_TAG_pointer_type:
      DevT->print(llvm::errs());
      report_fatal_error("can not handle Pointer Type");
    default:
      DevT->print(llvm::errs());
      report_fatal_error("can not handle DIDerivedType Tag");
  }
}

json::Value handleCompositeType(DICompositeType* CT){
  if(CT->getTag() == dwarf::DW_TAG_structure_type || CT->getTag() == dwarf::DW_TAG_class_type){
    json::Value Params = {};
    for(DINode* DN : CT->getElements()){
      DIDerivedType* Elem = cast<DIDerivedType>(DN);
      Params.getAsArray()->push_back(handleDerivedType(Elem));
    }
    return Params;
  } else {
    CT->print(llvm::errs());
    report_fatal_error("have not handle DICompositeType Tag");
  }
}

json::Value handleSubprogram(DISubprogram*  SP, vector<DILocalVariable*> &LVs, StringRef SPType){
  
  DISubroutineType* ST = cast<DISubroutineType>(SP->getType());
  DITypeRefArray TRA = ST->getTypeArray();
  Metadata* RetType = TRA->getOperand(0).get();
  json::Value Ret = {};
  if(RetType){
    DIType* RetType1 = cast<DIType>(RetType);
    Ret = json::Array{handleElem("", RetType1)};
  }

  json::Value Params = {};
  for(DILocalVariable* LV : LVs){
    Params.getAsArray()->push_back(handleElem(LV->getName(), LV->getType().resolve()));
  }

  return Object{{"name", SP->getName()},
                {"type", SPType},
                {"input", Params},
                {"output", Ret}
                };
}
