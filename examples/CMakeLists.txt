project(MSLFSSProjectExamples)

file(GLOB MSL_FSS_EXAMPLES "*.cpp")

foreach(exe_target ${MSL_FSS_EXAMPLES})
    get_filename_component(exe_name ${exe_target} NAME_WE)
    add_executable(${exe_name} ${exe_target})
    target_link_libraries(${exe_name} PRIVATE msl_fss)
    set_target_properties(${exe_name} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
endforeach()