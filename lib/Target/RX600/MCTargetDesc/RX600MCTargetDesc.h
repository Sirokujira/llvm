//===-- RX600MCTargetDesc.h - RX600 Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides RX600 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLEMCTARGETDESC_H
#define SAMPLEMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"
#include "llvm/Support/Debug.h"

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class raw_ostream;

extern Target TheRX600Target;

MCCodeEmitter *createRX600MCCodeEmitter(const MCInstrInfo &MCII,
                                         const MCRegisterInfo &MRI,
                                         const MCSubtargetInfo &STI,
                                         MCContext &Ctx);

MCAsmBackend *createRX600AsmBackend(const Target &T, StringRef TT, StringRef CPU);

MCObjectWriter *createRX600ELFObjectWriter(raw_ostream &OS,
                                            uint8_t OSABI);
} // End llvm namespace

// Defines symbolic names for RX600 registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "RX600GenRegisterInfo.inc"

// Defines symbolic names for the RX600 instructions.
#define GET_INSTRINFO_ENUM
#include "RX600GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "RX600GenSubtargetInfo.inc"

#endif
