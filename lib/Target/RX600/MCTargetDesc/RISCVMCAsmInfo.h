//===-- RX600MCAsmInfo.h - RX600 Asm Info ----------------------*- C++ -*--===//
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

#ifndef LLVM_LIB_TARGET_RX600_MCTARGETDESC_RX600MCASMINFO_H
#define LLVM_LIB_TARGET_RX600_MCTARGETDESC_RX600MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class RX600MCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit RX600MCAsmInfo(const Triple &TargetTriple);
};

} // namespace llvm

#endif
