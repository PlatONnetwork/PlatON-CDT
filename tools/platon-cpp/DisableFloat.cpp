#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugInfoMetadata.h"

using namespace llvm;


struct DisableFloatsPass : public llvm::FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  DisableFloatsPass() : FunctionPass(ID) {}
  bool runOnFunction(llvm::Function &F) override;
};

void assertFloat(Type* T, StringRef file, unsigned line ){
  if(T->isFloatTy() || T->isDoubleTy()){
    if(!file.empty())
      llvm::outs() << file << ":" << line << ": ";

    report_fatal_error("can not use float type\n");
  }
}

void checkFunctionType(DISubprogram* DI, FunctionType* FT){
  StringRef file = DI->getFile()->getFilename();
  assertFloat(FT->getReturnType(), file, DI->getLine());
  for(Type* T : FT->params()){
    assertFloat(T, file, DI->getLine());
  }
}

void checkInstruction(const Instruction &Inst){

  MDNode* Node = Inst.getMetadata(LLVMContext::MD_dbg);
  if(Node){
    if(DILocation* Loc = dyn_cast<DILocation>(Node)){
      assertFloat(Inst.getType(), Loc->getFilename(), Loc->getLine());
      return;
    }
  }

  assertFloat(Inst.getType(), "", 0);
}

bool DisableFloatsPass::runOnFunction(Function &F) {

  DISubprogram* DI = cast<DISubprogram>(F.getMetadata(LLVMContext::MD_dbg));
  checkFunctionType(DI, F.getFunctionType());
  
  for(const BasicBlock &BB : F)
    for(const Instruction &Inst : BB)
      checkInstruction(Inst);

  return false;
}


char DisableFloatsPass::ID = 0;

static RegisterPass<DisableFloatsPass> X("disable-float", "check float use");

FunctionPass* createDisableFloatsPass (){
  return new DisableFloatsPass();
}
