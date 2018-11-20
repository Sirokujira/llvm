//===-- RX600InstPrinter.cpp - Convert RX600 MCInst to assembly syntax ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an RX600 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "RX600InstPrinter.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#include "RX600GenAsmWriter.inc"

void RX600InstPrinter::
printRegName(raw_ostream &OS, unsigned RegNo) const {
  OS << '$' << StringRef(getRegisterName(RegNo)).lower();
}

void RX600InstPrinter::
printInst(const MCInst *MI, raw_ostream &O, StringRef Annot) {
  DEBUG(dbgs() << ">>> printInst:"; MI->dump());
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

void RX600InstPrinter::
printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
  DEBUG(dbgs() << ">>> printOperand:" << *MI << " OpNo:" << OpNo << "\n");
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
  } else if (Op.isImm()) {
    O << Op.getImm();
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    O << *Op.getExpr();
  }
}

void RX600InstPrinter::
printMemOperand(const MCInst *MI, int opNum, raw_ostream &O) {
  DEBUG(dbgs() << ">>> printMemOperand:"; MI->dump());
  printOperand(MI, opNum+1, O);
  O << "(";
  printOperand(MI, opNum, O);
  O << ")";
}
