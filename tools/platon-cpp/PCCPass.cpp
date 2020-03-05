
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/InitializePasses.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;


static void AddOptimizationPasses(legacy::PassManagerBase &MPM,
                                  legacy::FunctionPassManager &FPM,
                                  unsigned OptLevel,
                                  unsigned SizeLevel) {
  FPM.add(createVerifierPass()); // Verify that input is correct

  PassManagerBuilder Builder;
  Builder.OptLevel = OptLevel;
  Builder.SizeLevel = SizeLevel;

  Builder.Inliner = createFunctionInliningPass(OptLevel, SizeLevel, false);
  Builder.DisableUnitAtATime = false;
  Builder.DisableUnrollLoops = true;
  Builder.LoopVectorize = false;
  Builder.SLPVectorize = false;

  Builder.populateFunctionPassManager(FPM);
  Builder.populateModulePassManager(MPM);
}

FunctionPass* createRemoveAttrsPass ();
FunctionPass* createDisableFloatsPass ();

void PCCPass(llvm::Module &M){

  // Initialize passes
  PassRegistry &Registry = *PassRegistry::getPassRegistry();
  initializeCore(Registry);
  initializeCoroutines(Registry);
  initializeScalarOpts(Registry);
  initializeVectorization(Registry);
  initializeIPO(Registry);
  initializeAnalysis(Registry);
  initializeTransformUtils(Registry);
  initializeInstCombine(Registry);
  initializeInstrumentation(Registry);
  initializeTarget(Registry);

  legacy::PassManager PrePasses;
  legacy::PassManager Passes;
  legacy::FunctionPassManager FPasses(&M);

  PrePasses.add(createRemoveAttrsPass());
  auto PreserveMain = [=](const GlobalValue &GV) { return GV.getName() == "invoke";};
  PrePasses.add(createInternalizePass(PreserveMain));

  PrePasses.run(M);

  FPasses.add(createTargetTransformInfoWrapperPass(TargetIRAnalysis()));
  
  AddOptimizationPasses(Passes, FPasses, 2, 2);

  FPasses.doInitialization();
  for (Function &F : M)
    FPasses.run(F);
  FPasses.doFinalization();

  Passes.add(createDisableFloatsPass());

  Passes.add(createVerifierPass());
  Passes.run(M);

  StripDebugInfo(M);
}
