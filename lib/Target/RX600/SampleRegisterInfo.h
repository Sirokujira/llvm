//===-- RX600RegisterInfo.h - RX600 Register Information Impl -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the RX600 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLEREGISTERINFO_H
#define SAMPLEREGISTERINFO_H

#include "RX600.h"
#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "RX600GenRegisterInfo.inc"

namespace llvm {
class RX600Subtarget;
class TargetInstrInfo;
class Type;

struct RX600RegisterInfo : public RX600GenRegisterInfo {
  const TargetInstrInfo &TII;

  RX600RegisterInfo(const TargetInstrInfo &tii);

  /// Code Generation virtual methods...
  const uint16_t *getCalleeSavedRegs(const MachineFunction* MF = 0) const /*override*/;
  const uint32_t *getCallPreservedMask(CallingConv::ID) const /*override*/;

  BitVector getReservedRegs(const MachineFunction &MF) const /*override*/;

  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const /*override*/;

  /// Stack Frame Processing Methods
  void eliminateFrameIndex(MachineBasicBlock::iterator II,
                           int SPAdj, RegScavenger *RS = NULL) const;

  /// Debug information queries.
  unsigned getFrameRegister(const MachineFunction &MF) const /*override*/;
};

} // end namespace llvm


#endif

