//===-- RX600MCTargetDesc.cpp - RX600 Target Descriptions -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides RX600 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "RX600MCAsmInfo.h"
#include "RX600MCTargetDesc.h"
#include "InstPrinter/RX600InstPrinter.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "RX600GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "RX600GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "RX600GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createRX600MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitRX600MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createRX600MCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitRX600MCRegisterInfo(X, RX600::RA);
  return X;
}

static MCSubtargetInfo *createRX600MCSubtargetInfo(StringRef TT, StringRef CPU,
                                                  StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitRX600MCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

static MCAsmInfo *createRX600MCAsmInfo(const Target &T, StringRef TT) {
  MCAsmInfo *MAI = new RX600MCAsmInfo(T, TT);

  MachineLocation Dst(MachineLocation::VirtualFP);
  MachineLocation Src(RX600::SP, 0);
  MAI->addInitialFrameState(0, Dst, Src);

  return MAI;
}

static MCCodeGenInfo *createRX600MCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                              CodeModel::Model CM,
                                              CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCInstPrinter *createRX600MCInstPrinter(const Target &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI,
                                              const MCSubtargetInfo &STI) {
  return new RX600InstPrinter(MAI, MII, MRI);
}

static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
                                    MCContext &Ctx, MCAsmBackend &MAB,
                                    raw_ostream &_OS,
                                    MCCodeEmitter *_Emitter,
                                    bool RelaxAll,
                                    bool NoExecStack) {
  Triple TheTriple(TT);

  return createELFStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll, NoExecStack);
}

extern "C" void LLVMInitializeRX600TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(TheRX600Target, createRX600MCAsmInfo);
  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheRX600Target,
                                        createRX600MCCodeGenInfo);
  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheRX600Target, createRX600MCInstrInfo);
  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheRX600Target, createRX600MCRegisterInfo);
  // Register the MC Code Emitter
  TargetRegistry::RegisterMCCodeEmitter(TheRX600Target,
                                        createRX600MCCodeEmitter);
  // Register the object streamer.
  TargetRegistry::RegisterMCObjectStreamer(TheRX600Target, createMCStreamer);
  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(TheRX600Target,
                                       createRX600AsmBackend);
  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheRX600Target,
                                          createRX600MCSubtargetInfo);
  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheRX600Target,
                                        createRX600MCInstPrinter);
}

