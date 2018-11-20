//===-- RX600MCAsmInfo.cpp - RX600 asm properties -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the RX600MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "RX600MCAsmInfo.h"
#include "llvm/ADT/StringRef.h"
using namespace llvm;

RX600MCAsmInfo::RX600MCAsmInfo(const Target &T, StringRef TT) {
  PointerSize = 4;

  PrivateGlobalPrefix = ".L";
  //WeakRefDirective ="\t.weak\t";
  PCSymbol=".";
  CommentString = ";";

  AlignmentIsInBytes = false;
  AllowNameToStartWithDigit = true;
  UsesELFSectionDirectiveForBSS = true;
}
