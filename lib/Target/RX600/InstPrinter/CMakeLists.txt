include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMRX600AsmPrinter
  RX600InstPrinter.cpp
  )

add_dependencies(LLVMRX600AsmPrinter RX600CommonTableGen)
