//===-- RX600AsmParser.cpp - Parse RX600 assembly to MCInst instructions --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/RX600BaseInfo.h"
#include "MCTargetDesc/RX600MCExpr.h"
#include "MCTargetDesc/RX600MCTargetDesc.h"
#include "MCTargetDesc/RX600TargetStreamer.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TargetRegistry.h"

#include <limits>

using namespace llvm;

// Include the auto-generated portion of the compress emitter.
#define GEN_COMPRESS_INSTR
#include "RX600GenCompressInstEmitter.inc"

namespace {
struct RX600Operand;

class RX600AsmParser : public MCTargetAsmParser {
  SMLoc getLoc() const { return getParser().getTok().getLoc(); }
  bool isRX600() const { return getSTI().hasFeature(RX600::Feature64Bit); }

  RX600TargetStreamer &getTargetStreamer() {
    MCTargetStreamer &TS = *getParser().getStreamer().getTargetStreamer();
    return static_cast<RX600TargetStreamer &>(TS);
  }

  unsigned validateTargetOperandClass(MCParsedAsmOperand &Op,
                                      unsigned Kind) override;

  bool generateImmOutOfRangeError(OperandVector &Operands, uint64_t ErrorInfo,
                                  int64_t Lower, int64_t Upper, Twine Msg);

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  bool ParseDirective(AsmToken DirectiveID) override;

  // Helper to actually emit an instruction to the MCStreamer. Also, when
  // possible, compression of the instruction is performed.
  void emitToStreamer(MCStreamer &S, const MCInst &Inst);

  // Helper to emit a combination of LUI, ADDI(W), and SLLI instructions that
  // synthesize the desired immedate value into the destination register.
  void emitLoadImm(unsigned DestReg, int64_t Value, MCStreamer &Out);

  // Helper to emit pseudo instruction "lla" used in PC-rel addressing.
  void emitLoadLocalAddress(MCInst &Inst, SMLoc IDLoc, MCStreamer &Out);

  /// Helper for processing MC instructions that have been successfully matched
  /// by MatchAndEmitInstruction. Modifications to the emitted instructions,
  /// like the expansion of pseudo instructions (e.g., "li"), can be performed
  /// in this method.
  bool processInstruction(MCInst &Inst, SMLoc IDLoc, MCStreamer &Out);

// Auto-generated instruction matching functions
#define GET_ASSEMBLER_HEADER
#include "RX600GenAsmMatcher.inc"

  OperandMatchResultTy parseImmediate(OperandVector &Operands);
  OperandMatchResultTy parseRegister(OperandVector &Operands,
                                     bool AllowParens = false);
  OperandMatchResultTy parseMemOpBaseReg(OperandVector &Operands);
  OperandMatchResultTy parseOperandWithModifier(OperandVector &Operands);

  bool parseOperand(OperandVector &Operands, bool ForceImmediate);

  bool parseDirectiveOption();

  void setFeatureBits(uint64_t Feature, StringRef FeatureString) {
    if (!(getSTI().getFeatureBits()[Feature])) {
      MCSubtargetInfo &STI = copySTI();
      setAvailableFeatures(
          ComputeAvailableFeatures(STI.ToggleFeature(FeatureString)));
    }
  }

  void clearFeatureBits(uint64_t Feature, StringRef FeatureString) {
    if (getSTI().getFeatureBits()[Feature]) {
      MCSubtargetInfo &STI = copySTI();
      setAvailableFeatures(
          ComputeAvailableFeatures(STI.ToggleFeature(FeatureString)));
    }
  }
public:
  enum RX600MatchResultTy {
    Match_Dummy = FIRST_TARGET_MATCH_RESULT_TY,
#define GET_OPERAND_DIAGNOSTIC_TYPES
#include "RX600GenAsmMatcher.inc"
#undef GET_OPERAND_DIAGNOSTIC_TYPES
  };

  static bool classifySymbolRef(const MCExpr *Expr,
                                RX600MCExpr::VariantKind &Kind,
                                int64_t &Addend);

  RX600AsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
                 const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII) {
    Parser.addAliasForDirective(".half", ".2byte");
    Parser.addAliasForDirective(".hword", ".2byte");
    Parser.addAliasForDirective(".word", ".4byte");
    Parser.addAliasForDirective(".dword", ".8byte");
    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
  }
};

/// RX600Operand - Instances of this class represent a parsed machine
/// instruction
struct RX600Operand : public MCParsedAsmOperand {

  enum KindTy {
    Token,
    Register,
    Immediate,
  } Kind;

  bool IsRX600;

  struct RegOp {
    unsigned RegNum;
  };

  struct ImmOp {
    const MCExpr *Val;
  };

  SMLoc StartLoc, EndLoc;
  union {
    StringRef Tok;
    RegOp Reg;
    ImmOp Imm;
  };

  RX600Operand(KindTy K) : MCParsedAsmOperand(), Kind(K) {}

public:
  RX600Operand(const RX600Operand &o) : MCParsedAsmOperand() {
    Kind = o.Kind;
    IsRX600 = o.IsRX600;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;
    switch (Kind) {
    case Register:
      Reg = o.Reg;
      break;
    case Immediate:
      Imm = o.Imm;
      break;
    case Token:
      Tok = o.Tok;
      break;
    }
  }

  bool isToken() const override { return Kind == Token; }
  bool isReg() const override { return Kind == Register; }
  bool isImm() const override { return Kind == Immediate; }
  bool isMem() const override { return false; }

