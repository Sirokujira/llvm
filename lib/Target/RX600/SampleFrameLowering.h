//===-- RX600FrameLowering.h - Define frame lowering for RX600 ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_FRAMELOWERING_H
#define SAMPLE_FRAMELOWERING_H

#include "RX600.h"
#include "RX600Subtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class RX600Subtarget;

class RX600FrameLowering : public TargetFrameLowering {
 protected:
  const RX600Subtarget &STI;

 public:
  explicit RX600FrameLowering(const RX600Subtarget &sti)
      : TargetFrameLowering(StackGrowsDown, 8, 0), STI(sti) {
  }

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF) const /*override*/;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const /*override*/;
  bool hasFP(const MachineFunction &MF) const /*override*/;
};
} // End llvm namespace

#endif
