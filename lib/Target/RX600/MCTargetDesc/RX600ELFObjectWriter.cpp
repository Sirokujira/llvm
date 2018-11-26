//===-- RX600ELFObjectWriter.cpp - RX600 ELF Writer -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/RX600FixupKinds.h"
#include "MCTargetDesc/RX600MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class RX600ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  RX600ELFObjectWriter(uint8_t OSABI, bool Is64Bit);

  ~RX600ELFObjectWriter() override;

  // Return true if the given relocation must be with a symbol rather than
  // section plus offset.
  bool needsRelocateWithSymbol(const MCSymbol &Sym,
                               unsigned Type) const override {
    // TODO: this is very conservative, update once RX600 psABI requirements
    //       are clarified.
    return true;
  }

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

RX600ELFObjectWriter::RX600ELFObjectWriter(uint8_t OSABI, bool Is64Bit)
//    : MCELFObjectTargetWriter(Is64Bit, OSABI, ELF::EM_RX600,
    : MCELFObjectTargetWriter(Is64Bit, OSABI, ELF::EM_RX,   // already define ELF::EM_RX
                              /*HasRelocationAddend*/ true) {}

RX600ELFObjectWriter::~RX600ELFObjectWriter() {}

unsigned RX600ELFObjectWriter::getRelocType(MCContext &Ctx,
                                            const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {
  // Determine the type of the relocation
  switch ((unsigned)Fixup.getKind()) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case FK_Data_4:
    return ELF::R_RX600_32;
  case FK_Data_8:
    return ELF::R_RX600_64;
  case FK_Data_Add_1:
    return ELF::R_RX600_ADD8;
  case FK_Data_Add_2:
    return ELF::R_RX600_ADD16;
  case FK_Data_Add_4:
    return ELF::R_RX600_ADD32;
  case FK_Data_Add_8:
    return ELF::R_RX600_ADD64;
  case FK_Data_Sub_1:
    return ELF::R_RX600_SUB8;
  case FK_Data_Sub_2:
    return ELF::R_RX600_SUB16;
  case FK_Data_Sub_4:
    return ELF::R_RX600_SUB32;
  case FK_Data_Sub_8:
    return ELF::R_RX600_SUB64;
  case RX600::fixup_rx600_hi20:
    return ELF::R_RX600_HI20;
  case RX600::fixup_rx600_lo12_i:
    return ELF::R_RX600_LO12_I;
  case RX600::fixup_rx600_lo12_s:
    return ELF::R_RX600_LO12_S;
  case RX600::fixup_rx600_pcrel_hi20:
    return ELF::R_RX600_PCREL_HI20;
  case RX600::fixup_rx600_pcrel_lo12_i:
    return ELF::R_RX600_PCREL_LO12_I;
  case RX600::fixup_rx600_pcrel_lo12_s:
    return ELF::R_RX600_PCREL_LO12_S;
  case RX600::fixup_rx600_jal:
    return ELF::R_RX600_JAL;
  case RX600::fixup_rx600_branch:
    return ELF::R_RX600_BRANCH;
  case RX600::fixup_rx600_rvc_jump:
    return ELF::R_RX600_RVC_JUMP;
  case RX600::fixup_rx600_rvc_branch:
    return ELF::R_RX600_RVC_BRANCH;
  case RX600::fixup_rx600_call:
    return ELF::R_RX600_CALL;
  case RX600::fixup_rx600_relax:
    return ELF::R_RX600_RELAX;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createRX600ELFObjectWriter(uint8_t OSABI, bool Is64Bit) {
  return llvm::make_unique<RX600ELFObjectWriter>(OSABI, Is64Bit);
}