  bool evaluateConstantImm(int64_t &Imm, RX600MCExpr::VariantKind &VK) const {
    const MCExpr *Val = getImm();
    bool Ret = false;
    if (auto *RE = dyn_cast<RX600MCExpr>(Val)) {
      Ret = RE->evaluateAsConstant(Imm);
      VK = RE->getKind();
    } else if (auto CE = dyn_cast<MCConstantExpr>(Val)) {
      Ret = true;
      VK = RX600MCExpr::VK_RX600_None;
      Imm = CE->getValue();
    }
    return Ret;
  }

  // True if operand is a symbol with no modifiers, or a constant with no
  // modifiers and isShiftedInt<N-1, 1>(Op).
  template <int N> bool isBareSimmNLsb0() const {
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    if (!isImm())
      return false;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    bool IsValid;
    if (!IsConstantImm)
      IsValid = RX600AsmParser::classifySymbolRef(getImm(), VK, Imm);
    else
      IsValid = isShiftedInt<N - 1, 1>(Imm);
    return IsValid && VK == RX600MCExpr::VK_RX600_None;
  }

  // Predicate methods for AsmOperands defined in RX600InstrInfo.td

  bool isBareSymbol() const {
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    // Must be of 'immediate' type but not a constant.
    if (!isImm() || evaluateConstantImm(Imm, VK))
      return false;
    return RX600AsmParser::classifySymbolRef(getImm(), VK, Imm) &&
           VK == RX600MCExpr::VK_RX600_None;
  }

  /// Return true if the operand is a valid for the fence instruction e.g.
  /// ('iorw').
  bool isFenceArg() const {
    if (!isImm())
      return false;
    const MCExpr *Val = getImm();
    auto *SVal = dyn_cast<MCSymbolRefExpr>(Val);
    if (!SVal || SVal->getKind() != MCSymbolRefExpr::VK_None)
      return false;

    StringRef Str = SVal->getSymbol().getName();
    // Letters must be unique, taken from 'iorw', and in ascending order. This
    // holds as long as each individual character is one of 'iorw' and is
    // greater than the previous character.
    char Prev = '\0';
    for (char c : Str) {
      if (c != 'i' && c != 'o' && c != 'r' && c != 'w')
        return false;
      if (c <= Prev)
        return false;
      Prev = c;
    }
    return true;
  }

  /// Return true if the operand is a valid floating point rounding mode.
  bool isFRMArg() const {
    if (!isImm())
      return false;
    const MCExpr *Val = getImm();
    auto *SVal = dyn_cast<MCSymbolRefExpr>(Val);
    if (!SVal || SVal->getKind() != MCSymbolRefExpr::VK_None)
      return false;

    StringRef Str = SVal->getSymbol().getName();

    return RX600FPRndMode::stringToRoundingMode(Str) != RX600FPRndMode::Invalid;
  }

  bool isImmXLen() const {
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    if (!isImm())
      return false;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    // Given only Imm, ensuring that the actually specified constant is either
    // a signed or unsigned 64-bit number is unfortunately impossible.
    bool IsInRange = isRX600() ? true : isInt<32>(Imm) || isUInt<32>(Imm);
    return IsConstantImm && IsInRange && VK == RX600MCExpr::VK_RX600_None;
  }

  bool isUImmLog2XLen() const {
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    if (!isImm())
      return false;
    if (!evaluateConstantImm(Imm, VK) || VK != RX600MCExpr::VK_RX600_None)
      return false;
    return (isRX600() && isUInt<6>(Imm)) || isUInt<5>(Imm);
  }

  bool isUImmLog2XLenNonZero() const {
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    if (!isImm())
      return false;
    if (!evaluateConstantImm(Imm, VK) || VK != RX600MCExpr::VK_RX600_None)
      return false;
    if (Imm == 0)
      return false;
    return (isRX600() && isUInt<6>(Imm)) || isUInt<5>(Imm);
  }

  bool isUImm5() const {
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    if (!isImm())
      return false;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && isUInt<5>(Imm) && VK == RX600MCExpr::VK_RX600_None;
  }

  bool isUImm5NonZero() const {
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    if (!isImm())
      return false;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && isUInt<5>(Imm) && (Imm != 0) &&
           VK == RX600MCExpr::VK_RX600_None;
  }

  bool isSImm6() const {
    if (!isImm())
      return false;
    RX600MCExpr::VariantKind VK;
    int64_t Imm;
    bool IsValid;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    if (!IsConstantImm)
      IsValid = RX600AsmParser::classifySymbolRef(getImm(), VK, Imm);
    else
      IsValid = isInt<6>(Imm);
    return IsValid &&
           (VK == RX600MCExpr::VK_RX600_None || VK == RX600MCExpr::VK_RX600_LO);
  }

  bool isSImm6NonZero() const {
    if (!isImm())
      return false;
    RX600MCExpr::VariantKind VK;
    int64_t Imm;
    bool IsValid;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    if (!IsConstantImm)
      IsValid = RX600AsmParser::classifySymbolRef(getImm(), VK, Imm);
    else
      IsValid = ((Imm != 0) && isInt<6>(Imm));
    return IsValid &&
           (VK == RX600MCExpr::VK_RX600_None || VK == RX600MCExpr::VK_RX600_LO);
  }

  bool isCLUIImm() const {
    if (!isImm())
      return false;
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && (Imm != 0) &&
           (isUInt<5>(Imm) || (Imm >= 0xfffe0 && Imm <= 0xfffff)) &&
            VK == RX600MCExpr::VK_RX600_None;
  }

  bool isUImm7Lsb00() const {
    if (!isImm())
      return false;
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && isShiftedUInt<5, 2>(Imm) &&
           VK == RX600MCExpr::VK_RX600_None;
  }

