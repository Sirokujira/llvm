//===-- RX600TargetMachine.cpp - Define TargetMachine for RX600 -----------===//
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

#include "RX600.h"
#include "RX600TargetMachine.h"
#include "RX600TargetObjectFile.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
using namespace llvm;

extern "C" void LLVMInitializeRX600Target() {
  RegisterTargetMachine<RX600TargetMachine> X(getTheRX600Target());
}

static std::string computeDataLayout(const Triple &TT) {
  if (TT.isArch64Bit()) {
    return "e-m:e-p:64:64-i64:64-i128:128-n64-S128";
  } else {
    assert(TT.isArch32Bit() && "only RX600 are currently supported");
    return "e-m:e-p:32:32-i64:64-n32-S128";
  }
}

static Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                           Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::Static;
  return *RM;
}

static CodeModel::Model getEffectiveCodeModel(Optional<CodeModel::Model> CM) {
  if (CM)
    return *CM;
  return CodeModel::Small;
}

RX600TargetMachine::RX600TargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       Optional<Reloc::Model> RM,
                                       Optional<CodeModel::Model> CM,
                                       CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(TT, RM),
                        getEffectiveCodeModel(CM), OL),
      TLOF(make_unique<RX600ELFTargetObjectFile>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

namespace {
class RX600PassConfig : public TargetPassConfig {
public:
  RX600PassConfig(RX600TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  RX600TargetMachine &getRX600TargetMachine() const {
    return getTM<RX600TargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
  void addPreRegAlloc() override;
};
}

TargetPassConfig *RX600TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new RX600PassConfig(*this, PM);
}

void RX600PassConfig::addIRPasses() {
  addPass(createAtomicExpandPass());
  TargetPassConfig::addIRPasses();
}

bool RX600PassConfig::addInstSelector() {
  addPass(createRX600ISelDag(getRX600TargetMachine()));

  return false;
}

void RX600PassConfig::addPreEmitPass() { addPass(&BranchRelaxationPassID); }

void RX600PassConfig::addPreRegAlloc() {
  addPass(createRX600MergeBaseOffsetOptPass());
}
