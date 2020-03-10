
#include "llvm/IR/Type.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/FileSystem.h"
#include <map>
#include <vector>
#include "MakeAbi.h"

using namespace llvm;
using namespace json;
using namespace std;

StringRef getAnnoteKind(llvm::Value* cs) {
  if(ConstantStruct* CS=dyn_cast<ConstantStruct>(cs))
    if(CS->getNumOperands()>1)
      if(ConstantExpr* CE = dyn_cast<ConstantExpr>(CS->getAggregateElement(1)))
        if(CE->getNumOperands()>0)
          if(GlobalVariable* ActionString = dyn_cast<GlobalVariable>(CE->getOperand(0)))
            if(ActionString->hasInitializer())
              if (ConstantDataArray *arr = dyn_cast<ConstantDataArray>(ActionString->getInitializer()))
                return arr->getAsCString();

  return "";
}

DISubprogram* getFuncInfo(llvm::Value* cs){
  if(ConstantStruct* CS=dyn_cast<ConstantStruct>(cs)) {
    if (CS->getNumOperands() > 0)
      if (ConstantExpr *CE = dyn_cast<ConstantExpr>(CS->getAggregateElement((unsigned) 0)))
        if(CE->getNumOperands() > 0)
          if(Function* F = dyn_cast<Function>(CE->getOperand(0)))
            if(auto M = F->getMetadata(LLVMContext::MD_dbg))
              if(DISubprogram* DI = dyn_cast<DISubprogram>(M))
                return DI;
  }
  return nullptr;
}

typedef map<llvm::DISubprogram*, Attr> SubprogramMap;
typedef multimap<llvm::DISubprogram*, llvm::DILocalVariable*> ParamsMap;

void exportParams(DISubprogram* SP, ParamsMap &PMap, vector<DILocalVariable*> &Params){

  if(!isa<DICompositeType>(SP->getScope()))
    report_fatal_error("Action or Event function must be class member");

  unsigned num = SP->getType()->getTypeArray()->getNumOperands();

  Params.resize(num-2);

  for(auto iter = PMap.lower_bound(SP); iter != PMap.upper_bound(SP); iter++) {
    DILocalVariable* LV = iter->second;
    Params[LV->getArg()-2] = LV;
  }
}

void collectParams(Function* DbgDecl, SubprogramMap &SPMap, ParamsMap &PMap){
  if(DbgDecl){
    for(User* U : DbgDecl->users()){
      if(CallInst* CI = dyn_cast<CallInst>(U)){
        MetadataAsValue* MV = cast<MetadataAsValue>(CI->getOperand(1));
        DILocalVariable* LV = cast<DILocalVariable>(MV->getMetadata());
        if(LV->isParameter() && LV->getArg()>1)
          if(DISubprogram* SP = dyn_cast<DISubprogram>(LV->getScope()))
            if(SPMap.find(SP) != SPMap.end())
              PMap.insert(make_pair(SP, LV));
      }
    }
  }
}

void collectRetainedNodes(SubprogramMap &SPMap, ParamsMap &PMap){
  for(auto iter : SPMap){
    DISubprogram* SP = iter.first; 
    for(DINode* Node : SP->getRetainedNodes()){
      DILocalVariable* LV = cast<DILocalVariable>(Node);
      if(LV->isParameter() && LV->getArg()>1)
        PMap.insert(make_pair(SP, LV));
    }
  }
}

Attr DedupKind(Attr attr0, Attr attr1){
  AttrKind Kind0 = attr0.first;
  AttrKind Kind1 = attr1.first;
  unsigned num0 = attr0.second;
  unsigned num1 = attr1.second;

  if(Kind0==ActionKind && Kind1==ActionKind)
    return make_pair(ActionKind, num0 || num1);
  else if(Kind0==EventKind && Kind1==EventKind){
    if(num0==num1)return attr0;
    else report_fatal_error("Event dismatch");
  } else
    report_fatal_error("attr dismatch");
}

Attr parseAttr(StringRef Kind){
  if(Kind == "Action")
    return make_pair(ActionKind, 0);
  else if(Kind == "Const")
    return make_pair(ActionKind, 1);
  else if(Kind.startswith("Event")){
    APInt r;
    if(!Kind.drop_front(5).getAsInteger(10, r)){
      return make_pair(EventKind, r.getZExtValue());
    }
  }
  return make_pair(OtherKind, 0);
}

void collectAnnote(GlobalVariable* annote, SubprogramMap &SPMap){
  if(annote){
    if(ConstantArray* annotes = dyn_cast<ConstantArray>(annote->getInitializer())) {
      for (auto cs:annotes->operand_values()) {
        DISubprogram* SP = getFuncInfo(cs);

        Attr attr = parseAttr(getAnnoteKind(cs));

        if(attr.first != OtherKind){
          if(SPMap.find(SP)==SPMap.end())
            SPMap[SP] = attr;
          else 
            SPMap[SP] = DedupKind(SPMap[SP], attr);
        }
      }
    }
  }
}

void makeAbi(Module* M, MakeAbi &MABI){

  SubprogramMap SPMap;
  ParamsMap PMap;

  GlobalVariable* Annote = M->getGlobalVariable("llvm.global.annotations");
  collectAnnote(Annote, SPMap);

  if(SPMap.size()==0)
    report_fatal_error("have not define Action or Event function");

  collectRetainedNodes(SPMap, PMap);
  collectParams(M->getFunction("llvm.dbg.declare"), SPMap, PMap);
  collectParams(M->getFunction("llvm.dbg.value"), SPMap, PMap);

  for(auto iter : SPMap){
    DISubprogram* SP = iter.first;
    auto SPKind = iter.second;

    vector<DILocalVariable*> Params;
    exportParams(SP, PMap, Params);
    MABI.handleSubprogram(SP, Params, SPKind);
  }
}

MakeAbi::MakeAbi():SSaver(Alloc){}

int GenerateABI(std::string &WasmOutput, llvm::Module* M){
  SmallString<128> abiPath(WasmOutput);
  llvm::sys::path::replace_extension(abiPath, "abi.json");

  MakeAbi MABI;
  makeAbi(M, MABI);

  std::error_code EC;
  ToolOutputFile Out(abiPath, EC, sys::fs::F_None);
    
  if(EC)
    report_fatal_error(EC.message());

  Out.os() << llvm::formatv("{0:4}", MABI.contents);
  Out.keep();

  return 0;
}