  bool isUImm8Lsb00() const {
    if (!isImm())
      return false;
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && isShiftedUInt<6, 2>(Imm) &&
           VK == RX600MCExpr::VK_RX600_None;
  }

  bool isUImm8Lsb000() const {
    if (!isImm())
      return false;
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && isShiftedUInt<5, 3>(Imm) &&
           VK == RX600MCExpr::VK_RX600_None;
  }

  bool isSImm9Lsb0() const { return isBareSimmNLsb0<9>(); }

  bool isUImm9Lsb000() const {
    if (!isImm())
      return false;
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && isShiftedUInt<6, 3>(Imm) &&
           VK == RX600MCExpr::VK_RX600_None;
  }

  bool isUImm10Lsb00NonZero() const {
    if (!isImm())
      return false;
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && isShiftedUInt<8, 2>(Imm) && (Imm != 0) &&
           VK == RX600MCExpr::VK_RX600_None;
  }

  bool isSImm12() const {
    RX600MCExpr::VariantKind VK;
    int64_t Imm;
    bool IsValid;
    if (!isImm())
      return false;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    if (!IsConstantImm)
      IsValid = RX600AsmParser::classifySymbolRef(getImm(), VK, Imm);
    else
      IsValid = isInt<12>(Imm);
    return IsValid && (VK == RX600MCExpr::VK_RX600_None ||
                       VK == RX600MCExpr::VK_RX600_LO ||
                       VK == RX600MCExpr::VK_RX600_PCREL_LO);
  }

  bool isSImm12Lsb0() const { return isBareSimmNLsb0<12>(); }

  bool isUImm12() const {
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    if (!isImm())
      return false;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && isUInt<12>(Imm) && VK == RX600MCExpr::VK_RX600_None;
  }

  bool isSImm13Lsb0() const { return isBareSimmNLsb0<13>(); }

  bool isSImm10Lsb0000NonZero() const {
    if (!isImm())
      return false;
    int64_t Imm;
    RX600MCExpr::VariantKind VK;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    return IsConstantImm && (Imm != 0) && isShiftedInt<6, 4>(Imm) &&
           VK == RX600MCExpr::VK_RX600_None;
  }

  bool isUImm20() const {
    RX600MCExpr::VariantKind VK;
    int64_t Imm;
    bool IsValid;
    if (!isImm())
      return false;
    bool IsConstantImm = evaluateConstantImm(Imm, VK);
    if (!IsConstantImm)
      IsValid = RX600AsmParser::classifySymbolRef(getImm(), VK, Imm);
    else
      IsValid = isUInt<20>(Imm);
    return IsValid && (VK == RX600MCExpr::VK_RX600_None ||
                       VK == RX600MCExpr::VK_RX600_HI ||
                       VK == RX600MCExpr::VK_RX600_PCREL_HI);
  }

  bool isSImm21Lsb0() const { return isBareSimmNLsb0<21>(); }

  /// getStartLoc - Gets location of the first token of this operand
  SMLoc getStartLoc() const override { return StartLoc; }
  /// getEndLoc - Gets location of the last token of this operand
  SMLoc getEndLoc() const override { return EndLoc; }
  /// True if this operand is for an RX600 instruction
  bool isRX600() const { return IsRX600; }

  unsigned getReg() const override {
    assert(Kind == Register && "Invalid type access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert(Kind == Immediate && "Invalid type access!");
    return Imm.Val;
  }

  StringRef getToken() const {
    assert(Kind == Token && "Invalid type access!");
    return Tok;
  }

  void print(raw_ostream &OS) const override {
    switch (Kind) {
    case Immediate:
      OS << *getImm();
      break;
    case Register:
      OS << "<register x";
      OS << getReg() << ">";
      break;
    case Token:
      OS << "'" << getToken() << "'";
      break;
    }
  }

  static std::unique_ptr<RX600Operand> createToken(StringRef Str, SMLoc S,
                                                   bool IsRX600) {
    auto Op = make_unique<RX600Operand>(Token);
    Op->Tok = Str;
    Op->StartLoc = S;
    Op->EndLoc = S;
    Op->IsRX600 = IsRX600;
    return Op;
  }

  static std::unique_ptr<RX600Operand> createReg(unsigned RegNo, SMLoc S,
                                                 SMLoc E, bool IsRX600) {
    auto Op = make_unique<RX600Operand>(Register);
    Op->Reg.RegNum = RegNo;
    Op->StartLoc = S;
    Op->EndLoc = E;
    Op->IsRX600 = IsRX600;
    return Op;
  }

  static std::unique_ptr<RX600Operand> createImm(const MCExpr *Val, SMLoc S,
                                                 SMLoc E, bool IsRX600) {
    auto Op = make_unique<RX600Operand>(Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    Op->IsRX600 = IsRX600;
    return Op;
  }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    assert(Expr && "Expr shouldn't be null!");
    int64_t Imm = 0;
    bool IsConstant = false;
    if (auto *RE = dyn_cast<RX600MCExpr>(Expr)) {
      IsConstant = RE->evaluateAsConstant(Imm);
    } else if (auto *CE = dyn_cast<MCConstantExpr>(Expr)) {
      IsConstant = true;
      Imm = CE->getValue();
    }

    if (IsConstant)
      Inst.addOperand(MCOperand::createImm(Imm));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  // Used by the TableGen Code
  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }

  void addFenceArgOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    // isFenceArg has validated the operand, meaning this cast is safe
    auto SE = cast<MCSymbolRefExpr>(getImm());

    unsigned Imm = 0;
    for (char c : SE->getSymbol().getName()) {
      switch (c) {
        default: llvm_unreachable("FenceArg must contain only [iorw]");
        case 'i': Imm |= RX600FenceField::I; break;
        case 'o': Imm |= RX600FenceField::O; break;
        case 'r': Imm |= RX600FenceField::R; break;
        case 'w': Imm |= RX600FenceField::W; break;
      }
    }
    Inst.addOperand(MCOperand::createImm(Imm));
  }

