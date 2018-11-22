//===- SampleMachineFunctionInfo.h - Sample machine function info -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares Sample-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Sample_SampleMACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_Sample_SampleMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"
#include <vector>

namespace llvm {

/// SampleFunctionInfo - This class is derived from MachineFunction private
/// Sample target-specific information for each MachineFunction.
class SampleFunctionInfo : public MachineFunctionInfo {
  virtual void anchor();
  bool ReturnStackOffsetSet;
  int VarArgsFrameIndex;
  unsigned ReturnStackOffset;

public:
  SampleFunctionInfo()
      : ReturnStackOffsetSet(false), VarArgsFrameIndex(0),
        ReturnStackOffset(-1U), MaxCallStackReq(0) {}

  explicit SampleFunctionInfo(MachineFunction &MF)
      : ReturnStackOffsetSet(false), VarArgsFrameIndex(0),
        ReturnStackOffset(-1U), MaxCallStackReq(0) {
    // Functions are 4-byte (2**2) aligned.
    MF.setAlignment(2);
  }

  ~SampleFunctionInfo() {}

  void setVarArgsFrameIndex(int off) { VarArgsFrameIndex = off; }
  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }

  void setReturnStackOffset(unsigned value) {
    assert(!ReturnStackOffsetSet && "Return stack offset set twice");
    ReturnStackOffset = value;
    ReturnStackOffsetSet = true;
  }

  unsigned getReturnStackOffset() const {
    assert(ReturnStackOffsetSet && "Return stack offset not set");
    return ReturnStackOffset;
  }

  unsigned MaxCallStackReq;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_Sample_SampleMACHINEFUNCTIONINFO_H
