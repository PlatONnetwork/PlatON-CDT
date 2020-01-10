
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

using namespace llvm;
using namespace json;
using namespace std;

json::Value handleType(DINode* Node, DIType* DT);
json::Value handleElem(DINode* Node, DIType* DT);

typedef bool isEvent;
typedef bool isConst;
typedef pair<isEvent, isConst> Attr;

json::Value handleSubprogram(DISubprogram*  SP, vector<DILocalVariable*> &LVs, Attr SPKind){
  
  DISubroutineType* ST = cast<DISubroutineType>(SP->getType());
  DITypeRefArray TRA = ST->getTypeArray();
  Metadata* RetType = TRA->getOperand(0).get();
  json::Value Ret = {};
  if(RetType){
    DIType* RetType1 = cast<DIType>(RetType);
    Ret = json::Array{handleType(SP, RetType1)};
  }

  json::Value Params = {};
  for(DILocalVariable* LV : LVs){
    json::Value elem = handleElem(LV, LV->getType().resolve());
    Params.getAsArray()->push_back(elem);
  }

  return Object{{"name", SP->getName()},
                {"input", Params},
                {"output", Ret},
                {"type", SPKind.first?"event":"function"},
                {"constant", SPKind.second}
                };
}

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

void collectAnnote(GlobalVariable* annote, SubprogramMap &SPMap){
  if(annote){
    if(ConstantArray* annotes = dyn_cast<ConstantArray>(annote->getInitializer())) {
      for (auto cs:annotes->operand_values()) {
        DISubprogram* SP = getFuncInfo(cs);
        StringRef kind = getAnnoteKind(cs);

        if(kind == "Action")
          SPMap[SP].first = false;
        else if(kind == "Event")
          SPMap[SP].first = true;
        else if(kind == "Const")
          SPMap[SP].second = true;
      }
    }
  }
}


json::Value makeAbi(Module* M){

  SubprogramMap SPMap;
  ParamsMap PMap;

  GlobalVariable* Annote = M->getGlobalVariable("llvm.global.annotations");
  collectAnnote(Annote, SPMap);

  if(SPMap.size()==0)
    report_fatal_error("have not Action and Event");

  collectParams(M->getFunction("llvm.dbg.declare"), SPMap, PMap);
  collectParams(M->getFunction("llvm.dbg.value"), SPMap, PMap);

  json::Value Funcs = {};

  for(auto iter : SPMap){
    DISubprogram* SP = iter.first;
    auto SPKind = iter.second;

    vector<DILocalVariable*> Params;
    exportParams(SP, PMap, Params);
    json::Value func = handleSubprogram(SP, Params, SPKind);

    Funcs.getAsArray()->push_back(func);
  }

  return Funcs;
}

int GenerateABI(std::string &WasmOutput, llvm::Module* M){
  SmallString<128> abiPath(WasmOutput);
  llvm::sys::path::replace_extension(abiPath, "abi.json");

  json::Value v = makeAbi(M);

  std::error_code EC;
  ToolOutputFile Out(abiPath, EC, sys::fs::F_None);
  if (EC) {
    errs() << EC.message() << '\n';
    return 1; 
  }
  Out.os() << llvm::formatv("{0:4}", v);
  Out.keep();

  return 0;
}

