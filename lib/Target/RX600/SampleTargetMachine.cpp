//===-- RX600TargetMachine.cpp - Define TargetMachine for RX600 -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about RX600 target spec.
//
//===----------------------------------------------------------------------===//

#include "RX600TargetMachine.h"
#include "RX600.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeRX600Target() {
  // Register the target.
  RegisterTargetMachine<RX600TargetMachine> X(TheRX600Target);
}

RX600TargetMachine::
RX600TargetMachine(const Target &T, StringRef Triple,
                    StringRef CPU, StringRef FS, const TargetOptions &Options,
                    Reloc::Model RM, CodeModel::Model CM,
                    CodeGenOpt::Level OL)
    : LLVMTargetMachine(T, Triple, CPU, FS, Options, RM, CM, OL),
      DL("e-p:32:32:32-i8:8:32-i16:16:32-i64:64:64-n32"),
      Subtarget(Triple, CPU, FS),
      InstrInfo(*this),
      FrameLowering(Subtarget),
      TLInfo(*this), TSInfo(*this) {}

namespace {
/// RX600 Code Generator Pass Configuration Options.
class RX600PassConfig : public TargetPassConfig {
 public:
  RX600PassConfig(RX600TargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  RX600TargetMachine &getRX600TargetMachine() const {
    return getTM<RX600TargetMachine>();
  }

  virtual bool addInstSelector();
};
} // namespace

TargetPassConfig *RX600TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new RX600PassConfig(this, PM);
}

bool RX600PassConfig::addInstSelector() {
  // Install an instruction selector.
  addPass(createRX600ISelDag(getRX600TargetMachine()));
  return false;
}