  // Returns the rounding mode represented by this RX600Operand. Should only
  // be called after checking isFRMArg.
  RX600FPRndMode::RoundingMode getRoundingMode() const {
    // isFRMArg has validated the operand, meaning this cast is safe.
    auto SE = cast<MCSymbolRefExpr>(getImm());
    RX600FPRndMode::RoundingMode FRM =
        RX600FPRndMode::stringToRoundingMode(SE->getSymbol().getName());
    assert(FRM != RX600FPRndMode::Invalid && "Invalid rounding mode");
    return FRM;
  }

  void addFRMArgOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createImm(getRoundingMode()));
  }
};
} // end anonymous namespace.

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "RX600GenAsmMatcher.inc"

// Return the matching FPR64 register for the given FPR32.
// FIXME: Ideally this function could be removed in favour of using
// information from TableGen.
// conflict risc-v function
// unsigned convertFPR32ToFPR64(unsigned Reg) {
unsigned convert_RX_FPR32ToFPR64(unsigned Reg) {
  switch (Reg) {
    default:
      llvm_unreachable("Not a recognised FPR32 register");
    case RX600::F0_32: return RX600::F0_64;
    case RX600::F1_32: return RX600::F1_64;
    case RX600::F2_32: return RX600::F2_64;
    case RX600::F3_32: return RX600::F3_64;
    case RX600::F4_32: return RX600::F4_64;
    case RX600::F5_32: return RX600::F5_64;
    case RX600::F6_32: return RX600::F6_64;
    case RX600::F7_32: return RX600::F7_64;
    case RX600::F8_32: return RX600::F8_64;
    case RX600::F9_32: return RX600::F9_64;
    case RX600::F10_32: return RX600::F10_64;
    case RX600::F11_32: return RX600::F11_64;
    case RX600::F12_32: return RX600::F12_64;
    case RX600::F13_32: return RX600::F13_64;
    case RX600::F14_32: return RX600::F14_64;
    case RX600::F15_32: return RX600::F15_64;
    case RX600::F16_32: return RX600::F16_64;
    case RX600::F17_32: return RX600::F17_64;
    case RX600::F18_32: return RX600::F18_64;
    case RX600::F19_32: return RX600::F19_64;
    case RX600::F20_32: return RX600::F20_64;
    case RX600::F21_32: return RX600::F21_64;
    case RX600::F22_32: return RX600::F22_64;
    case RX600::F23_32: return RX600::F23_64;
    case RX600::F24_32: return RX600::F24_64;
    case RX600::F25_32: return RX600::F25_64;
    case RX600::F26_32: return RX600::F26_64;
    case RX600::F27_32: return RX600::F27_64;
    case RX600::F28_32: return RX600::F28_64;
    case RX600::F29_32: return RX600::F29_64;
    case RX600::F30_32: return RX600::F30_64;
    case RX600::F31_32: return RX600::F31_64;
  }
}

unsigned RX600AsmParser::validateTargetOperandClass(MCParsedAsmOperand &AsmOp,
                                                    unsigned Kind) {
  RX600Operand &Op = static_cast<RX600Operand &>(AsmOp);
  if (!Op.isReg())
    return Match_InvalidOperand;

  unsigned Reg = Op.getReg();
  bool IsRegFPR32 =
      RX600MCRegisterClasses[RX600::FPR32RegClassID].contains(Reg);
  bool IsRegFPR32C =
      RX600MCRegisterClasses[RX600::FPR32CRegClassID].contains(Reg);

  // Op.Reg.RegNum = Reg;
  // return Match_Success;
  // return Match_InvalidOperand;
  // As the parser couldn't differentiate an FPR32 from an FPR64, coerce the
  // register from FPR32 to FPR64 or FPR32C to FPR64C if necessary.
  if ((IsRegFPR32 && Kind == MCK_FPR64) ||
      (IsRegFPR32C && Kind == MCK_FPR64C)) {
    Op.Reg.RegNum = convert_RX_FPR32ToFPR64(Reg);
    return Match_Success;
  }
  return Match_InvalidOperand;
}

bool RX600AsmParser::generateImmOutOfRangeError(
    OperandVector &Operands, uint64_t ErrorInfo, int64_t Lower, int64_t Upper,
    Twine Msg = "immediate must be an integer in the range") {
  SMLoc ErrorLoc = ((RX600Operand &)*Operands[ErrorInfo]).getStartLoc();
  return Error(ErrorLoc, Msg + " [" + Twine(Lower) + ", " + Twine(Upper) + "]");
}

