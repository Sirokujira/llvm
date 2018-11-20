//===-- RX600AsmPrinter.cpp - RX600 LLVM assembly writer ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the RX600 assembly language.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "RX600.h"
#include "RX600InstrInfo.h"
#include "RX600MCInstLower.h"
#include "RX600TargetMachine.h"
#include "InstPrinter/RX600InstPrinter.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

namespace {
class RX600AsmPrinter : public AsmPrinter {
 public:
  RX600AsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
      : AsmPrinter(TM, Streamer) {}

  virtual const char *getPassName() const {
    return "RX600 Assembly Printer";
  }

  // should overwrite functions
  void EmitInstruction(const MachineInstr *MI) /*override*/;
};
} // end of anonymous namespace

void RX600AsmPrinter::
EmitInstruction(const MachineInstr *MI) {
  DEBUG(dbgs() << ">> RX600AsmPinter::EmitInstruction <<\n");
  DEBUG(MI->dump());
  RX600MCInstLower MCInstLowering(OutContext, *Mang, *this);
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  OutStreamer.EmitInstruction(TmpInst);
}

// Force static initialization.
extern "C" void LLVMInitializeRX600AsmPrinter() {
  RegisterAsmPrinter<RX600AsmPrinter> X(TheRX600Target);
}
