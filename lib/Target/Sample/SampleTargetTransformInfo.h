//===- SampleTargetTransformInfo.h - Sample specific TTI --------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// \file
// This file contains a TargetTransformInfo::Concept conforming object specific
// to the Sample target machine. It uses the target's detailed information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Sample_SampleTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_Sample_SampleTARGETTRANSFORMINFO_H

#include "Sample.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

namespace llvm {

class SampleSubtarget;
class SampleTargetLowering;
class SampleTargetMachine;

class SampleTTIImpl : public BasicTTIImplBase<SampleTTIImpl> {
  using BaseT = BasicTTIImplBase<SampleTTIImpl>;
  friend BaseT;

  const SampleSubtarget *ST;
  const SampleTargetLowering *TLI;

  const SampleSubtarget *getST() const { return ST; }
  const SampleTargetLowering *getTLI() const { return TLI; }

public:
  explicit SampleTTIImpl(const SampleTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl()),
        TLI(ST->getTargetLowering()) {}

  // Provide value semantics. MSVC requires that we spell all of these out.
  SampleTTIImpl(const SampleTTIImpl &Arg)
      : BaseT(static_cast<const BaseT &>(Arg)), ST(Arg.ST), TLI(Arg.TLI) {}
  SampleTTIImpl(SampleTTIImpl &&Arg)
      : BaseT(std::move(static_cast<BaseT &>(Arg))), ST(std::move(Arg.ST)),
        TLI(std::move(Arg.TLI)) {}
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_Sample_SampleTARGETTRANSFORMINFO_H
