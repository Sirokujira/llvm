//===-- RX600Subtarget.cpp - RX600 Subtarget Information ----------------===//
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
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "RX600GenSubtargetInfo.inc"

using namespace llvm;

RX600Subtarget::RX600Subtarget(const std::string &TT,
                                 const std::string &CPU,
                                 const std::string &FS)
    : RX600GenSubtargetInfo(TT, CPU, FS) {
  std::string CPUName = "generic";

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);
}
