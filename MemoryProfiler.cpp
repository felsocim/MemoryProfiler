#include <vector>

// LLVM includes
#include "llvm/IR/Constants.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace {
  // Pass itself
  struct MemoryProfiler : public ModulePass {
    static char pid;
    MemoryProfiler() : ModulePass(pid) {}

    // Main pass function
    virtual bool runOnModule(Module &M) {
      IRBuilder<> * builder = new IRBuilder<>(M.getContext());

      // Creating 'printf' function instance
      Function * print = Function::Create(
        TypeBuilder<int(char *, ...),false>::get(M.getContext()), // Function type construction
        Function::ExternalLinkage,
        "printf",
        &M
      );

      // Creating messages constants
      Constant * loadString = ConstantDataArray::getString(
        M.getContext(),
        "I am loading address %p\n"
      );

      Constant * storeString = ConstantDataArray::getString(
        M.getContext(),
        "I am storing %ld at address %p\n"
      );

      // Defining and initializing global variables corresponding to message strings
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

      // Initializing a zero constant
      Constant * nullValue = Constant::getNullValue(IntegerType::getInt32Ty(M.getContext()));

      // Creating array of indices which we'll use to point to the beginning of a string array storing our messages
      std::vector<Constant *> beginAt;
      beginAt.push_back(nullValue);
      beginAt.push_back(nullValue);

      // Retrieving 'getelementptr' form of our string constants
      Constant * loadStringPointer = ConstantExpr::getGetElementPtr(loadString->getType(), loadStringVariable, beginAt);
      Constant * storeStringPointer = ConstantExpr::getGetElementPtr(storeString->getType(), storeStringVariable, beginAt);

      int loadCounter = 0, storeCounter = 0;

      for(Function &F : M) {
        for(BasicBlock &B : F) {
          for(Instruction &I : B) {
            LoadInst * load = nullptr;
            StoreInst * store = nullptr;

            // Setting insertion point for new instructions
            builder->SetInsertPoint(&I);

            if((load = dyn_cast<LoadInst>(&I)) != nullptr) { // Check if the current instruction is a load
              loadCounter++;

              Value * address = load->getPointerOperand(); // Getting source address

              // Constructing argument array for 'printf' function call
              std::vector<Value *> args;
              args.push_back(loadStringPointer);
              args.push_back(address);

              // Adding 'printf' function call using the IR builder instance
              CallInst * call = builder->CreateCall(
                print,
                args,
                ""
              );
            } else if ((store = dyn_cast<StoreInst>(&I)) != nullptr) { // Check if the current instruction is a store
              storeCounter++;

              Value * value = store->getValueOperand(); // Getting the value to be stored at given address
              Value * address = store->getPointerOperand(); // Getting source address

              // Constructing argument array for 'printf' function call
              std::vector<Value *> args;
              args.push_back(storeStringPointer);
              args.push_back(value);
              args.push_back(address);

              // Adding 'printf' function call using the IR builder instance
              CallInst * call = builder->CreateCall(
                print,
                args,
                ""
              );
            }
          }
        }
      }

      errs() << "Successfully detected " << loadCounter << " load call(s) and " << storeCounter << " store call(s) [total " << (loadCounter + storeCounter) << "]\n";

      return true;
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
