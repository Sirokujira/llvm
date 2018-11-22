//===- SampleTargetStreamer.h - Sample Target Streamer ----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Sample_SampleTARGETSTREAMER_H
#define LLVM_LIB_TARGET_Sample_SampleTARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {

class SampleTargetStreamer : public MCTargetStreamer {
public:
  SampleTargetStreamer(MCStreamer &S);
  ~SampleTargetStreamer() override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_Sample_SampleTARGETSTREAMER_H
