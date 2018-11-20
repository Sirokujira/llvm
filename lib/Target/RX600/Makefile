##===- lib/Target/RX600/Makefile ----------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMRX600CodeGen
TARGET = RX600

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = RX600GenRegisterInfo.inc RX600GenInstrInfo.inc \
                RX600GenAsmWriter.inc RX600GenCodeEmitter.inc \
                RX600GenDAGISel.inc RX600GenCallingConv.inc \
                RX600GenSubtargetInfo.inc RX600GenMCCodeEmitter.inc \
                RX600GenEDInfo.inc RX600GenDisassemblerTables.inc

DIRS = InstPrinter Disassembler TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common

