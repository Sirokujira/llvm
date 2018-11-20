//===-- RX600ELFStreamer.h - RX600 ELF Target Streamer ---------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RX600_RX600ELFSTREAMER_H
#define LLVM_LIB_TARGET_RX600_RX600ELFSTREAMER_H

#include "RX600TargetStreamer.h"
#include "llvm/MC/MCELFStreamer.h"

namespace llvm {

class RX600TargetELFStreamer : public RX600TargetStreamer {
public:
  MCELFStreamer &getStreamer();
  RX600TargetELFStreamer(MCStreamer &S, const MCSubtargetInfo &STI);

  virtual void emitDirectiveOptionRVC();
  virtual void emitDirectiveOptionNoRVC();
};
}
#endif
