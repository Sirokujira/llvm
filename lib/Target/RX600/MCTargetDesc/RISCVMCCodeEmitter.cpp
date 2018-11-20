//===-- RX600MCCodeEmitter.cpp - Convert RX600 code to machine code -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the RX600MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/RX600BaseInfo.h"
#include "MCTargetDesc/RX600FixupKinds.h"
#include "MCTargetDesc/RX600MCExpr.h"
#include "MCTargetDesc/RX600MCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");
STATISTIC(MCNumFixups, "Number of MC fixups created");

namespace {
class RX600MCCodeEmitter : public MCCodeEmitter {
  RX600MCCodeEmitter(const RX600MCCodeEmitter &) = delete;
  void operator=(const RX600MCCodeEmitter &) = delete;
  MCContext &Ctx;
  MCInstrInfo const &MCII;

public:
  RX600MCCodeEmitter(MCContext &ctx, MCInstrInfo const &MCII)
      : Ctx(ctx), MCII(MCII) {}

  ~RX600MCCodeEmitter() override {}

  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  void expandFunctionCall(const MCInst &MI, raw_ostream &OS,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;

  /// TableGen'erated function for getting the binary encoding for an
  /// instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// Return binary encoding of operand. If the machine operand requires
  /// relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  unsigned getImmOpValueAsr1(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  unsigned getImmOpValue(const MCInst &MI, unsigned OpNo,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const;
};
} // end anonymous namespace

MCCodeEmitter *llvm::createRX600MCCodeEmitter(const MCInstrInfo &MCII,
                                              const MCRegisterInfo &MRI,
                                              MCContext &Ctx) {
  return new RX600MCCodeEmitter(Ctx, MCII);
}

// Expand PseudoCALL and PseudoTAIL to AUIPC and JALR with relocation types.
// We expand PseudoCALL and PseudoTAIL while encoding, meaning AUIPC and JALR
// won't go through RX600 MC to MC compressed instruction transformation. This
// is acceptable because AUIPC has no 16-bit form and C_JALR have no immediate
// operand field.  We let linker relaxation deal with it. When linker
// relaxation enabled, AUIPC and JALR have chance relax to JAL. If C extension
// is enabled, JAL has chance relax to C_JAL.
void RX600MCCodeEmitter::expandFunctionCall(const MCInst &MI, raw_ostream &OS,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCInst TmpInst;
  MCOperand Func = MI.getOperand(0);
  unsigned Ra = (MI.getOpcode() == RX600::PseudoTAIL) ? RX600::X6 : RX600::X1;
  uint32_t Binary;

  assert(Func.isExpr() && "Expected expression");

  const MCExpr *Expr = Func.getExpr();

  // Create function call expression CallExpr for AUIPC.
  const MCExpr *CallExpr =
      RX600MCExpr::create(Expr, RX600MCExpr::VK_RX600_CALL, Ctx);

  // Emit AUIPC Ra, Func with R_RX600_CALL relocation type.
  TmpInst = MCInstBuilder(RX600::AUIPC)
                .addReg(Ra)
                .addOperand(MCOperand::createExpr(CallExpr));
  Binary = getBinaryCodeForInstr(TmpInst, Fixups, STI);
  support::endian::write(OS, Binary, support::little);

  if (MI.getOpcode() == RX600::PseudoTAIL)
    // Emit JALR X0, X6, 0
    TmpInst = MCInstBuilder(RX600::JALR).addReg(RX600::X0).addReg(Ra).addImm(0);
  else
    // Emit JALR X1, X1, 0
    TmpInst = MCInstBuilder(RX600::JALR).addReg(Ra).addReg(Ra).addImm(0);
  Binary = getBinaryCodeForInstr(TmpInst, Fixups, STI);
  support::endian::write(OS, Binary, support::little);
}

void RX600MCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
  // Get byte count of instruction.
  unsigned Size = Desc.getSize();

  if (MI.getOpcode() == RX600::PseudoCALL ||
      MI.getOpcode() == RX600::PseudoTAIL) {
    expandFunctionCall(MI, OS, Fixups, STI);
    MCNumEmitted += 2;
    return;
  }

  switch (Size) {
  default:
    llvm_unreachable("Unhandled encodeInstruction length!");
  case 2: {
    uint16_t Bits = getBinaryCodeForInstr(MI, Fixups, STI);
    support::endian::write<uint16_t>(OS, Bits, support::little);
    break;
  }
  case 4: {
    uint32_t Bits = getBinaryCodeForInstr(MI, Fixups, STI);
    support::endian::write(OS, Bits, support::little);
    break;
  }
  }

