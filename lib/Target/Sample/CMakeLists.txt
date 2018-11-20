set(LLVM_TARGET_DEFINITIONS Sample.td)

tablegen(LLVM SampleGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM SampleGenCallingConv.inc -gen-callingconv)
tablegen(LLVM SampleGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM SampleGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM SampleGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM SampleGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM SampleGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(SampleCommonTableGen)

add_llvm_target(SampleCodeGen
  SampleAsmPrinter.cpp
  SampleBranchFinalize.cpp
  SampleExpandPseudos.cpp
  SampleFrameLowering.cpp
  SampleInstrInfo.cpp
  SampleISelDAGToDAG.cpp
  SampleISelLowering.cpp
  SampleMachineFunctionInfo.cpp
  SampleMCInstLower.cpp
  SampleRegisterInfo.cpp
  SampleSubtarget.cpp
  SampleTargetMachine.cpp
  )

add_subdirectory(Disassembler)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
