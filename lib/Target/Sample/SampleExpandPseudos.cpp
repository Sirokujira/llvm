//===- SampleExpandPseudosPass - Sample expand pseudo loads -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass expands stores with large offsets into an appropriate sequence.
//===----------------------------------------------------------------------===//

#include "Sample.h"
#include "SampleInstrInfo.h"
#include "SampleRegisterInfo.h"
#include "SampleSubtarget.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

#define DEBUG_TYPE "Sample-expand-pseudos"

namespace {

class SampleExpandPseudos : public MachineFunctionPass {
public:
  static char ID;
  SampleExpandPseudos() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &Fn) override;

  StringRef getPassName() const override { return "Sample Expand Pseudos"; }

private:
  void ExpandStore(MachineFunction &, MachineBasicBlock::iterator);

  const SampleInstrInfo *TII;
};

char SampleExpandPseudos::ID = 0;

} // end anonymous namespace

static unsigned getMappedOp(unsigned PseudoOp) {
  switch (PseudoOp) {
  case Sample::ST_FAR:
    return Sample::ST_rs9;
  case Sample::STH_FAR:
    return Sample::STH_rs9;
  case Sample::STB_FAR:
    return Sample::STB_rs9;
  default:
    llvm_unreachable("Unhandled pseudo op.");
  }
}

void SampleExpandPseudos::ExpandStore(MachineFunction &MF,
                                   MachineBasicBlock::iterator SII) {
  MachineInstr &SI = *SII;
  unsigned AddrReg = MF.getRegInfo().createVirtualRegister(&Sample::GPR32RegClass);
  unsigned AddOpc =
      isUInt<6>(SI.getOperand(2).getImm()) ? Sample::ADD_rru6 : Sample::ADD_rrlimm;
  BuildMI(*SI.getParent(), SI, SI.getDebugLoc(), TII->get(AddOpc), AddrReg)
      .addReg(SI.getOperand(1).getReg())
      .addImm(SI.getOperand(2).getImm());
  BuildMI(*SI.getParent(), SI, SI.getDebugLoc(),
          TII->get(getMappedOp(SI.getOpcode())))
      .addReg(SI.getOperand(0).getReg())
      .addReg(AddrReg)
      .addImm(0);
  SI.eraseFromParent();
}

bool SampleExpandPseudos::runOnMachineFunction(MachineFunction &MF) {
  const SampleSubtarget *STI = &MF.getSubtarget<SampleSubtarget>();
  TII = STI->getInstrInfo();
  bool ExpandedStore = false;
  for (auto &MBB : MF) {
    MachineBasicBlock::iterator MBBI = MBB.begin(), E = MBB.end();
    while (MBBI != E) {
      MachineBasicBlock::iterator NMBBI = std::next(MBBI);
      switch (MBBI->getOpcode()) {
      case Sample::ST_FAR:
      case Sample::STH_FAR:
      case Sample::STB_FAR:
        ExpandStore(MF, MBBI);
        ExpandedStore = true;
        break;
      default:
        break;
      }
      MBBI = NMBBI;
    }
  }
  return ExpandedStore;
}

FunctionPass *llvm::createSampleExpandPseudosPass() {
  return new SampleExpandPseudos();
}