bool RX600AsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                             OperandVector &Operands,
                                             MCStreamer &Out,
                                             uint64_t &ErrorInfo,
                                             bool MatchingInlineAsm) {
  MCInst Inst;

  switch (MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm)) {
  default:
    break;
  case Match_Success:
    return processInstruction(Inst, IDLoc, Out);
  case Match_MissingFeature:
    return Error(IDLoc, "instruction use requires an option to be enabled");
  case Match_MnemonicFail:
    return Error(IDLoc, "unrecognized instruction mnemonic");
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0U) {
      if (ErrorInfo >= Operands.size())
        return Error(ErrorLoc, "too few operands for instruction");

      ErrorLoc = ((RX600Operand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }
    return Error(ErrorLoc, "invalid operand for instruction");
  }
  case Match_InvalidImmXLen:
    if (isRX600()) {
      SMLoc ErrorLoc = ((RX600Operand &)*Operands[ErrorInfo]).getStartLoc();
      return Error(ErrorLoc, "operand must be a constant 64-bit integer");
    }
    return generateImmOutOfRangeError(Operands, ErrorInfo,
                                      std::numeric_limits<int32_t>::min(),
                                      std::numeric_limits<uint32_t>::max());
  case Match_InvalidUImmLog2XLen:
    if (isRX600())
      return generateImmOutOfRangeError(Operands, ErrorInfo, 0, (1 << 6) - 1);
    return generateImmOutOfRangeError(Operands, ErrorInfo, 0, (1 << 5) - 1);
  case Match_InvalidUImmLog2XLenNonZero:
    if (isRX600())
      return generateImmOutOfRangeError(Operands, ErrorInfo, 1, (1 << 6) - 1);
    return generateImmOutOfRangeError(Operands, ErrorInfo, 1, (1 << 5) - 1);
  case Match_InvalidUImm5:
    return generateImmOutOfRangeError(Operands, ErrorInfo, 0, (1 << 5) - 1);
  case Match_InvalidSImm6:
    return generateImmOutOfRangeError(Operands, ErrorInfo, -(1 << 5),
                                      (1 << 5) - 1);
  case Match_InvalidSImm6NonZero:
    return generateImmOutOfRangeError(Operands, ErrorInfo, -(1 << 5),
                                      (1 << 5) - 1,
        "immediate must be non-zero in the range");
  case Match_InvalidCLUIImm:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, 1, (1 << 5) - 1,
        "immediate must be in [0xfffe0, 0xfffff] or");
  case Match_InvalidUImm7Lsb00:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, 0, (1 << 7) - 4,
        "immediate must be a multiple of 4 bytes in the range");
  case Match_InvalidUImm8Lsb00:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, 0, (1 << 8) - 4,
        "immediate must be a multiple of 4 bytes in the range");
  case Match_InvalidUImm8Lsb000:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, 0, (1 << 8) - 8,
        "immediate must be a multiple of 8 bytes in the range");
  case Match_InvalidSImm9Lsb0:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, -(1 << 8), (1 << 8) - 2,
        "immediate must be a multiple of 2 bytes in the range");
  case Match_InvalidUImm9Lsb000:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, 0, (1 << 9) - 8,
        "immediate must be a multiple of 8 bytes in the range");
  case Match_InvalidUImm10Lsb00NonZero:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, 4, (1 << 10) - 4,
        "immediate must be a multiple of 4 bytes in the range");
  case Match_InvalidSImm10Lsb0000NonZero:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, -(1 << 9), (1 << 9) - 16,
        "immediate must be a multiple of 16 bytes and non-zero in the range");
  case Match_InvalidSImm12:
    return generateImmOutOfRangeError(Operands, ErrorInfo, -(1 << 11),
                                      (1 << 11) - 1);
  case Match_InvalidSImm12Lsb0:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, -(1 << 11), (1 << 11) - 2,
        "immediate must be a multiple of 2 bytes in the range");
  case Match_InvalidUImm12:
    return generateImmOutOfRangeError(Operands, ErrorInfo, 0, (1 << 12) - 1);
  case Match_InvalidSImm13Lsb0:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, -(1 << 12), (1 << 12) - 2,
        "immediate must be a multiple of 2 bytes in the range");
  case Match_InvalidUImm20:
    return generateImmOutOfRangeError(Operands, ErrorInfo, 0, (1 << 20) - 1);
  case Match_InvalidSImm21Lsb0:
    return generateImmOutOfRangeError(
        Operands, ErrorInfo, -(1 << 20), (1 << 20) - 2,
        "immediate must be a multiple of 2 bytes in the range");
  case Match_InvalidFenceArg: {
    SMLoc ErrorLoc = ((RX600Operand &)*Operands[ErrorInfo]).getStartLoc();
    return Error(
        ErrorLoc,
        "operand must be formed of letters selected in-order from 'iorw'");
  }
  case Match_InvalidFRMArg: {
    SMLoc ErrorLoc = ((RX600Operand &)*Operands[ErrorInfo]).getStartLoc();
    return Error(
        ErrorLoc,
        "operand must be a valid floating point rounding mode mnemonic");
  }
  case Match_InvalidBareSymbol: {
    SMLoc ErrorLoc = ((RX600Operand &)*Operands[ErrorInfo]).getStartLoc();
    return Error(ErrorLoc, "operand must be a bare symbol name");
  }
  }

  llvm_unreachable("Unknown match type detected!");
}

bool RX600AsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                   SMLoc &EndLoc) {
  const AsmToken &Tok = getParser().getTok();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();
  RegNo = 0;
  StringRef Name = getLexer().getTok().getIdentifier();

  if (!MatchRegisterName(Name) || !MatchRegisterAltName(Name)) {
    getParser().Lex(); // Eat identifier token.
    return false;
  }

  return Error(StartLoc, "invalid register name");
}

