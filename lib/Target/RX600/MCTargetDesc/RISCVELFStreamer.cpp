//===-- RX600ELFStreamer.cpp - RX600 ELF Target Streamer Methods ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides RX600 specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "RX600ELFStreamer.h"
#include "RX600MCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCSubtargetInfo.h"

using namespace llvm;

// This part is for ELF object output.
RX600TargetELFStreamer::RX600TargetELFStreamer(MCStreamer &S,
                                               const MCSubtargetInfo &STI)
    : RX600TargetStreamer(S) {
  MCAssembler &MCA = getStreamer().getAssembler();

  const FeatureBitset &Features = STI.getFeatureBits();

  unsigned EFlags = MCA.getELFHeaderEFlags();

  if (Features[RX600::FeatureStdExtC])
    EFlags |= ELF::EF_RX600_RVC;

  MCA.setELFHeaderEFlags(EFlags);
}

MCELFStreamer &RX600TargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}

void RX600TargetELFStreamer::emitDirectiveOptionRVC() {}
void RX600TargetELFStreamer::emitDirectiveOptionNoRVC() {}
