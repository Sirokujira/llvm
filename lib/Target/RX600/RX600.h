//===-- RX600.h - Top-level interface for RX600 -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// RISC-V back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RX600_RX600_H
#define LLVM_LIB_TARGET_RX600_RX600_H

#include "MCTargetDesc/RX600BaseInfo.h"

namespace llvm {
class RX600TargetMachine;
class AsmPrinter;
class FunctionPass;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineOperand;
class PassRegistry;

void LowerRX600MachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                    const AsmPrinter &AP);
bool LowerRX600MachineOperandToMCOperand(const MachineOperand &MO,
                                         MCOperand &MCOp, const AsmPrinter &AP);

FunctionPass *createRX600ISelDag(RX600TargetMachine &TM);

FunctionPass *createRX600MergeBaseOffsetOptPass();
void initializeRX600MergeBaseOffsetOptPass(PassRegistry &);
}

#endif
