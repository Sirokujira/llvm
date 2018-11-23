//===-- RX600TargetInfo.cpp - RX600 Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

namespace llvm {
Target &getTheRX600Target() {
  static Target TheRX600Target;
  return TheRX600Target;
}

extern "C" void LLVMInitializeRISCVTargetInfo() {
  RegisterTarget<Triple::RX600> X(getTheRX600Target(), "RX600",
                                    "32-bit Renesas CPU", "RX600");
}