OperandMatchResultTy RX600AsmParser::parseRegister(OperandVector &Operands,
                                                   bool AllowParens) {
  SMLoc FirstS = getLoc();
  bool HadParens = false;
  AsmToken Buf[2];

  // If this a parenthesised register name is allowed, parse it atomically
  if (AllowParens && getLexer().is(AsmToken::LParen)) {
    size_t ReadCount = getLexer().peekTokens(Buf);
    if (ReadCount == 2 && Buf[1].getKind() == AsmToken::RParen) {
      HadParens = true;
      getParser().Lex(); // Eat '('
    }
  }

  switch (getLexer().getKind()) {
  default:
    return MatchOperand_NoMatch;
  case AsmToken::Identifier:
    StringRef Name = getLexer().getTok().getIdentifier();
    unsigned RegNo = MatchRegisterName(Name);
    if (RegNo == 0) {
      RegNo = MatchRegisterAltName(Name);
      if (RegNo == 0) {
        if (HadParens)
          getLexer().UnLex(Buf[0]);
        return MatchOperand_NoMatch;
      }
    }
    if (HadParens)
      Operands.push_back(RX600Operand::createToken("(", FirstS, isRX600()));
    SMLoc S = getLoc();
    SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);
    getLexer().Lex();
    Operands.push_back(RX600Operand::createReg(RegNo, S, E, isRX600()));
  }

  if (HadParens) {
    getParser().Lex(); // Eat ')'
    Operands.push_back(RX600Operand::createToken(")", getLoc(), isRX600()));
  }

  return MatchOperand_Success;
}

OperandMatchResultTy RX600AsmParser::parseImmediate(OperandVector &Operands) {
  SMLoc S = getLoc();
  SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);
  const MCExpr *Res;

  switch (getLexer().getKind()) {
  default:
    return MatchOperand_NoMatch;
  case AsmToken::LParen:
  case AsmToken::Minus:
  case AsmToken::Plus:
  case AsmToken::Integer:
  case AsmToken::String:
    if (getParser().parseExpression(Res))
      return MatchOperand_ParseFail;
    break;
  case AsmToken::Identifier: {
    StringRef Identifier;
    if (getParser().parseIdentifier(Identifier))
      return MatchOperand_ParseFail;
    MCSymbol *Sym = getContext().getOrCreateSymbol(Identifier);
    Res = MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, getContext());
    break;
  }
  case AsmToken::Percent:
    return parseOperandWithModifier(Operands);
  }

  Operands.push_back(RX600Operand::createImm(Res, S, E, isRX600()));
  return MatchOperand_Success;
}

OperandMatchResultTy
RX600AsmParser::parseOperandWithModifier(OperandVector &Operands) {
  SMLoc S = getLoc();
  SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);

  if (getLexer().getKind() != AsmToken::Percent) {
    Error(getLoc(), "expected '%' for operand modifier");
    return MatchOperand_ParseFail;
  }

  getParser().Lex(); // Eat '%'

  if (getLexer().getKind() != AsmToken::Identifier) {
    Error(getLoc(), "expected valid identifier for operand modifier");
    return MatchOperand_ParseFail;
  }
  StringRef Identifier = getParser().getTok().getIdentifier();
  RX600MCExpr::VariantKind VK = RX600MCExpr::getVariantKindForName(Identifier);
  if (VK == RX600MCExpr::VK_RX600_Invalid) {
    Error(getLoc(), "unrecognized operand modifier");
    return MatchOperand_ParseFail;
  }

  getParser().Lex(); // Eat the identifier
  if (getLexer().getKind() != AsmToken::LParen) {
    Error(getLoc(), "expected '('");
    return MatchOperand_ParseFail;
  }
  getParser().Lex(); // Eat '('

  const MCExpr *SubExpr;
  if (getParser().parseParenExpression(SubExpr, E)) {
    return MatchOperand_ParseFail;
  }

  const MCExpr *ModExpr = RX600MCExpr::create(SubExpr, VK, getContext());
  Operands.push_back(RX600Operand::createImm(ModExpr, S, E, isRX600()));
  return MatchOperand_Success;
}

OperandMatchResultTy
RX600AsmParser::parseMemOpBaseReg(OperandVector &Operands) {
  if (getLexer().isNot(AsmToken::LParen)) {
    Error(getLoc(), "expected '('");
    return MatchOperand_ParseFail;
  }

  getParser().Lex(); // Eat '('
  Operands.push_back(RX600Operand::createToken("(", getLoc(), isRX600()));

  if (parseRegister(Operands) != MatchOperand_Success) {
    Error(getLoc(), "expected register");
    return MatchOperand_ParseFail;
  }

  if (getLexer().isNot(AsmToken::RParen)) {
    Error(getLoc(), "expected ')'");
    return MatchOperand_ParseFail;
  }

  getParser().Lex(); // Eat ')'
  Operands.push_back(RX600Operand::createToken(")", getLoc(), isRX600()));

  return MatchOperand_Success;
}

/// Looks at a token type and creates the relevant operand from this
/// information, adding to Operands. If operand was parsed, returns false, else
/// true. If ForceImmediate is true, no attempt will be made to parse the
/// operand as a register, which is needed for pseudoinstructions such as
/// call.
bool RX600AsmParser::parseOperand(OperandVector &Operands,
                                  bool ForceImmediate) {
  // Attempt to parse token as register, unless ForceImmediate.
  if (!ForceImmediate && parseRegister(Operands, true) == MatchOperand_Success)
    return false;

  // Attempt to parse token as an immediate
  if (parseImmediate(Operands) == MatchOperand_Success) {
    // Parse memory base register if present
    if (getLexer().is(AsmToken::LParen))
      return parseMemOpBaseReg(Operands) != MatchOperand_Success;
    return false;
  }

  // Finally we have exhausted all options and must declare defeat.
  Error(getLoc(), "unknown operand");
  return true;
}

/// Return true if the operand at the OperandIdx for opcode Name should be
/// 'forced' to be parsed as an immediate. This is required for
/// pseudoinstructions such as tail or call, which allow bare symbols to be used
/// that could clash with register names.
static bool shouldForceImediateOperand(StringRef Name, unsigned OperandIdx) {
  // FIXME: This may not scale so perhaps we want to use a data-driven approach
  // instead.
  switch (OperandIdx) {
  case 0:
    // call imm
    // tail imm
    return Name == "tail" || Name == "call";
  case 1:
    // lla rdest, imm
    return Name == "lla";
  default:
    return false;
  }
}

