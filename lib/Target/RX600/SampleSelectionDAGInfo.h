//===-- RX600SelectionDAGInfo.h - RX600 SelectionDAG Info ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the RX600 subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_SELECTIONDAGINFO_H
#define SAMPLE_SELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class RX600TargetMachine;

class RX600SelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit RX600SelectionDAGInfo(const RX600TargetMachine &TM);
  ~RX600SelectionDAGInfo();
};
} // end of namespace llvm

#endif