  ++MCNumEmitted; // Keep track of the # of mi's emitted.
}

unsigned
RX600MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const {

  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  llvm_unreachable("Unhandled expression!");
  return 0;
}

unsigned
RX600MCCodeEmitter::getImmOpValueAsr1(const MCInst &MI, unsigned OpNo,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);

  if (MO.isImm()) {
    unsigned Res = MO.getImm();
    assert((Res & 1) == 0 && "LSB is non-zero");
    return Res >> 1;
  }

  return getImmOpValue(MI, OpNo, Fixups, STI);
}

unsigned RX600MCCodeEmitter::getImmOpValue(const MCInst &MI, unsigned OpNo,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  bool EnableRelax = STI.getFeatureBits()[RX600::FeatureRelax];
  const MCOperand &MO = MI.getOperand(OpNo);

  MCInstrDesc const &Desc = MCII.get(MI.getOpcode());
  unsigned MIFrm = Desc.TSFlags & RX600II::InstFormatMask;

  // If the destination is an immediate, there is nothing to do.
  if (MO.isImm())
    return MO.getImm();

  assert(MO.isExpr() &&
         "getImmOpValue expects only expressions or immediates");
  const MCExpr *Expr = MO.getExpr();
  MCExpr::ExprKind Kind = Expr->getKind();
  RX600::Fixups FixupKind = RX600::fixup_riscv_invalid;
  if (Kind == MCExpr::Target) {
    const RX600MCExpr *RVExpr = cast<RX600MCExpr>(Expr);

    switch (RVExpr->getKind()) {
    case RX600MCExpr::VK_RX600_None:
    case RX600MCExpr::VK_RX600_Invalid:
      llvm_unreachable("Unhandled fixup kind!");
    case RX600MCExpr::VK_RX600_LO:
      if (MIFrm == RX600II::InstFormatI)
        FixupKind = RX600::fixup_riscv_lo12_i;
      else if (MIFrm == RX600II::InstFormatS)
        FixupKind = RX600::fixup_riscv_lo12_s;
      else
        llvm_unreachable("VK_RX600_LO used with unexpected instruction format");
      break;
    case RX600MCExpr::VK_RX600_HI:
      FixupKind = RX600::fixup_riscv_hi20;
      break;
    case RX600MCExpr::VK_RX600_PCREL_LO:
      if (MIFrm == RX600II::InstFormatI)
        FixupKind = RX600::fixup_riscv_pcrel_lo12_i;
      else if (MIFrm == RX600II::InstFormatS)
        FixupKind = RX600::fixup_riscv_pcrel_lo12_s;
      else
        llvm_unreachable(
            "VK_RX600_PCREL_LO used with unexpected instruction format");
      break;
    case RX600MCExpr::VK_RX600_PCREL_HI:
      FixupKind = RX600::fixup_riscv_pcrel_hi20;
      break;
    case RX600MCExpr::VK_RX600_CALL:
      FixupKind = RX600::fixup_riscv_call;
      break;
    }
  } else if (Kind == MCExpr::SymbolRef &&
             cast<MCSymbolRefExpr>(Expr)->getKind() == MCSymbolRefExpr::VK_None) {
    if (Desc.getOpcode() == RX600::JAL) {
      FixupKind = RX600::fixup_riscv_jal;
    } else if (MIFrm == RX600II::InstFormatB) {
      FixupKind = RX600::fixup_riscv_branch;
    } else if (MIFrm == RX600II::InstFormatCJ) {
      FixupKind = RX600::fixup_riscv_rvc_jump;
    } else if (MIFrm == RX600II::InstFormatCB) {
      FixupKind = RX600::fixup_riscv_rvc_branch;
    }
  }

  assert(FixupKind != RX600::fixup_riscv_invalid && "Unhandled expression!");

  Fixups.push_back(
      MCFixup::create(0, Expr, MCFixupKind(FixupKind), MI.getLoc()));
  ++MCNumFixups;

  if (EnableRelax) {
    if (FixupKind == RX600::fixup_riscv_call) {
      Fixups.push_back(
      MCFixup::create(0, Expr, MCFixupKind(RX600::fixup_riscv_relax),
                      MI.getLoc()));
      ++MCNumFixups;
    }
  }

  return 0;
}

#include "RX600GenMCCodeEmitter.inc"
