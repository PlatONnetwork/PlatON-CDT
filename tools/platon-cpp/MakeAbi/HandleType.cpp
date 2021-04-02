
#include <map>
#include <set>
#include <vector>
#include "MakeAbi.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/JSON.h"

using namespace llvm;
using namespace json;
using namespace std;

json::Value handleType(DINode* Node, DIType* DT);

bool isString(DICompositeType*);

bool isVector(DICompositeType*);
bool isArray(DICompositeType*);

bool isSet(DICompositeType*);
bool isList(DICompositeType*);

bool isMap(DICompositeType*);
bool isPair(DICompositeType*);

bool isFixedHash(DICompositeType*);
bool isVectorRef(DICompositeType*);
bool isBigInt(DICompositeType*);
StringRef getName(DINode* Node) {
  if (DILocalVariable* LV = dyn_cast<DILocalVariable>(Node)) {
    return LV->getName();
  } else if (DIDerivedType* DerT = dyn_cast<DIDerivedType>(Node)) {
    if (DerT->getTag() == dwarf::DW_TAG_member)
      return DerT->getName();
    else if (DerT->getTag() == dwarf::DW_TAG_inheritance)
      return "BaseClass";
    else
      llvm_unreachable("unknown variable");
  } else {
    llvm_unreachable("unknown variable");
  }
}

void report_error(DINode* Node) {
  StringRef Name;
  DIFile* File;
  unsigned Line;

  if (DILocalVariable* LV = dyn_cast<DILocalVariable>(Node)) {
    Name = LV->getName();
    File = LV->getFile();
    Line = LV->getLine();
  } else if (DIDerivedType* DerT = dyn_cast<DIDerivedType>(Node)) {
    Name = DerT->getName();
    File = DerT->getFile();
    Line = DerT->getLine();
  } else if (DISubprogram* SP = dyn_cast<DISubprogram>(Node)) {
    Name = SP->getName();
    File = SP->getFile();
    Line = SP->getLine();
    llvm::errs() << "function return type can not support\n";
  } else if (DICompositeType* CT = dyn_cast<DICompositeType>(Node)) {
    Name = CT->getName();
    File = CT->getFile();
    Line = CT->getLine();
  } else {
    llvm_unreachable("unknown error");
  }
  llvm::errs() << File->getFilename() << ":" << Line << "\n";
  llvm::errs() << "Variable: " << Name << "\n";
}

json::Value MakeAbi::handleElem(DINode* Node, DIType* DT) {
  StringRef s = handleType(Node, DT);
  json::Value type = s;
  return Object{{"name", getName(Node)}, {"type", type}};
}

StringRef MakeAbi::handleBasicType(DINode* Node, DIBasicType* BT) {
  unsigned size = BT->getSizeInBits();
  string str;
  switch (BT->getEncoding()) {
    case llvm::dwarf::DW_ATE_boolean:
      return "bool";

    case llvm::dwarf::DW_ATE_unsigned_char:
    case llvm::dwarf::DW_ATE_unsigned:
      str = "uint";
      str += to_string(size);
      return SSaver.save(str);

    case llvm::dwarf::DW_ATE_signed_char:
    case llvm::dwarf::DW_ATE_signed:
      str = "int";
      str += to_string(size);
      return SSaver.save(str);

    case llvm::dwarf::DW_ATE_float:
      report_error(Node);
      report_fatal_error("can not use float or double type\n");

    default:
      report_error(Node);
      report_fatal_error("unknown base type");
  }
}

StringRef MakeAbi::handleDerivedType(DINode* Node, DIDerivedType* DevT) {
  DIType* BT = DevT->getBaseType();

  switch (DevT->getTag()) {
    case dwarf::DW_TAG_reference_type:
    case dwarf::DW_TAG_typedef:
    case dwarf::DW_TAG_const_type:
      return handleType(Node, BT);

    case dwarf::DW_TAG_pointer_type:
      report_error(Node);
      report_fatal_error("can not handle Pointer Type");
    default:
      report_error(Node);
      report_fatal_error("can not handle DIDerivedType Tag");
  }
}

