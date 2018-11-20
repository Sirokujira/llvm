//===-- RX600MachineFunctionInfo.h - Private data used for RX600 ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the RX600 specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_MACHINE_FUNCTION_INFO_H
#define SAMPLE_MACHINE_FUNCTION_INFO_H

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include <utility>

namespace llvm {

/// RX600FunctionInfo - This class is derived from MachineFunction private
/// RX600 target-specific information for each MachineFunction.
class RX600MachineFunctionInfo : public MachineFunctionInfo {
  virtual void anchor();

public:
  RX600MachineFunctionInfo(MachineFunction& MF) {}
};
} // end of namespace llvm

#endif // SAMPLE_MACHINE_FUNCTION_INFO_H
