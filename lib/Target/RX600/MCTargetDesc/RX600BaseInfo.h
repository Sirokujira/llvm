//===-- RX600BaseInfo.h - Top level definitions for MIPS MC ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the RX600 target useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef SAMPLEBASEINFO_H
#define SAMPLEASEINFO_H

//#include "RX600FixupKinds.h"
#include "RX600MCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

/// getRX600RegisterNumbering - Given the enum value for some register,
/// return the number that it corresponds to.
inline static unsigned getRX600RegisterNumbering(unsigned RegEnum)
{
  switch (RegEnum) {
  case RX600::ZERO:
    return 0;
  case RX600::V0:
    return 1;
  case RX600::A0:
    return 2;
  case RX600::A1:
    return 3;
  case RX600::A2:
    return 4;
  case RX600::A3:
    return 5;
  case RX600::T0:
    return 6;
  case RX600::T1:
    return 7;
  case RX600::T2:
    return 8;
  case RX600::T3:
    return 9;
  case RX600::S0:
    return 10;
  case RX600::S1:
    return 11;
  case RX600::S2:
    return 12;
  case RX600::S3:
    return 13;
  case RX600::SP:
    return 14;
  case RX600::RA:
    return 15;
  default: llvm_unreachable("Unknown register number!");
  }
}
}

#endif
