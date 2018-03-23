#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include <vector>

using namespace llvm;

namespace {
  // Pass itself
  struct MemoryProfiler : public ModulePass {
    static char pid;
    MemoryProfiler() : ModulePass(pid) {}

    // Main pass function
    virtual bool runOnModule(Module &M) {
      IRBuilder<> * builder = new IRBuilder<>(M.getContext());

      Function * print = Function::Create(
        TypeBuilder<int(char *, ...),false>::get(M.getContext()),
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

      llvm::Constant * zero = llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(M.getContext()));

      std::vector<llvm::Constant *> indices;
      indices.push_back(zero);
      indices.push_back(zero);

      GlobalVariable * loadStringVariable = new GlobalVariable(
        M,
        loadString->getType(),
        true,
        Function::InternalLinkage,
        loadString,
        "load_string"
      );

      GlobalVariable * storeStringVariable = new GlobalVariable(
        M,
        storeString->getType(),
        true,
        Function::InternalLinkage,
        storeString,
        "store_string"
      );

      Constant * loadStringPointer = ConstantExpr::getGetElementPtr(loadString->getType(), loadStringVariable, indices);
      Constant * storeStringPointer = ConstantExpr::getGetElementPtr(storeString->getType(), storeStringVariable, indices);

      for(Function &F : M) {
        for(BasicBlock &B : F) {
          for(Instruction &I : B) {
            LoadInst * load = nullptr;
            StoreInst * store = nullptr;
            builder->SetInsertPoint(&I);

            if((load = dyn_cast<LoadInst>(&I)) != nullptr) {
              outs() << "Load instruction\n";
              Value * address = load->getPointerOperand();
              std::vector<Value *> args;
              args.push_back(loadStringPointer);
              args.push_back(address);

              CallInst * call = builder->CreateCall(
                print,
                args,
                ""
              );

              errs()<<"Insert call"<<*call<<"\n";
            } else if ((store = dyn_cast<StoreInst>(&I)) != nullptr) {
              outs() << "Store instruction\n";
              Value * value = store->getValueOperand();
              Value * address = store->getPointerOperand();

              std::vector<Value *> args;
              args.push_back(storeStringPointer);
              args.push_back(value);
              args.push_back(address);

              CallInst * call = builder->CreateCall(
                print,
                args,
                ""
              );

              errs()<<"Insert call"<<*call<<"\n";
            }
          }
        }
      }

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
