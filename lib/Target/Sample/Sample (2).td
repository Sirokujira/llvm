//===- Sample.td - Describe the Sample Target Machine ------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

include "SampleRegisterInfo.td"
include "SampleInstrInfo.td"
include "SampleCallingConv.td"

def SampleInstrInfo : InstrInfo;

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

def Sample : Target {
  let InstructionSet = SampleInstrInfo;
}
