//===-- RX600TargetStreamer.h - RX600 Target Streamer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RX600_RX600TARGETSTREAMER_H
#define LLVM_LIB_TARGET_RX600_RX600TARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {

class RX600TargetStreamer : public MCTargetStreamer {
public:
  RX600TargetStreamer(MCStreamer &S);

  virtual void emitDirectiveOptionRVC() = 0;
  virtual void emitDirectiveOptionNoRVC() = 0;
};

// This part is for ascii assembly output
class RX600TargetAsmStreamer : public RX600TargetStreamer {
  formatted_raw_ostream &OS;

public:
  RX600TargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);

  void emitDirectiveOptionRVC() override;
  void emitDirectiveOptionNoRVC() override;
};

}
#endif
