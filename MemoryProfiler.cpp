#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  // Pass itself
  struct MemoryProfiler : public ModulePass {
    static char pid;
    MemoryProfiler() : ModulePass(pid) {}

    // Main pass function
    virtual bool runOnModule(Module &M) {
      Function * print = Function::Create(
        FunctionType::get(TypeBuilder<int(char *, ...),false>::get(M.getContext()), true),
        Function::ExternalLinkage,
        "printf",
        &M
      );

      Constant * loadString = ConstantDataArray::getString(
        M.getContext(),
        "I am loading address %p\n"
      );

      Constant * storeString = ConstantDataArray::getString(
        M.getContext(),
        "I am storing %ld at address %p\n"
      );


      return false;
    }
  };
}

// Class memeber init
char MemoryProfiler::pid = 0;

// Pass registration
static void registerMemoryProfilerPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
  PM.add(new MemoryProfiler());
}

// ! Not suitable to run with optimization (-O) compiler option(s) ! (@see EP_EnabledOnOptLevel0)
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EnabledOnOptLevel0, registerMemoryProfilerPass);
