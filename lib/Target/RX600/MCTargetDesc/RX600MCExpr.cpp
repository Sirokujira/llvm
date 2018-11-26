//===-- RX600MCExpr.cpp - RX600 specific MC expression classes ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the assembly expression modifiers
// accepted by the RX600 architecture (e.g. ":lo12:", ":gottprel_g1:", ...).
//
//===----------------------------------------------------------------------===//

#include "RX600.h"
#include "RX600MCExpr.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "rx600mcexpr"

const RX600MCExpr *RX600MCExpr::create(const MCExpr *Expr, VariantKind Kind,
                                       MCContext &Ctx) {
  return new (Ctx) RX600MCExpr(Expr, Kind);
}

void RX600MCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  bool HasVariant =
      ((getKind() != VK_RX600_None) && (getKind() != VK_RX600_CALL));
  if (HasVariant)
    OS << '%' << getVariantKindName(getKind()) << '(';
  Expr->print(OS, MAI);
  if (HasVariant)
    OS << ')';
}

bool RX600MCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                            const MCAsmLayout *Layout,
                                            const MCFixup *Fixup) const {
  if (!getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup))
    return false;

  // Some custom fixup types are not valid with symbol difference expressions
  if (Res.getSymA() && Res.getSymB()) {
    switch (getKind()) {
    default:
      return true;
    case VK_RX600_LO:
    case VK_RX600_HI:
    case VK_RX600_PCREL_LO:
    case VK_RX600_PCREL_HI:
      return false;
    }
  }

  return true;
}

void RX600MCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

RX600MCExpr::VariantKind RX600MCExpr::getVariantKindForName(StringRef name) {
  return StringSwitch<RX600MCExpr::VariantKind>(name)
      .Case("lo", VK_RX600_LO)
      .Case("hi", VK_RX600_HI)
      .Case("pcrel_lo", VK_RX600_PCREL_LO)
      .Case("pcrel_hi", VK_RX600_PCREL_HI)
      .Default(VK_RX600_Invalid);
}

StringRef RX600MCExpr::getVariantKindName(VariantKind Kind) {
  switch (Kind) {
  default:
    llvm_unreachable("Invalid ELF symbol kind");
  case VK_RX600_LO:
    return "lo";
  case VK_RX600_HI:
    return "hi";
  case VK_RX600_PCREL_LO:
    return "pcrel_lo";
  case VK_RX600_PCREL_HI:
    return "pcrel_hi";
  }
}

bool RX600MCExpr::evaluateAsConstant(int64_t &Res) const {
  MCValue Value;

  if (Kind == VK_RX600_PCREL_HI || Kind == VK_RX600_PCREL_LO ||
      Kind == VK_RX600_CALL)
    return false;

  if (!getSubExpr()->evaluateAsRelocatable(Value, nullptr, nullptr))
    return false;

  if (!Value.isAbsolute())
    return false;

  Res = evaluateAsInt64(Value.getConstant());
  return true;
}

int64_t RX600MCExpr::evaluateAsInt64(int64_t Value) const {
  switch (Kind) {
  default:
    llvm_unreachable("Invalid kind");
  case VK_RX600_LO:
    return SignExtend64<12>(Value);
  case VK_RX600_HI:
    // Add 1 if bit 11 is 1, to compensate for low 12 bits being negative.
    return ((Value + 0x800) >> 12) & 0xfffff;
  }
}
