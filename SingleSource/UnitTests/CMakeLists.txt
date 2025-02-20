
include(CheckCCompilerFlag)

add_subdirectory(C++11)
add_subdirectory(SignlessTypes)
if(NOT ARCH STREQUAL "MOS")
  add_subdirectory(Threads)
  add_subdirectory(Vector)
  add_subdirectory(Vectorizer)
endif()
# FIXME: Disable SJLJ tests for now, until EH edges are represented.
# add_subdirectory(SetjmpLongjmp)
if(TARGET_OS STREQUAL "Darwin")
  add_subdirectory(ObjC)
  add_subdirectory(ObjC++)
endif()

# Fortran
if(TEST_SUITE_FORTRAN)
  add_subdirectory(Fortran)
endif()

file(GLOB Source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c *.cpp)
if(TARGET_OS STREQUAL "AIX" AND ARCH STREQUAL "PowerPC" AND NOT PPC_IS_PPC64_ENABLED) 
  list(REMOVE_ITEM Source AtomicOps.c)
endif()
if(TARGET_OS STREQUAL "Darwin")
  list(REMOVE_ITEM Source 2007-04-25-weak.c)
  if(ARCH STREQUAL "PowerPC")
    list(REMOVE_ITEM Source AtomicOps.c)
  endif()
endif()
if(ARCH STREQUAL "AArch64")
  list(REMOVE_ITEM Source blockstret.c)
endif()
if(ARCH STREQUAL "Mips")
  add_subdirectory(Mips)
  # Only MIPS 64-bit supports 8 bytes atomic operations.
  if(NOT MIPS_IS_MIPS64_ENABLED)
    list(REMOVE_ITEM Source AtomicOps.c)
  endif()
endif()
if(ARCH STREQUAL "XCore")
  list(REMOVE_ITEM Source AtomicOps.c)
  list(REMOVE_ITEM Source initp1.cpp)
endif()
if(NOT ARCH STREQUAL "x86")
  list(REMOVE_ITEM Source
    ms_struct-bitfield.c
    ms_struct-bitfield-1.c
    ms_struct-bitfield-init.c
    ms_struct-bitfield-init-1.c
    ms_struct_pack_layout.c
    ms_struct_pack_layout-1.c
  )
endif()
if(ARCH STREQUAL "MOS")
  list(REMOVE_ITEM Source
    # Permanently disabled tests.
    # ---------------------------

    # Requires 32-bit int.
    2002-10-13-BadLoad.c
    2003-05-26-Shorts.c

    # Requires libc++
    matrix-types-spec.cpp

    # Requires <strings.h>
    2005-05-11-Popcount-ffs-fls.c

    # Requires atomic operations
    AtomicOps.c

    # Undefined behavior?!
    2020-01-06-coverage-009.c

    # Temporarily disabled tests.
    # ---------------------------

    # Requires floating point.
    2002-05-02-ArgumentTest.c
    2002-05-02-ManyArguments.c
    2002-10-12-StructureArgs.c
    2002-10-12-StructureArgsSimple.c
    2003-05-07-VarArgs.c
    2003-08-05-CastFPToUint.c
    2003-08-11-VaListArg.c
    2004-02-02-NegativeZero.c
    2005-05-12-Int64ToFP.c
    2005-07-17-INT-To-FP.c
    2006-12-01-float_varg.c
    2007-01-04-KNR-Args.c
    2009-04-16-BitfieldInitialization.c
    2009-12-07-StructReturn.c
    FloatPrecision.c
    StructModifyTest.c
    byval-alignment.c

    # Requires malloc.
    2002-08-19-CodegenBug.c
    2003-07-09-LoadShorts.c
    DefaultInitDynArrays.cpp

    # Requires atexit.
    2003-05-14-AtExit.c

    # Requires scanf.
    2004-11-28-GlobalBoolLayout.c

    # Requires argc/argv.
    2003-05-31-LongShifts.c
    2006-01-29-SimpleIndirectCall.c
    printargs.c
    TestLoop.c

    # Requires static constructors.
    initp1.cpp

    # Requires vtables.
    member-function-pointers.cpp
  )
endif()

# Enable matrix types extension tests for compilers supporting -fenable-matrix.
check_c_compiler_flag(-fenable-matrix COMPILER_HAS_MATRIX_FLAG)
if (COMPILER_HAS_MATRIX_FLAG)
  set_property(SOURCE matrix-types-spec.cpp PROPERTY COMPILE_FLAGS -fenable-matrix)
else()
  list(REMOVE_ITEM Source matrix-types-spec.cpp)
endif()

llvm_singlesource()
