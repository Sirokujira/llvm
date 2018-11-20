//===-- RX600TargetInfo.cpp - RX600 Target Implementation -----------------===//
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
Target &getTheRX60032Target() {
  static Target TheRX60032Target;
  return TheRX60032Target;
}

Target &getTheRX60064Target() {
  static Target TheRX60064Target;
  return TheRX60064Target;
}
}

extern "C" void LLVMInitializeRX600TargetInfo() {
  RegisterTarget<Triple::riscv32> X(getTheRX60032Target(), "riscv32",
                                    "32-bit RISC-V", "RX600");
  RegisterTarget<Triple::riscv64> Y(getTheRX60064Target(), "riscv64",
                                    "64-bit RISC-V", "RX600");
}
