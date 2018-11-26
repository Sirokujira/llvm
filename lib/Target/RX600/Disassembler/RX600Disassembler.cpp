//===-- RX600Disassembler.cpp - Disassembler for RX600 --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the RX600Disassembler class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/RX600MCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "rx600-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
class RX600Disassembler : public MCDisassembler {

public:
  RX600Disassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &VStream,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

static MCDisassembler *createRX600Disassembler(const Target &T,
                                               const MCSubtargetInfo &STI,
                                               MCContext &Ctx) {
  return new RX600Disassembler(STI, Ctx);
}

extern "C" void LLVMInitializeRX600Disassembler() {
  // Register the disassembler for each target.
  TargetRegistry::RegisterMCDisassembler(getTheRX600Target(),
                                         createRX600Disassembler);
}

static const unsigned GPRDecoderTable[] = {
  RX600::X0,  RX600::X1,  RX600::X2,  RX600::X3,
  RX600::X4,  RX600::X5,  RX600::X6,  RX600::X7,
  RX600::X8,  RX600::X9,  RX600::X10, RX600::X11,
  RX600::X12, RX600::X13, RX600::X14, RX600::X15,
  RX600::X16, RX600::X17, RX600::X18, RX600::X19,
  RX600::X20, RX600::X21, RX600::X22, RX600::X23,
  RX600::X24, RX600::X25, RX600::X26, RX600::X27,
  RX600::X28, RX600::X29, RX600::X30, RX600::X31
};

static DecodeStatus DecodeGPRRegisterClass(MCInst &Inst, uint64_t RegNo,
                                           uint64_t Address,
                                           const void *Decoder) {
  if (RegNo > sizeof(GPRDecoderTable))
    return MCDisassembler::Fail;

  // We must define our own mapping from RegNo to register identifier.
  // Accessing index RegNo in the register class will work in the case that
  // registers were added in ascending order, but not in general.
  unsigned Reg = GPRDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static const unsigned FPR32DecoderTable[] = {
  RX600::F0_32,  RX600::F1_32,  RX600::F2_32,  RX600::F3_32,
  RX600::F4_32,  RX600::F5_32,  RX600::F6_32,  RX600::F7_32,
  RX600::F8_32,  RX600::F9_32,  RX600::F10_32, RX600::F11_32,
  RX600::F12_32, RX600::F13_32, RX600::F14_32, RX600::F15_32,
  RX600::F16_32, RX600::F17_32, RX600::F18_32, RX600::F19_32,
  RX600::F20_32, RX600::F21_32, RX600::F22_32, RX600::F23_32,
  RX600::F24_32, RX600::F25_32, RX600::F26_32, RX600::F27_32,
  RX600::F28_32, RX600::F29_32, RX600::F30_32, RX600::F31_32
};

static DecodeStatus DecodeFPR32RegisterClass(MCInst &Inst, uint64_t RegNo,
                                             uint64_t Address,
                                             const void *Decoder) {
  if (RegNo > sizeof(FPR32DecoderTable))
    return MCDisassembler::Fail;

  // We must define our own mapping from RegNo to register identifier.
  // Accessing index RegNo in the register class will work in the case that
  // registers were added in ascending order, but not in general.
  unsigned Reg = FPR32DecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeFPR32CRegisterClass(MCInst &Inst, uint64_t RegNo,
                                              uint64_t Address,
                                              const void *Decoder) {
  if (RegNo > 8) {
    return MCDisassembler::Fail;
  }
  unsigned Reg = FPR32DecoderTable[RegNo + 8];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static const unsigned FPR64DecoderTable[] = {
  RX600::F0_64,  RX600::F1_64,  RX600::F2_64,  RX600::F3_64,
  RX600::F4_64,  RX600::F5_64,  RX600::F6_64,  RX600::F7_64,
  RX600::F8_64,  RX600::F9_64,  RX600::F10_64, RX600::F11_64,
  RX600::F12_64, RX600::F13_64, RX600::F14_64, RX600::F15_64,
  RX600::F16_64, RX600::F17_64, RX600::F18_64, RX600::F19_64,
  RX600::F20_64, RX600::F21_64, RX600::F22_64, RX600::F23_64,
  RX600::F24_64, RX600::F25_64, RX600::F26_64, RX600::F27_64,
  RX600::F28_64, RX600::F29_64, RX600::F30_64, RX600::F31_64
};

static DecodeStatus DecodeFPR64RegisterClass(MCInst &Inst, uint64_t RegNo,
                                             uint64_t Address,
                                             const void *Decoder) {
  if (RegNo > sizeof(FPR64DecoderTable))
    return MCDisassembler::Fail;

  // We must define our own mapping from RegNo to register identifier.
  // Accessing index RegNo in the register class will work in the case that
  // registers were added in ascending order, but not in general.
  unsigned Reg = FPR64DecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeFPR64CRegisterClass(MCInst &Inst, uint64_t RegNo,
                                              uint64_t Address,
                                              const void *Decoder) {
  if (RegNo > 8) {
    return MCDisassembler::Fail;
  }
  unsigned Reg = FPR64DecoderTable[RegNo + 8];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeGPRNoX0RegisterClass(MCInst &Inst, uint64_t RegNo,
                                               uint64_t Address,
                                               const void *Decoder) {
  if (RegNo == 0) {
    return MCDisassembler::Fail;
  }

  return DecodeGPRRegisterClass(Inst, RegNo, Address, Decoder);
}

static DecodeStatus DecodeGPRNoX0X2RegisterClass(MCInst &Inst, uint64_t RegNo,
                                                 uint64_t Address,
                                                 const void *Decoder) {
  if (RegNo == 2) {
    return MCDisassembler::Fail;
  }

  return DecodeGPRNoX0RegisterClass(Inst, RegNo, Address, Decoder);
}

static DecodeStatus DecodeGPRCRegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t Address,
                                            const void *Decoder) {
  if (RegNo > 8)
    return MCDisassembler::Fail;

  unsigned Reg = GPRDecoderTable[RegNo + 8];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

// Add implied SP operand for instructions *SP compressed instructions. The SP
// operand isn't explicitly encoded in the instruction.
static void addImplySP(MCInst &Inst, int64_t Address, const void *Decoder) {
  if (Inst.getOpcode() == RX600::C_LWSP || Inst.getOpcode() == RX600::C_SWSP ||
      Inst.getOpcode() == RX600::C_LDSP || Inst.getOpcode() == RX600::C_SDSP ||
      Inst.getOpcode() == RX600::C_FLWSP ||
      Inst.getOpcode() == RX600::C_FSWSP ||
      Inst.getOpcode() == RX600::C_FLDSP ||
      Inst.getOpcode() == RX600::C_FSDSP ||
      Inst.getOpcode() == RX600::C_ADDI4SPN) {
    DecodeGPRRegisterClass(Inst, 2, Address, Decoder);
  }
  if (Inst.getOpcode() == RX600::C_ADDI16SP) {
    DecodeGPRRegisterClass(Inst, 2, Address, Decoder);
    DecodeGPRRegisterClass(Inst, 2, Address, Decoder);
  }
}

template <unsigned N>
static DecodeStatus decodeUImmOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  addImplySP(Inst, Address, Decoder);
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeSImmOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  addImplySP(Inst, Address, Decoder);
  // Sign-extend the number in the bottom N bits of Imm
  Inst.addOperand(MCOperand::createImm(SignExtend64<N>(Imm)));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeSImmOperandAndLsl1(MCInst &Inst, uint64_t Imm,
                                             int64_t Address,
                                             const void *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  // Sign-extend the number in the bottom N bits of Imm after accounting for
  // the fact that the N bit immediate is stored in N-1 bits (the LSB is
  // always zero)
  Inst.addOperand(MCOperand::createImm(SignExtend64<N>(Imm << 1)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeCLUIImmOperand(MCInst &Inst, uint64_t Imm,
                                         int64_t Address,
                                         const void *Decoder) {
  assert(isUInt<6>(Imm) && "Invalid immediate");
  if (Imm > 31) {
    Imm = (SignExtend64<6>(Imm) & 0xfffff);
  }
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

#include "RX600GenDisassemblerTables.inc"

DecodeStatus RX600Disassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                               ArrayRef<uint8_t> Bytes,
                                               uint64_t Address,
                                               raw_ostream &OS,
                                               raw_ostream &CS) const {
  // TODO: This will need modification when supporting instruction set
  // extensions with instructions > 32-bits (up to 176 bits wide).
  uint32_t Insn;
  DecodeStatus Result;

  // It's a 32 bit instruction if bit 0 and 1 are 1.
  if ((Bytes[0] & 0x3) == 0x3) {
    Insn = support::endian::read32le(Bytes.data());
    LLVM_DEBUG(dbgs() << "Trying RX600 table :\n");
    Result = decodeInstruction(DecoderTable32, MI, Insn, Address, this, STI);
    Size = 4;
  } else {
    Insn = support::endian::read16le(Bytes.data());

    if (!STI.getFeatureBits()[RX600::Feature64Bit]) {
      LLVM_DEBUG(
          dbgs() << "Trying RX600Only_16 table (16-bit Instruction):\n");
      // Calling the auto-generated decoder function.
      Result = decodeInstruction(DecoderTableRV32Only_16, MI, Insn, Address,
                                 this, STI);
      if (Result != MCDisassembler::Fail) {
        Size = 2;
        return Result;
      }
    }

    LLVM_DEBUG(dbgs() << "Trying RX600_C table (16-bit Instruction):\n");
    // Calling the auto-generated decoder function.
    Result = decodeInstruction(DecoderTable16, MI, Insn, Address, this, STI);
    Size = 2;
  }

  return Result;
}
