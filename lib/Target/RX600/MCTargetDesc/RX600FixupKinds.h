//===-- RX600FixupKinds.h - RX600 Specific Fixup Entries --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_RX600_MCTARGETDESC_RX600FIXUPKINDS_H
#define LLVM_LIB_TARGET_RX600_MCTARGETDESC_RX600FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

#undef RX600

namespace llvm {
namespace RX600 {
enum Fixups {
  // fixup_RX600_hi20 - 20-bit fixup corresponding to hi(foo) for
  // instructions like lui
  fixup_RX600_hi20 = FirstTargetFixupKind,
  // fixup_RX600_lo12_i - 12-bit fixup corresponding to lo(foo) for
  // instructions like addi
  fixup_RX600_lo12_i,
  // fixup_RX600_lo12_s - 12-bit fixup corresponding to lo(foo) for
  // the S-type store instructions
  fixup_RX600_lo12_s,
  // fixup_RX600_pcrel_hi20 - 20-bit fixup corresponding to pcrel_hi(foo) for
  // instructions like auipc
  fixup_RX600_pcrel_hi20,
  // fixup_RX600_pcrel_lo12_i - 12-bit fixup corresponding to pcrel_lo(foo) for
  // instructions like addi
  fixup_RX600_pcrel_lo12_i,
  // fixup_RX600_pcrel_lo12_s - 12-bit fixup corresponding to pcrel_lo(foo) for
  // the S-type store instructions
  fixup_RX600_pcrel_lo12_s,
  // fixup_RX600_jal - 20-bit fixup for symbol references in the jal
  // instruction
  fixup_RX600_jal,
  // fixup_RX600_branch - 12-bit fixup for symbol references in the branch
  // instructions
  fixup_RX600_branch,
  // fixup_RX600_rvc_jump - 11-bit fixup for symbol references in the
  // compressed jump instruction
  fixup_RX600_rvc_jump,
  // fixup_RX600_rvc_branch - 8-bit fixup for symbol references in the
  // compressed branch instruction
  fixup_RX600_rvc_branch,
  // fixup_RX600_call - A fixup representing a call attached to the auipc
  // instruction in a pair composed of adjacent auipc+jalr instructions.
  fixup_RX600_call,
  // fixup_RX600_relax - Used to generate an R_RX600_RELAX relocation type,
  // which indicates the linker may relax the instruction pair.
  fixup_RX600_relax,

  // fixup_RX600_invalid - used as a sentinel and a marker, must be last fixup
  fixup_RX600_invalid,
  NumTargetFixupKinds = fixup_RX600_invalid - FirstTargetFixupKind
};
} // end namespace RX600
} // end namespace llvm

#endif
