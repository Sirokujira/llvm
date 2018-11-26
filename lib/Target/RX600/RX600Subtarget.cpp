//===-- RX600Subtarget.cpp - RX600 Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the RX600 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "RX600Subtarget.h"
#include "RX600.h"
#include "RX600FrameLowering.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "rx600-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "RX600GenSubtargetInfo.inc"

void RX600Subtarget::anchor() {}

RX600Subtarget &RX600Subtarget::initializeSubtargetDependencies(StringRef CPU,
                                                                StringRef FS,
                                                                bool Is64Bit) {
  // Determine default and user-specified characteristics
  std::string CPUName = CPU;
  if (CPUName.empty())
    CPUName = "rx600";
  ParseSubtargetFeatures(CPUName, FS);
  if (Is64Bit) {
    XLenVT = MVT::i64;
    XLen = 64;
  }
  return *this;
}

RX600Subtarget::RX600Subtarget(const Triple &TT, const std::string &CPU,
                               const std::string &FS, const TargetMachine &TM)
    : RX600GenSubtargetInfo(TT, CPU, FS),
      FrameLowering(initializeSubtargetDependencies(CPU, FS, TT.isArch64Bit())),
      InstrInfo(), RegInfo(getHwMode()), TLInfo(TM, *this) {}
