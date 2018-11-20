//===-- RX600MachineFunctionInfo.cpp - Private data used for RX600 ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "RX600MachineFunction.h"
#include "RX600InstrInfo.h"
#include "MCTargetDesc/RX600MCTargetDesc.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

void RX600MachineFunctionInfo::anchor() { }
