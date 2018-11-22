//===- SampleBranchFinalize.cpp - Sample conditional branches ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass takes existing conditional branches and expands them into longer
// range conditional branches.
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "Sample-branch-finalize"

#include "SampleInstrInfo.h"
#include "SampleTargetMachine.h"
#include "MCTargetDesc/SampleInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/Debug.h"
#include <vector>

using namespace llvm;

namespace llvm {

void initializeSampleBranchFinalizePass(PassRegistry &Registry);
FunctionPass *createSampleBranchFinalizePass();

} // end namespace llvm

namespace {

class SampleBranchFinalize : public MachineFunctionPass {
public:
  static char ID;

  SampleBranchFinalize() : MachineFunctionPass(ID) {
    initializeSampleBranchFinalizePass(*PassRegistry::getPassRegistry());
  }

  StringRef getPassName() const override {
    return "Sample Branch Finalization Pass";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;
  void replaceWithBRcc(MachineInstr *MI) const;
  void replaceWithCmpBcc(MachineInstr *MI) const;

private:
  const SampleInstrInfo *TII{nullptr};
};

char SampleBranchFinalize::ID = 0;

} // end anonymous namespace

INITIALIZE_PASS_BEGIN(SampleBranchFinalize, "Sample-branch-finalize",
                      "Sample finalize branches", false, false)
INITIALIZE_PASS_DEPENDENCY(MachineDominatorTree)
INITIALIZE_PASS_END(SampleBranchFinalize, "Sample-branch-finalize",
                    "Sample finalize branches", false, false)

// BRcc has 6 supported condition codes, which differ from the 16
// condition codes supported in the predicated instructions:
// EQ -- 000
// NE -- 001
// LT -- 010
// GE -- 011
// LO -- 100
// HS -- 101
static unsigned getCCForBRcc(unsigned CC) {
  switch (CC) {
  case SampleCC::EQ:
    return 0;
  case SampleCC::NE:
    return 1;
  case SampleCC::LT:
    return 2;
  case SampleCC::GE:
    return 3;
  case SampleCC::LO:
    return 4;
  case SampleCC::HS:
    return 5;
  default:
    return -1U;
  }
}

static bool isBRccPseudo(MachineInstr *MI) {
  return !(MI->getOpcode() != Sample::BRcc_rr_p &&
           MI->getOpcode() != Sample::BRcc_ru6_p);
}

static unsigned getBRccForPseudo(MachineInstr *MI) {
  assert(isBRccPseudo(MI) && "Can't get BRcc for wrong instruction.");
  if (MI->getOpcode() == Sample::BRcc_rr_p)
    return Sample::BRcc_rr;
  return Sample::BRcc_ru6;
}

static unsigned getCmpForPseudo(MachineInstr *MI) {
  assert(isBRccPseudo(MI) && "Can't get BRcc for wrong instruction.");
  if (MI->getOpcode() == Sample::BRcc_rr_p)
    return Sample::CMP_rr;
  return Sample::CMP_ru6;
}

void SampleBranchFinalize::replaceWithBRcc(MachineInstr *MI) const {
  LLVM_DEBUG(dbgs() << "Replacing pseudo branch with BRcc\n");
  unsigned CC = getCCForBRcc(MI->getOperand(3).getImm());
  if (CC != -1U) {
    BuildMI(*MI->getParent(), MI, MI->getDebugLoc(),
            TII->get(getBRccForPseudo(MI)))
        .addMBB(MI->getOperand(0).getMBB())
        .addReg(MI->getOperand(1).getReg())
        .add(MI->getOperand(2))
        .addImm(getCCForBRcc(MI->getOperand(3).getImm()));
    MI->eraseFromParent();
  } else {
    replaceWithCmpBcc(MI);
  }
}

void SampleBranchFinalize::replaceWithCmpBcc(MachineInstr *MI) const {
  LLVM_DEBUG(dbgs() << "Branch: " << *MI << "\n");
  LLVM_DEBUG(dbgs() << "Replacing pseudo branch with Cmp + Bcc\n");
  BuildMI(*MI->getParent(), MI, MI->getDebugLoc(),
          TII->get(getCmpForPseudo(MI)))
      .addReg(MI->getOperand(1).getReg())
      .add(MI->getOperand(2));
  BuildMI(*MI->getParent(), MI, MI->getDebugLoc(), TII->get(Sample::Bcc))
      .addMBB(MI->getOperand(0).getMBB())
      .addImm(MI->getOperand(3).getImm());
  MI->eraseFromParent();
}

bool SampleBranchFinalize::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(dbgs() << "Running Sample Branch Finalize on " << MF.getName()
                    << "\n");
  std::vector<MachineInstr *> Branches;
  bool Changed = false;
  unsigned MaxSize = 0;
  TII = MF.getSubtarget<SampleSubtarget>().getInstrInfo();
  std::map<MachineBasicBlock *, unsigned> BlockToPCMap;
  std::vector<std::pair<MachineInstr *, unsigned>> BranchToPCList;
  unsigned PC = 0;

  for (auto &MBB : MF) {
    BlockToPCMap.insert(std::make_pair(&MBB, PC));
    for (auto &MI : MBB) {
      unsigned Size = TII->getInstSizeInBytes(MI);
      if (Size > 8 || Size == 0) {
        LLVM_DEBUG(dbgs() << "Unknown (or size 0) size for: " << MI << "\n");
      } else {
        MaxSize += Size;
      }
      if (MI.isBranch()) {
        Branches.push_back(&MI);
        BranchToPCList.emplace_back(&MI, PC);
      }
      PC += Size;
    }
  }
  for (auto P : BranchToPCList) {
    if (isBRccPseudo(P.first))
      isInt<9>(MaxSize) ? replaceWithBRcc(P.first) : replaceWithCmpBcc(P.first);
  }

  LLVM_DEBUG(dbgs() << "Estimated function size for " << MF.getName() << ": "
                    << MaxSize << "\n");

  return Changed;
}

FunctionPass *llvm::createSampleBranchFinalizePass() {
  return new SampleBranchFinalize();
}
