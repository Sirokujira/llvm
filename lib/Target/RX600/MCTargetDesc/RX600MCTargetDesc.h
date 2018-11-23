//===-- RX600MCTargetDesc.h - RX600 Target Descriptions ---------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_RX600_MCTARGETDESC_RX600MCTARGETDESC_H
#define LLVM_LIB_TARGET_RX600_MCTARGETDESC_RX600MCTARGETDESC_H

#include "llvm/Config/config.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"
#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class Triple;
class raw_ostream;
class raw_pwrite_stream;

Target &getTheRX600Target();

MCCodeEmitter *createRX600MCCodeEmitter(const MCInstrInfo &MCII,
                                        const MCRegisterInfo &MRI,
                                        MCContext &Ctx);

MCAsmBackend *createRX600AsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createRX600ELFObjectWriter(uint8_t OSABI,
                                                                 bool Is64Bit);
}

// Defines symbolic names for RX600 registers.
#define GET_REGINFO_ENUM
#include "RX600GenRegisterInfo.inc"

// Defines symbolic names for RX600 instructions.
#define GET_INSTRINFO_ENUM
#include "RX600GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "RX600GenSubtargetInfo.inc"

#endif