StringRef MakeAbi::handleStructType(DINode* Node, DICompositeType* CT) {
  {
    auto getValueParam = [](DICompositeType* CT, unsigned i) -> llvm::Value* {
      if (CT->getTemplateParams().get() == nullptr)
        report_fatal_error("has not value params");

      const MDOperand& Op = CT->getTemplateParams()->getOperand(i);
      const DITemplateValueParameter* VP =
          cast<const DITemplateValueParameter>(Op);
      ValueAsMetadata* MV = cast<ValueAsMetadata>(VP->getValue());
      return MV->getValue();
    };
    llvm::Value* V1 = getValueParam(CT, 0);
    ConstantInt* CI1 = cast<ConstantInt>(V1);
    llvm::Value* V2 = getValueParam(CT, 1);
    ConstantInt* CI2 = cast<ConstantInt>(V2);
    printf("CT->getIdentifier():%s-%lu-%lu\n", CT->getIdentifier().data(),
           CI1->getZExtValue(), CI2->getZExtValue());
  }
  json::Value Elems = {};
  json::Value BaseClass = {};

  if (CT->getElements().get() == nullptr) {
    report_error(Node);
    report_fatal_error("StructType have define but never used");
  }

  for (DINode* DN : CT->getElements()) {
    if (DIDerivedType* Elem = dyn_cast<DIDerivedType>(DN)) {
      DIType* BT = Elem->getBaseType();
      if (Elem->getTag() == dwarf::DW_TAG_member) {
        json::Value e = handleElem(Elem, BT);
        Elems.getAsArray()->push_back(e);
      } else if (Elem->getTag() == dwarf::DW_TAG_inheritance) {
        StringRef c = handleType(Elem, BT);
        BaseClass.getAsArray()->push_back(c);
      }
    }
  }
  json::Value v = json::Object{{"name", CT->getName()},
                               {"type", "struct"},
                               {"baseclass", BaseClass},
                               {"fields", Elems}};

  contents.getAsArray()->push_back(v);
  return CT->getName();
}

StringRef MakeAbi::handleCompositeType(DINode* Node, DICompositeType* CT) {
  if (isString(CT)) {
    return "string";

  } else if (isVector(CT)) {
    return handleVector(Node, CT);

  } else if (isArray(CT)) {
    return handleArray(Node, CT);

  } else if (isPair(CT)) {
    return handleStd2(Node, CT, "pair");

  } else if (isSet(CT)) {
    return handleStd1(Node, CT, "set");

  } else if (isMap(CT)) {
    return handleStd2(Node, CT, "map");

  } else if (isList(CT)) {
    return handleStd1(Node, CT, "list");

  } else if (isFixedHash(CT)) {
    return handleFixedHash(Node, CT);

  } else if (isVectorRef(CT)) {
    return "uint8[]";

  } else if (isBigInt(CT)) {
    return handleBigint(Node, CT);
  } else if (CT->getTag() == dwarf::DW_TAG_structure_type ||

             CT->getTag() == dwarf::DW_TAG_class_type) {
    return handleStructType(Node, CT);

  } else {
    report_error(Node);
    report_fatal_error("have not handle DICompositeType Tag");
  }
}

StringRef MakeAbi::handleType_(DINode* Node, DIType* DT) {
  if (DIBasicType* BT = dyn_cast<DIBasicType>(DT)) {
    return handleBasicType(Node, BT);
  } else if (DICompositeType* CT = dyn_cast<DICompositeType>(DT)) {
    return handleCompositeType(Node, CT);
  } else if (DIDerivedType* DevT = dyn_cast<DIDerivedType>(DT)) {
    return handleDerivedType(Node, DevT);
  } else if (isa<DISubroutineType>(DT)) {
    report_error(Node);
    report_fatal_error("can not pass function type");
  } else {
    llvm_unreachable("have not DIType SubClass");
  }
}

StringRef MakeAbi::handleType(DINode* Node, DIType* DT) {
  auto iter = TypeMap.find(DT);
  if (iter != TypeMap.end()) {
    return iter->second;
  } else {
    StringRef s = handleType_(Node, DT);
    TypeMap[DT] = s;
    return s;
  }
}

json::Value MakeAbi::handleAction(DISubprogram* SP, json::Value Params,
                                  bool isConst) {
  DISubroutineType* ST = cast<DISubroutineType>(SP->getType());
  DITypeRefArray TRA = ST->getTypeArray();
  Metadata* RetType = TRA->getOperand(0).get();

  json::Value Ret = RetType ? handleType(SP, cast<DIType>(RetType)) : "void";

  return Object{{"name", SP->getName()},
                {"input", Params},
                {"output", Ret},
                {"type", "Action"},
                {"constant", isConst}};
}

json::Value MakeAbi::handleEvent(DISubprogram* SP, json::Value Params,
                                 unsigned num) {
  DISubroutineType* ST = cast<DISubroutineType>(SP->getType());
  DITypeRefArray TRA = ST->getTypeArray();
  Metadata* RetType = TRA->getOperand(0).get();

  if (RetType != nullptr) report_fatal_error("event can not hava return value");

  return Object{{"name", SP->getName()},
                {"input", Params},
                {"type", "Event"},
                {"topic", num},
                {"anonymous", false}};
}

void MakeAbi::handleSubprogram(DISubprogram* SP, vector<DILocalVariable*>& LVs,
                               Attr attr) {
  json::Value Params = {};
  for (DILocalVariable* LV : LVs) {
    DIType* T = LV->getType();
    json::Value elem = handleElem(LV, T);
    Params.getAsArray()->push_back(elem);
  }

  AttrKind kind = attr.first;
  unsigned attrnum = attr.second;

  json::Value v = kind == ActionKind ? handleAction(SP, Params, attrnum)
                                     : handleEvent(SP, Params, attrnum);

  contents.getAsArray()->push_back(v);
}
