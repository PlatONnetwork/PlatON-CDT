#include "llvm/ADT/Statistic.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"

using namespace llvm;

#define DEBUG_TYPE "RemoveAttrs"

STATISTIC(AttrCounter, "Counts number of Attribute");

struct RemoveAttrsPass : public llvm::FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  RemoveAttrsPass() : FunctionPass(ID) {}
  bool runOnFunction(llvm::Function &F) override;
};

bool RemoveAttrsPass::runOnFunction(Function &F) {
  if(!F.hasFnAttribute(Attribute::OptimizeNone) && !F.hasFnAttribute(Attribute::NoInline))
    return false;

  F.removeFnAttr(Attribute::OptimizeNone);
  F.removeFnAttr(Attribute::NoInline);

  ++AttrCounter;
  return true;
}


char RemoveAttrsPass::ID = 0;

static RegisterPass<RemoveAttrsPass> X("remove-attrs", "Remove all attributes");

FunctionPass* createRemoveAttrsPass (){
  return new RemoveAttrsPass();
}
