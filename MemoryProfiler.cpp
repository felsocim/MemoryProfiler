#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/GlobalVariable.h"

using namespace llvm;

namespace {
  // Test if n is a prime number
  bool isPrime(int64_t n) {
    if(n < 2)
      return false;

    for(int64_t i = 2; i <= (n/2); i++) {
      if(n % i == 0)
        return false;
    }

    return true;
  }

  // Pass itself
  struct PrimeNumberCounter : public ModulePass {
    static char pid;
    PrimeNumberCounter() : ModulePass(pid) {}
    
    virtual bool runOnModule(Module &M) {
      // Iterate over global constants
      Module::global_iterator it = M.global_begin();
      
      for(;it != M.global_end(); it++) {
        GlobalVariable * global = dyn_cast<GlobalVariable>(it);
        for(unsigned int k = 0; k < global->getNumOperands(); k++) {
          Value * operand = global->getOperand(k);
          if(operand->getType()->isIntegerTy()) {
            ConstantInt * constant = dyn_cast<ConstantInt>(operand);
            if(constant != nullptr) {
              int64_t global_value = constant->getSExtValue();
              if(isPrime(global_value))
                outs() << global_value << "\n";
            }
          }
        }
      } 

      // Iterate over local constants
      for(Function& F : M.functions()) {
        for(BasicBlock& B : F) {
          for(Instruction& I : B) {
            for(unsigned int i = 0; i < I.getNumOperands(); i++) {
              Value * operand = I.getOperand(i);
              if(operand->getType()->isIntegerTy()) {
                ConstantInt * constant = dyn_cast<ConstantInt>(operand);
                if(constant != nullptr) {
                  int64_t local_value = constant->getSExtValue();
                  if(isPrime(local_value))
                    outs() << local_value << "\n";
                }
              }
            }
          }
        }
      }
      return false;
    }
  };
}

// Class memeber init
char PrimeNumberCounter::pid = 0;

// Pass registration
static void registerPrimeNumberCounter(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
  PM.add(new PrimeNumberCounter());
}

// ! Not suitable to run with -O compiler options ! (see EP_EnabledOnOptLevel0)
static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EnabledOnOptLevel0, registerPrimeNumberCounter);
