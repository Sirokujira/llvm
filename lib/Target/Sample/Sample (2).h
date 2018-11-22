//===- Sample.h - Top-level interface for Sample representation -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Sample back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Sample_Sample_H
#define LLVM_LIB_TARGET_Sample_Sample_H

#include "MCTargetDesc/SampleMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class FunctionPass;
class SampleTargetMachine;

FunctionPass *createSampleISelDag(SampleTargetMachine &TM,
                               CodeGenOpt::Level OptLevel);
FunctionPass *createSampleExpandPseudosPass();
FunctionPass *createSampleBranchFinalizePass();

} // end namespace llvm

#endif // LLVM_LIB_TARGET_Sample_Sample_H
