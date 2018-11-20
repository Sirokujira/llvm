//===-- RX600FrameLowering.cpp - RX600 Frame Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the RX600 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "RX600FrameLowering.h"
#include "RX600InstrInfo.h"
#include "RX600MachineFunction.h"
#include "MCTargetDesc/RX600MCTargetDesc.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/DataLayout.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

bool RX600FrameLowering::
hasFP(const MachineFunction &MF) const {
  return false;
}

void RX600FrameLowering::
emitPrologue(MachineFunction &MF) const {
  DEBUG(dbgs() << ">> RX600FrameLowering::emitPrologue <<\n");

  MachineBasicBlock &MBB   = MF.front();
  MachineFrameInfo *MFI = MF.getFrameInfo();

  const RX600InstrInfo &TII =
    *static_cast<const RX600InstrInfo*>(MF.getTarget().getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // allocate fixed size for simplicity
  uint64_t StackSize = 4 * 16;

   // Update stack size
  MFI->setStackSize(StackSize);

  BuildMI(MBB, MBBI, dl, TII.get(RX600::MOVE), RX600::T0)
      .addImm(-StackSize);
  BuildMI(MBB, MBBI, dl, TII.get(RX600::ADD), RX600::SP)
      .addReg(RX600::SP)
      .addReg(RX600::T0);
}

void RX600FrameLowering::
emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
  DEBUG(dbgs() << ">> RX600FrameLowering::emitEpilogue <<\n");

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo *MFI            = MF.getFrameInfo();
  const RX600InstrInfo &TII =
    *static_cast<const RX600InstrInfo*>(MF.getTarget().getInstrInfo());
  DebugLoc dl = MBBI->getDebugLoc();

  // Get the number of bytes from FrameInfo
  uint64_t StackSize = MFI->getStackSize();

  // Adjust stack.
  BuildMI(MBB, MBBI, dl, TII.get(RX600::MOVE), RX600::T0)
      .addImm(StackSize);
  BuildMI(MBB, MBBI, dl, TII.get(RX600::ADD), RX600::SP)
      .addReg(RX600::SP)
      .addReg(RX600::T0);
}
