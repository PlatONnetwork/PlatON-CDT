
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

json::Value handleBasicType(DIBasicType* BT);
json::Value handleCompositeType(DICompositeType* CT);

bool isString(DIType* DT){
  if(DIDerivedType* DerT = dyn_cast<DIDerivedType>(DT)) {
    if (DerT->getTag() == llvm::dwarf::DW_TAG_typedef) {
      if (DerT->getName() == "string") {
        if (DICompositeType *CT = dyn_cast<DICompositeType>(DerT->getBaseType().resolve())) {
          if (CT->getIdentifier() == "_ZTSNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE"){
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool isVector(DIType* DT){
  if(DICompositeType* CT = dyn_cast<DICompositeType>(DT)) {
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
  }
  return false;
}

DIType* getVectorParam(DIType* DT){
  DICompositeType* CT = dyn_cast<DICompositeType>(DT);
  const MDOperand &Op = CT->getTemplateParams()->getOperand(0);
  const DITemplateTypeParameter* TP = cast<const DITemplateTypeParameter>(Op);
  return TP->getType().resolve();
}

json::Value handleElem(StringRef Name, DIType* DT){
  if(isString(DT)){
    return "bytes";

  } else if(isVector(DT)){
    DIType* T = getVectorParam(DT);
    json::Value v = handleElem(Name, T);
    llvm::json::Object vv = *v.getAsObject();
    llvm::Optional<StringRef> OStr = vv["type"].getAsString();
    vv["type"] = OStr->str() + "[]";
    return vv;

  } else if(DIBasicType* BT = dyn_cast<DIBasicType>(DT)){
      return Object{
        {"name", Name},
        {"type", handleBasicType(BT)}};

  } else if(DICompositeType* CT = dyn_cast<DICompositeType>(DT)){
      return Object{
        {"name",       Name},
        {"type",       "tuple"},
        {"components", handleCompositeType(CT)}};

  } else if(isa<DIDerivedType>(DT)){
    DT->print(llvm::outs());
    report_fatal_error("can not pass pointer type");
  } else if(isa<DISubroutineType>(DT)){
    DT->print(llvm::outs());
    report_fatal_error("can not pass function");
  } else {
    DT->print(llvm::outs());
    llvm_unreachable("have not DIType SubClass");
  }
}
