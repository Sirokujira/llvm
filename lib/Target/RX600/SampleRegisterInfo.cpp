//===-- RX600RegisterInfo.cpp - RX600 Register Information -== --------------===//
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

#define DEBUG_TYPE "sample-reg-info"

#include "RX600RegisterInfo.h"
#include "RX600.h"
#include "llvm/Constants.h"
#include "llvm/Type.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/DebugInfo.h"

#include "MCTargetDesc/RX600MCTargetDesc.h"

#define GET_REGINFO_TARGET_DESC
#include "RX600GenRegisterInfo.inc"

using namespace llvm;

RX600RegisterInfo::
RX600RegisterInfo(const TargetInstrInfo &tii)
  : RX600GenRegisterInfo(RX600::RA), TII(tii) { }

//===----------------------------------------------------------------------===//
// Callee Saved Registers methods
//===----------------------------------------------------------------------===//

// 呼び出し先待避レジスタ
const uint16_t* RX600RegisterInfo::
getCalleeSavedRegs(const MachineFunction *MF) const {
    return CSR_SingleFloatOnly_SaveList;
}

// 呼び出し元待避レジスタ
const uint32_t* RX600RegisterInfo::
getCallPreservedMask(CallingConv::ID) const {  
    return CSR_SingleFloatOnly_RegMask;
}

BitVector RX600RegisterInfo::
getReservedRegs(const MachineFunction &MF) const {
  static const uint16_t ReservedCPURegs[] = {
    RX600::ZERO, RX600::SP, RX600::RA, RX600::V0,
  };

  BitVector Reserved(getNumRegs());
  typedef TargetRegisterClass::iterator RegIter;

  for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
    Reserved.set(ReservedCPURegs[I]);

  return Reserved;
}

// ADJCALLSTACKDOWNとADJCALLSTACKUPを単純に削除する
void RX600RegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const {
  DEBUG(dbgs() << ">> RX600RegisterInfo::eliminateCallFramePseudoInstr <<\n";);
  MBB.erase(I);
}

// FrameIndexをスタックポインタに置き換える
void RX600RegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                    RegScavenger *RS) const {
  DEBUG(dbgs() << ">> RX600RegisterInfo::eliminateFrameIndex <<\n";);

  MachineInstr &MI = *II;
  const MachineFunction &MF = *MI.getParent()->getParent();

  unsigned opIndex;
  for (opIndex = 0; opIndex < MI.getNumOperands(); opIndex++) {
    if (MI.getOperand(opIndex).isFI()) break;
  }
  assert(opIndex < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");

  int FrameIndex = MI.getOperand(opIndex).getIndex();
  uint64_t stackSize = MF.getFrameInfo()->getStackSize();
  int64_t spOffset = MF.getFrameInfo()->getObjectOffset(FrameIndex);
  int64_t Offset = spOffset + stackSize + MI.getOperand(opIndex+1).getImm();
  unsigned FrameReg = RX600::SP;

  DEBUG(errs() 
        << "\nFunction : " << MF.getFunction()->getName() << "\n"
        << "<--------->\n" << MI
        << "FrameIndex : " << FrameIndex << "\n"
        << "spOffset   : " << spOffset << "\n"
        << "stackSize  : " << stackSize << "\n"
        << "Offset     : " << Offset << "\n" << "<--------->\n");

  DEBUG(errs() << "Before:" << MI);
  MI.getOperand(opIndex).ChangeToRegister(FrameReg, false);
  MI.getOperand(opIndex+1).ChangeToImmediate(Offset);
  DEBUG(errs() << "After:" << MI);
}

unsigned RX600RegisterInfo::
getFrameRegister(const MachineFunction &MF) const {
    return RX600::SP;
}
