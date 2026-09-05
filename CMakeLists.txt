cmake_minimum_required(VERSION 3.16.0)

project(MSLFSSProject CXX)

include(CTest)

# Build option: whether the dependency graph mirrors the real MSL stack.
# The dummy implementation is standalone, so this option is accepted but the
# project does not require any external MSL library to compile. It is kept
# only to document the intended deployment-time coupling.
option(USE_MPI "Enable MPI cmake options" OFF)
option(USE_MUMPS "Enable MUMPS cmake options" OFF)
option(USE_OPENMP "Enable OpenMP compile options (not recommended with MSL-MFEM)" OFF)
option(RUN_TESTS "Run tests post-build" OFF)

# To ease external tooling that expects this header to exist.
set(CPACK_PACKAGE_NAME "msl_fss")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Directory with .cmake files with build variants, (post-)build options, etc.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MSLFSSProjectOptions)
list(REMOVE_AT CMAKE_MODULE_PATH -1)

add_subdirectory(src)
add_subdirectory(examples)

if(RUN_TESTS OR BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "MSL_FSS configured with: USE_MPI=${USE_MPI}, USE_MUMPS=${USE_MUMPS}, USE_OPENMP=${USE_OPENMP}")
