//===-- RX600TargetStreamer.cpp - RX600 Target Streamer Methods -----------===//
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

#include "RX600TargetStreamer.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

RX600TargetStreamer::RX600TargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

// This part is for ascii assembly output
RX600TargetAsmStreamer::RX600TargetAsmStreamer(MCStreamer &S,
                                               formatted_raw_ostream &OS)
    : RX600TargetStreamer(S), OS(OS) {}

void RX600TargetAsmStreamer::emitDirectiveOptionRVC() {
  OS << "\t.option\trvc\n";
}

void RX600TargetAsmStreamer::emitDirectiveOptionNoRVC() {
  OS << "\t.option\tnorvc\n";
}
