//===-- RX600MCAsmInfo.h - RX600 asm properties --------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the RX600MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_TARGETASMINFO_H
#define SAMPLE_TARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  class StringRef;
  class Target;

  class RX600MCAsmInfo : public MCAsmInfo {
    virtual void anchor() {};
  public:
    explicit RX600MCAsmInfo(const Target &T, StringRef TT);
  };

} // namespace llvm

#endif
