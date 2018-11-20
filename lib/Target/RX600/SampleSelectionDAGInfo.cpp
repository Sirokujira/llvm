//===-- RX600SelectionDAGInfo.cpp - RX600 SelectionDAG Info -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the RX600SelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "sample-selectiondag-info"
#include "RX600TargetMachine.h"
using namespace llvm;

RX600SelectionDAGInfo::RX600SelectionDAGInfo(const RX600TargetMachine &TM)
    : TargetSelectionDAGInfo(TM) {}

RX600SelectionDAGInfo::~RX600SelectionDAGInfo() {}
