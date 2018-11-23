//===-- RX600MCTargetDesc.cpp - RX600 Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// This file provides RX600-specific target descriptions.
///
//===----------------------------------------------------------------------===//

#include "RX600MCTargetDesc.h"
#include "InstPrinter/RX600InstPrinter.h"
#include "RX600ELFStreamer.h"
#include "RX600MCAsmInfo.h"
#include "RX600TargetStreamer.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "RX600GenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "RX600GenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "RX600GenSubtargetInfo.inc"

using namespace llvm;

static MCInstrInfo *createRX600MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitRX600MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createRX600MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitRX600MCRegisterInfo(X, RX600::X1);
  return X;
}

static MCAsmInfo *createRX600MCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT) {
  return new RX600MCAsmInfo(TT);
}

static MCSubtargetInfo *createRX600MCSubtargetInfo(const Triple &TT,
                                                   StringRef CPU, StringRef FS) {
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = TT.isArch64Bit() ? "rx600";
  return createRX600MCSubtargetInfoImpl(TT, CPUName, FS);
}

static MCInstPrinter *createRX600MCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new RX600InstPrinter(MAI, MII, MRI);
}

static MCTargetStreamer *
createRX600ObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  const Triple &TT = STI.getTargetTriple();
  if (TT.isOSBinFormatELF())
    return new RX600TargetELFStreamer(S, STI);
  return nullptr;
}

static MCTargetStreamer *createRX600AsmTargetStreamer(MCStreamer &S,
                                                      formatted_raw_ostream &OS,
                                                      MCInstPrinter *InstPrint,
                                                      bool isVerboseAsm) {
  return new RX600TargetAsmStreamer(S, OS);
}

extern "C" void LLVMInitializeRX600TargetMC() {
  for (Target *T : {&getTheRX600Target()}) {
    TargetRegistry::RegisterMCAsmInfo(*T, createRX600MCAsmInfo);
    TargetRegistry::RegisterMCInstrInfo(*T, createRX600MCInstrInfo);
    TargetRegistry::RegisterMCRegInfo(*T, createRX600MCRegisterInfo);
    TargetRegistry::RegisterMCAsmBackend(*T, createRX600AsmBackend);
    TargetRegistry::RegisterMCCodeEmitter(*T, createRX600MCCodeEmitter);
    TargetRegistry::RegisterMCInstPrinter(*T, createRX600MCInstPrinter);
    TargetRegistry::RegisterMCSubtargetInfo(*T, createRX600MCSubtargetInfo);
    TargetRegistry::RegisterObjectTargetStreamer(
        *T, createRX600ObjectTargetStreamer);

    // Register the asm target streamer.
    TargetRegistry::RegisterAsmTargetStreamer(*T, createRX600AsmTargetStreamer);
  }
}
