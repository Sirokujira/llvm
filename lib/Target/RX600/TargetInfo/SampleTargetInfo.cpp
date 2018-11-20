//===-- RX600TargetInfo.cpp - RX600 Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "RX600.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheRX600Target;

extern "C" void LLVMInitializeRX600TargetInfo() { 
  DEBUG(dbgs() << ">> InitRX600TargetInfo <<\n");
  RegisterTarget<Triple::sample, /*HasJIT=*/false>
    X(TheRX600Target, "sample", "RX600");
}
