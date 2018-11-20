//===-- RX600ISelLowering.h - RX600 DAG Lowering Interface --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that RX600 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_ISELLOWERING_H
#define SAMPLE_ISELLOWERING_H

#include "RX600.h"
#include "RX600Subtarget.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/Target/TargetLowering.h"

namespace llvm {
namespace RX600ISD {
  enum {
    FIRST_NUMBER = ISD::BUILTIN_OP_END,

    // Jump and link (call)
    Call,

    // Return
    Ret
  };
}

class RX600Subtarget;
//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//

class RX600TargetLowering : public TargetLowering {
  const RX600Subtarget &Subtarget;

 public:
  explicit RX600TargetLowering(RX600TargetMachine &TM);

  /// LowerOperation - Provide custom lowering hooks for some operations.
  virtual SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const;
  virtual SDValue
  LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                       bool isVarArg,
                       const SmallVectorImpl<ISD::InputArg> &Ins,
                       DebugLoc dl, SelectionDAG &DAG,
                       SmallVectorImpl<SDValue> &InVals) const;

  virtual SDValue
  LowerCall(CallLoweringInfo &CLI,
            SmallVectorImpl<SDValue> &InVals) const;

  virtual SDValue
  LowerCallResult(SDValue Chain, SDValue InFlag,
                  CallingConv::ID CallConv, bool isVarArg,
                  const SmallVectorImpl<ISD::InputArg> &Ins,
                  DebugLoc dl, SelectionDAG &DAG,
                  SmallVectorImpl<SDValue> &InVals) const;

  virtual SDValue
  LowerReturn(SDValue Chain,
              CallingConv::ID CallConv, bool isVarArg,
              const SmallVectorImpl<ISD::OutputArg> &Outs,
              const SmallVectorImpl<SDValue> &OutVals,
              DebugLoc dl, SelectionDAG &DAG) const;

 private:
};
} // end of namespace llvm

#endif // SAMPLE_ISELLOWERING_H