bool RX600AsmParser::ParseInstruction(ParseInstructionInfo &Info,
                                      StringRef Name, SMLoc NameLoc,
                                      OperandVector &Operands) {
  // First operand is token for instruction
  Operands.push_back(RX600Operand::createToken(Name, NameLoc, isRX600()));

  // If there are no more operands, then finish
  if (getLexer().is(AsmToken::EndOfStatement))
    return false;

  // Parse first operand
  if (parseOperand(Operands, shouldForceImediateOperand(Name, 0)))
    return true;

  // Parse until end of statement, consuming commas between operands
  unsigned OperandIdx = 1;
  while (getLexer().is(AsmToken::Comma)) {
    // Consume comma token
    getLexer().Lex();

    // Parse next operand
    if (parseOperand(Operands, shouldForceImediateOperand(Name, OperandIdx)))
      return true;

    ++OperandIdx;
  }

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    SMLoc Loc = getLexer().getLoc();
    getParser().eatToEndOfStatement();
    return Error(Loc, "unexpected token");
  }

  getParser().Lex(); // Consume the EndOfStatement.
  return false;
}

bool RX600AsmParser::classifySymbolRef(const MCExpr *Expr,
                                       RX600MCExpr::VariantKind &Kind,
                                       int64_t &Addend) {
  Kind = RX600MCExpr::VK_RX600_None;
  Addend = 0;

  if (const RX600MCExpr *RE = dyn_cast<RX600MCExpr>(Expr)) {
    Kind = RE->getKind();
    Expr = RE->getSubExpr();
  }

  // It's a simple symbol reference or constant with no addend.
  if (isa<MCConstantExpr>(Expr) || isa<MCSymbolRefExpr>(Expr))
    return true;

  const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr);
  if (!BE)
    return false;

  if (!isa<MCSymbolRefExpr>(BE->getLHS()))
    return false;

  if (BE->getOpcode() != MCBinaryExpr::Add &&
      BE->getOpcode() != MCBinaryExpr::Sub)
    return false;

  // We are able to support the subtraction of two symbol references
  if (BE->getOpcode() == MCBinaryExpr::Sub &&
      isa<MCSymbolRefExpr>(BE->getRHS()))
    return true;

  // See if the addend is a constant, otherwise there's more going
  // on here than we can deal with.
  auto AddendExpr = dyn_cast<MCConstantExpr>(BE->getRHS());
  if (!AddendExpr)
    return false;

  Addend = AddendExpr->getValue();
  if (BE->getOpcode() == MCBinaryExpr::Sub)
    Addend = -Addend;

  // It's some symbol reference + a constant addend
  return Kind != RX600MCExpr::VK_RX600_Invalid;
}

bool RX600AsmParser::ParseDirective(AsmToken DirectiveID) {
  // This returns false if this function recognizes the directive
  // regardless of whether it is successfully handles or reports an
  // error. Otherwise it returns true to give the generic parser a
  // chance at recognizing it.
  StringRef IDVal = DirectiveID.getString();

  if (IDVal == ".option")
    return parseDirectiveOption();

  return true;
}

bool RX600AsmParser::parseDirectiveOption() {
  MCAsmParser &Parser = getParser();
  // Get the option token.
  AsmToken Tok = Parser.getTok();
  // At the moment only identifiers are supported.
  if (Tok.isNot(AsmToken::Identifier))
    return Error(Parser.getTok().getLoc(),
                 "unexpected token, expected identifier");

  StringRef Option = Tok.getIdentifier();

  if (Option == "rvc") {
    getTargetStreamer().emitDirectiveOptionRVC();

    Parser.Lex();
    if (Parser.getTok().isNot(AsmToken::EndOfStatement))
      return Error(Parser.getTok().getLoc(),
                   "unexpected token, expected end of statement");

    setFeatureBits(RX600::FeatureStdExtC, "c");
    return false;
  }

  if (Option == "norvc") {
    getTargetStreamer().emitDirectiveOptionNoRVC();

    Parser.Lex();
    if (Parser.getTok().isNot(AsmToken::EndOfStatement))
      return Error(Parser.getTok().getLoc(),
                   "unexpected token, expected end of statement");

    clearFeatureBits(RX600::FeatureStdExtC, "c");
    return false;
  }

  // Unknown option.
  Warning(Parser.getTok().getLoc(),
          "unknown option, expected 'rvc' or 'norvc'");
  Parser.eatToEndOfStatement();
  return false;
}

void RX600AsmParser::emitToStreamer(MCStreamer &S, const MCInst &Inst) {
  MCInst CInst;
  bool Res = compressInst(CInst, Inst, getSTI(), S.getContext());
  CInst.setLoc(Inst.getLoc());
  S.EmitInstruction((Res ? CInst : Inst), getSTI());
}

