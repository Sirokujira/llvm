//===-- RX600RegisterInfo.cpp - RX600 Register Information ------*- C++ -*-===//
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

#include "RX600RegisterInfo.h"
#include "RX600.h"
#include "RX600Subtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "RX600GenRegisterInfo.inc"

using namespace llvm;

RX600RegisterInfo::RX600RegisterInfo(unsigned HwMode)
    : RX600GenRegisterInfo(RX600::X1, /*DwarfFlavour*/0, /*EHFlavor*/0,
                           /*PC*/0, HwMode) {}

const MCPhysReg *
RX600RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  if (MF->getFunction().hasFnAttribute("interrupt")) {
    if (MF->getSubtarget<RX600Subtarget>().hasStdExtD())
      return CSR_XLEN_F64_Interrupt_SaveList;
    if (MF->getSubtarget<RX600Subtarget>().hasStdExtF())
      return CSR_XLEN_F32_Interrupt_SaveList;
    return CSR_Interrupt_SaveList;
  }
  return CSR_SaveList;
}

BitVector RX600RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  // Use markSuperRegs to ensure any register aliases are also reserved
  markSuperRegs(Reserved, RX600::X0); // zero
  markSuperRegs(Reserved, RX600::X1); // ra
  markSuperRegs(Reserved, RX600::X2); // sp
  markSuperRegs(Reserved, RX600::X3); // gp
  markSuperRegs(Reserved, RX600::X4); // tp
  markSuperRegs(Reserved, RX600::X8); // fp
  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

bool RX600RegisterInfo::isConstantPhysReg(unsigned PhysReg) const {
  return PhysReg == RX600::X0;
}

const uint32_t *RX600RegisterInfo::getNoPreservedMask() const {
  return CSR_NoRegs_RegMask;
}

void RX600RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineRegisterInfo &MRI = MF.getRegInfo();
  const RX600InstrInfo *TII = MF.getSubtarget<RX600Subtarget>().getInstrInfo();
  DebugLoc DL = MI.getDebugLoc();

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  unsigned FrameReg;
  int Offset =
      getFrameLowering(MF)->getFrameIndexReference(MF, FrameIndex, FrameReg) +
      MI.getOperand(FIOperandNum + 1).getImm();

  if (!isInt<32>(Offset)) {
    report_fatal_error(
        "Frame offsets outside of the signed 32-bit range not supported");
  }

  MachineBasicBlock &MBB = *MI.getParent();
  bool FrameRegIsKill = false;

  if (!isInt<12>(Offset)) {
    assert(isInt<32>(Offset) && "Int32 expected");
    // The offset won't fit in an immediate, so use a scratch register instead
    // Modify Offset and FrameReg appropriately
    unsigned ScratchReg = MRI.createVirtualRegister(&RX600::GPRRegClass);
    TII->movImm32(MBB, II, DL, ScratchReg, Offset);
    BuildMI(MBB, II, DL, TII->get(RX600::ADD), ScratchReg)
        .addReg(FrameReg)
        .addReg(ScratchReg, RegState::Kill);
    Offset = 0;
    FrameReg = ScratchReg;
    FrameRegIsKill = true;
  }

  MI.getOperand(FIOperandNum)
      .ChangeToRegister(FrameReg, false, false, FrameRegIsKill);
  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
}

unsigned RX600RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = getFrameLowering(MF);
  return TFI->hasFP(MF) ? RX600::X8 : RX600::X2;
}

const uint32_t *
RX600RegisterInfo::getCallPreservedMask(const MachineFunction & MF,
                                        CallingConv::ID /*CC*/) const {
  if (MF.getFunction().hasFnAttribute("interrupt")) {
    if (MF.getSubtarget<RX600Subtarget>().hasStdExtD())
      return CSR_XLEN_F64_Interrupt_RegMask;
    if (MF.getSubtarget<RX600Subtarget>().hasStdExtF())
      return CSR_XLEN_F32_Interrupt_RegMask;
    return CSR_Interrupt_RegMask;
  }
  return CSR_RegMask;
}
