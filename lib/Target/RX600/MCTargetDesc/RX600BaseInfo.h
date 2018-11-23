//===-- RX600BaseInfo.h - Top level definitions for RX600 MC ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone enum definitions for the RX600 target
// useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_RX600_MCTARGETDESC_RX600BASEINFO_H
#define LLVM_LIB_TARGET_RX600_MCTARGETDESC_RX600BASEINFO_H

#include "RX600MCTargetDesc.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"

namespace llvm {

// RX600II - This namespace holds all of the target specific flags that
// instruction info tracks. All definitions must match RX600InstrFormats.td.
namespace RX600II {
enum {
  InstFormatPseudo = 0,
  InstFormatR = 1,
  InstFormatR4 = 2,
  InstFormatI = 3,
  InstFormatS = 4,
  InstFormatB = 5,
  InstFormatU = 6,
  InstFormatJ = 7,
  InstFormatCR = 8,
  InstFormatCI = 9,
  InstFormatCSS = 10,
  InstFormatCIW = 11,
  InstFormatCL = 12,
  InstFormatCS = 13,
  InstFormatCB = 14,
  InstFormatCJ = 15,
  InstFormatOther = 16,

  InstFormatMask = 31
};

enum {
  MO_None,
  MO_LO,
  MO_HI,
  MO_PCREL_HI,
};
} // namespace RX600II

// Describes the predecessor/successor bits used in the FENCE instruction.
namespace RX600FenceField {
enum FenceField {
  I = 8,
  O = 4,
  R = 2,
  W = 1
};
}

// Describes the supported floating point rounding mode encodings.
namespace RX600FPRndMode {
enum RoundingMode {
  RNE = 0,
  RTZ = 1,
  RDN = 2,
  RUP = 3,
  RMM = 4,
  DYN = 7,
  Invalid
};

inline static StringRef roundingModeToString(RoundingMode RndMode) {
  switch (RndMode) {
  default:
    llvm_unreachable("Unknown floating point rounding mode");
  case RX600FPRndMode::RNE:
    return "rne";
  case RX600FPRndMode::RTZ:
    return "rtz";
  case RX600FPRndMode::RDN:
    return "rdn";
  case RX600FPRndMode::RUP:
    return "rup";
  case RX600FPRndMode::RMM:
    return "rmm";
  case RX600FPRndMode::DYN:
    return "dyn";
  }
}

inline static RoundingMode stringToRoundingMode(StringRef Str) {
  return StringSwitch<RoundingMode>(Str)
      .Case("rne", RX600FPRndMode::RNE)
      .Case("rtz", RX600FPRndMode::RTZ)
      .Case("rdn", RX600FPRndMode::RDN)
      .Case("rup", RX600FPRndMode::RUP)
      .Case("rmm", RX600FPRndMode::RMM)
      .Case("dyn", RX600FPRndMode::DYN)
      .Default(RX600FPRndMode::Invalid);
}
} // namespace RX600FPRndMode
} // namespace llvm

#endif