void RX600AsmParser::emitLoadImm(unsigned DestReg, int64_t Value,
                                 MCStreamer &Out) {
  if (isInt<32>(Value)) {
    // Emits the MC instructions for loading a 32-bit constant into a register.
    //
    // Depending on the active bits in the immediate Value v, the following
    // instruction sequences are emitted:
    //
    // v == 0                        : ADDI(W)
    // v[0,12) != 0 && v[12,32) == 0 : ADDI(W)
    // v[0,12) == 0 && v[12,32) != 0 : LUI
    // v[0,32) != 0                  : LUI+ADDI(W)
    //
    int64_t Hi20 = ((Value + 0x800) >> 12) & 0xFFFFF;
    int64_t Lo12 = SignExtend64<12>(Value);
    unsigned SrcReg = RX600::X0;

    if (Hi20) {
      emitToStreamer(Out,
                     MCInstBuilder(RX600::LUI).addReg(DestReg).addImm(Hi20));
      SrcReg = DestReg;
    }

    if (Lo12 || Hi20 == 0) {
      unsigned AddiOpcode =
          STI->hasFeature(RX600::Feature64Bit) ? RX600::ADDIW : RX600::ADDI;
      emitToStreamer(Out, MCInstBuilder(AddiOpcode)
                              .addReg(DestReg)
                              .addReg(SrcReg)
                              .addImm(Lo12));
    }
    return;
  }
  assert(STI->hasFeature(RX600::Feature64Bit) &&
         "Target must be 64-bit to support a >32-bit constant");

  // In the worst case, for a full 64-bit constant, a sequence of 8 instructions
  // (i.e., LUI+ADDIW+SLLI+ADDI+SLLI+ADDI+SLLI+ADDI) has to be emmitted. Note
  // that the first two instructions (LUI+ADDIW) can contribute up to 32 bits
  // while the following ADDI instructions contribute up to 12 bits each.
  //
  // On the first glance, implementing this seems to be possible by simply
  // emitting the most significant 32 bits (LUI+ADDIW) followed by as many left
  // shift (SLLI) and immediate additions (ADDI) as needed. However, due to the
  // fact that ADDI performs a sign extended addition, doing it like that would
  // only be possible when at most 11 bits of the ADDI instructions are used.
  // Using all 12 bits of the ADDI instructions, like done by GAS, actually
  // requires that the constant is processed starting with the least significant
  // bit.
  //
  // In the following, constants are processed from LSB to MSB but instruction
  // emission is performed from MSB to LSB by recursively calling
  // emitLoadImm. In each recursion, first the lowest 12 bits are removed
  // from the constant and the optimal shift amount, which can be greater than
  // 12 bits if the constant is sparse, is determined. Then, the shifted
  // remaining constant is processed recursively and gets emitted as soon as it
  // fits into 32 bits. The emission of the shifts and additions is subsequently
  // performed when the recursion returns.
  //
  int64_t Lo12 = SignExtend64<12>(Value);
  int64_t Hi52 = (Value + 0x800) >> 12;
  int ShiftAmount = 12 + findFirstSet((uint64_t)Hi52);
  Hi52 = SignExtend64(Hi52 >> (ShiftAmount - 12), 64 - ShiftAmount);

  emitLoadImm(DestReg, Hi52, Out);

  emitToStreamer(Out, MCInstBuilder(RX600::SLLI)
                          .addReg(DestReg)
                          .addReg(DestReg)
                          .addImm(ShiftAmount));

  if (Lo12)
    emitToStreamer(Out, MCInstBuilder(RX600::ADDI)
                            .addReg(DestReg)
                            .addReg(DestReg)
                            .addImm(Lo12));
}

void RX600AsmParser::emitLoadLocalAddress(MCInst &Inst, SMLoc IDLoc,
                                          MCStreamer &Out) {
  // The local load address pseudo-instruction "lla" is used in PC-relative
  // addressing of symbols:
  //   lla rdest, symbol
  // expands to
  //   TmpLabel: AUIPC rdest, %pcrel_hi(symbol)
  //             ADDI rdest, %pcrel_lo(TmpLabel)
  MCContext &Ctx = getContext();

  MCSymbol *TmpLabel = Ctx.createTempSymbol(
      "pcrel_hi", /* AlwaysAddSuffix */ true, /* CanBeUnnamed */ false);
  Out.EmitLabel(TmpLabel);

  MCOperand DestReg = Inst.getOperand(0);
  const RX600MCExpr *Symbol = RX600MCExpr::create(
      Inst.getOperand(1).getExpr(), RX600MCExpr::VK_RX600_PCREL_HI, Ctx);

  emitToStreamer(
      Out, MCInstBuilder(RX600::AUIPC).addOperand(DestReg).addExpr(Symbol));

  const MCExpr *RefToLinkTmpLabel =
      RX600MCExpr::create(MCSymbolRefExpr::create(TmpLabel, Ctx),
                          RX600MCExpr::VK_RX600_PCREL_LO, Ctx);

  emitToStreamer(Out, MCInstBuilder(RX600::ADDI)
                          .addOperand(DestReg)
                          .addOperand(DestReg)
                          .addExpr(RefToLinkTmpLabel));
}

bool RX600AsmParser::processInstruction(MCInst &Inst, SMLoc IDLoc,
                                        MCStreamer &Out) {
  Inst.setLoc(IDLoc);

  if (Inst.getOpcode() == RX600::PseudoLI) {
    auto Reg = Inst.getOperand(0).getReg();
    int64_t Imm = Inst.getOperand(1).getImm();
    // On RX600 the immediate here can either be a signed or an unsigned
    // 32-bit number. Sign extension has to be performed to ensure that Imm
    // represents the expected signed 64-bit number.
    if (!isRX600())
      Imm = SignExtend64<32>(Imm);
    emitLoadImm(Reg, Imm, Out);
    return false;
  } else if (Inst.getOpcode() == RX600::PseudoLLA) {
    emitLoadLocalAddress(Inst, IDLoc, Out);
    return false;
  }

  emitToStreamer(Out, Inst);
  return false;
}

extern "C" void LLVMInitializeRX600AsmParser() {
  RegisterMCAsmParser<RX600AsmParser> X(getTheRX600Target());
}
