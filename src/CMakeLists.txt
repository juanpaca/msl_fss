project(MSLFSSProjectSources)

# Standalone dummy library. In the real product this would link against
# msl_mfem / msl_mhm / msl_cg / msl_core; the dummy implementation is
# self-contained so that the module structure can be validated without the
# heavy MSL/MFEM dependencies (see MSLFSSProjectOptions.cmake).
file(GLOB MSL_FSS_SRCS "*.cpp" "*/*.cpp")

add_library(msl_fss ${MSL_FSS_SRCS})
target_include_directories(msl_fss PUBLIC ../include)
set_target_properties(msl_fss PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)