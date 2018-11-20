//===-- RX600InstrInfo.cpp - RX600 Instruction Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the RX600 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "RX600InstrInfo.h"
#include "RX600TargetMachine.h"
#include "RX600MachineFunction.h"
#include "MCTargetDesc/RX600MCTargetDesc.h"
#include "InstPrinter/RX600InstPrinter.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/STLExtras.h"

#define GET_INSTRINFO_CTOR
#include "RX600GenInstrInfo.inc"

using namespace llvm;

RX600InstrInfo::RX600InstrInfo(RX600TargetMachine &tm)
  : RX600GenInstrInfo(RX600::ADJCALLSTACKDOWN, RX600::ADJCALLSTACKUP),
    TM(tm),
    RI(*this){}

const RX600RegisterInfo &RX600InstrInfo::getRegisterInfo() const {
  return RI;
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned RX600InstrInfo::
isLoadFromStackSlot(const MachineInstr *MI, int &FrameIndex) const {
  unsigned Opc = MI->getOpcode();

  if (Opc == RX600::LOAD       && // Load命令
      MI->getOperand(1).isFI()  && // スタックスロット
      MI->getOperand(2).isImm() && // 即値が0
      MI->getOperand(2).getImm() == 0) {
    FrameIndex = MI->getOperand(1).getIndex();
    return MI->getOperand(0).getReg();
  }
  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned RX600InstrInfo::
isStoreToStackSlot(const MachineInstr *MI, int &FrameIndex) const {
  unsigned Opc = MI->getOpcode();

  if (Opc == RX600::STORE      && // Store命令
      MI->getOperand(1).isFI()  && // スタックスロット
      MI->getOperand(2).isImm() && // 即値が0
      MI->getOperand(2).getImm() == 0) {
    FrameIndex = MI->getOperand(1).getIndex();
    return MI->getOperand(0).getReg();
  }
  return 0;
}

void RX600InstrInfo::
copyPhysReg(MachineBasicBlock &MBB,
            MachineBasicBlock::iterator I, DebugLoc DL,
            unsigned DestReg, unsigned SrcReg,
            bool KillSrc) const {
  unsigned Opc = 0, ZeroReg = 0;
  Opc = RX600::ADD, ZeroReg = RX600::ZERO;

  MachineInstrBuilder MIB = BuildMI(MBB, I, DL, get(Opc));

  if (DestReg)
    MIB.addReg(DestReg, RegState::Define);

  if (ZeroReg)
    MIB.addReg(ZeroReg);

  if (SrcReg)
    MIB.addReg(SrcReg, getKillRegState(KillSrc));
}

void RX600InstrInfo::
storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    unsigned SrcReg, bool isKill, int FI,
                    const TargetRegisterClass *RC,
                    const TargetRegisterInfo *TRI) const {
  DEBUG(dbgs() << ">> RX600InstrInfo::storeRegToStackSlot <<\n");

  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = *MF.getFrameInfo();

  MachineMemOperand *MMO =
    MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(FI),
                            MachineMemOperand::MOStore,
                            MFI.getObjectSize(FI),
                            MFI.getObjectAlignment(FI));

  BuildMI(MBB, I, DL, get(RX600::STORE))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FI).addImm(0).addMemOperand(MMO);
}

void RX600InstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB,
                     MachineBasicBlock::iterator MI,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC,
                     const TargetRegisterInfo *TRI) const
{
  DEBUG(dbgs() << ">> RX600InstrInfo::loadRegFromStackSlot <<\n");

  DebugLoc DL;
  if (MI != MBB.end()) DL = MI->getDebugLoc();
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = *MF.getFrameInfo();

  MachineMemOperand *MMO =
    MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(FI),
                            MachineMemOperand::MOLoad,
                            MFI.getObjectSize(FI),
                            MFI.getObjectAlignment(FI));

  BuildMI(MBB, MI, DL, get(RX600::LOAD))
      .addReg(DestReg).addFrameIndex(FI).addImm(0).addMemOperand(MMO);
}

//===----------------------------------------------------------------------===//
// Branch Analysis
//===----------------------------------------------------------------------===//

bool RX600InstrInfo::
AnalyzeBranch(MachineBasicBlock &MBB,
              MachineBasicBlock *&TBB,
              MachineBasicBlock *&FBB,
              SmallVectorImpl<MachineOperand> &Cond,
              bool AllowModify) const
{
  // 未実装の場合はtrueを返す
  return true;
}

unsigned RX600InstrInfo::
InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
             MachineBasicBlock *FBB,
             const SmallVectorImpl<MachineOperand> &Cond,
             DebugLoc DL) const {
  llvm_unreachable("Target doesn't implement RX600InstrInfo::InsertBranch!");
}

unsigned RX600InstrInfo::
RemoveBranch(MachineBasicBlock &MBB) const
{
  llvm_unreachable("Target doesn't implement RX600InstrInfo::RemoveBranch");
}
