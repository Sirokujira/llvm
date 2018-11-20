//===-- RX600Subtarget.h - Define Subtarget for the RX600 -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the RX600 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RX600_RX600SUBTARGET_H
#define LLVM_LIB_TARGET_RX600_RX600SUBTARGET_H

#include "RX600FrameLowering.h"
#include "RX600ISelLowering.h"
#include "RX600InstrInfo.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

#define GET_SUBTARGETINFO_HEADER
#include "RX600GenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class RX600Subtarget : public RX600GenSubtargetInfo {
  virtual void anchor();
  bool HasStdExtM = false;
  bool HasStdExtA = false;
  bool HasStdExtF = false;
  bool HasStdExtD = false;
  bool HasStdExtC = false;
  bool HasRV64 = false;
  bool EnableLinkerRelax = false;
  unsigned XLen = 32;
  MVT XLenVT = MVT::i32;
  RX600FrameLowering FrameLowering;
  RX600InstrInfo InstrInfo;
  RX600RegisterInfo RegInfo;
  RX600TargetLowering TLInfo;
  SelectionDAGTargetInfo TSInfo;

  /// Initializes using the passed in CPU and feature strings so that we can
  /// use initializer lists for subtarget initialization.
  RX600Subtarget &initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                                  bool Is64Bit);

public:
  // Initializes the data members to match that of the specified triple.
  RX600Subtarget(const Triple &TT, const std::string &CPU,
                 const std::string &FS, const TargetMachine &TM);

  // Parses features string setting specified subtarget options. The
  // definition of this function is auto-generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  const RX600FrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const RX600InstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const RX600RegisterInfo *getRegisterInfo() const override {
    return &RegInfo;
  }
  const RX600TargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }
  bool hasStdExtM() const { return HasStdExtM; }
  bool hasStdExtA() const { return HasStdExtA; }
  bool hasStdExtF() const { return HasStdExtF; }
  bool hasStdExtD() const { return HasStdExtD; }
  bool hasStdExtC() const { return HasStdExtC; }
  bool is64Bit() const { return HasRV64; }
  bool enableLinkerRelax() const { return EnableLinkerRelax; }
  MVT getXLenVT() const { return XLenVT; }
  unsigned getXLen() const { return XLen; }
};
} // End llvm namespace

#endif
