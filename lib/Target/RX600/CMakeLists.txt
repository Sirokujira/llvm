set(LLVM_TARGET_DEFINITIONS RX600.td)

tablegen(LLVM RX600GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM RX600GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM RX600GenCompressInstEmitter.inc -gen-compress-inst-emitter)
tablegen(LLVM RX600GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM RX600GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM RX600GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM RX600GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM RX600GenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM RX600GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM RX600GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(RX600CommonTableGen)

add_llvm_target(RX600CodeGen
  RX600AsmPrinter.cpp
  RX600FrameLowering.cpp
  RX600InstrInfo.cpp
  RX600ISelDAGToDAG.cpp
  RX600ISelLowering.cpp
  RX600MCInstLower.cpp
  RX600MergeBaseOffset.cpp
  RX600RegisterInfo.cpp
  RX600Subtarget.cpp
  RX600TargetMachine.cpp
  RX600TargetObjectFile.cpp
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
