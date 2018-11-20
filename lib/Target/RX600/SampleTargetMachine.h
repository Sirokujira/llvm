//===-- RX600.h - Top-level interface for RX600 representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM RX600 back-end.
//
//===----------------------------------------------------------------------===//
#ifndef SAMPLE_TARGETMACHINE_H
#define SAMPLE_TARGETMACHINE_H

#include "RX600FrameLowering.h"
#include "RX600InstrInfo.h"
#include "RX600ISelLowering.h"
#include "RX600SelectionDAGInfo.h"
#include "RX600RegisterInfo.h"
#include "RX600Subtarget.h"
#include "llvm/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Support/Debug.h"

namespace llvm {

class Module;

class RX600TargetMachine : public LLVMTargetMachine {
  const DataLayout DL;
  RX600Subtarget Subtarget;
  RX600InstrInfo InstrInfo;
  RX600FrameLowering FrameLowering;
  RX600TargetLowering TLInfo;
  RX600SelectionDAGInfo TSInfo;

 public:
  RX600TargetMachine(const Target &T, StringRef TT,
                      StringRef CPU, StringRef FS, const TargetOptions &Options,
                      Reloc::Model RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);

  virtual const RX600InstrInfo *getInstrInfo() const {
    return &InstrInfo;
  }
  virtual const RX600Subtarget *getSubtargetImpl() const {
    return &Subtarget;
  }
  virtual const RX600RegisterInfo *getRegisterInfo() const {
    return &InstrInfo.getRegisterInfo();
  }
  virtual const DataLayout *getDataLayout() const {
    return &DL;
  }
  virtual const RX600TargetLowering *getTargetLowering() const {
    return &TLInfo;
  }
  virtual const RX600FrameLowering *getFrameLowering() const{
    return &FrameLowering;
  }
  virtual const RX600SelectionDAGInfo* getSelectionDAGInfo() const {
    return &TSInfo;
  }

  // Pass Pipeline Configuration
  virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
};
} // end namespace llvm

#endif
