project(MSLFSSProjectTests)

include(FetchContent)

# Catch2 is fetched the same way the MSL sibling projects do.
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.10
)

FetchContent_MakeAvailable(Catch2)

file(GLOB MSL_FSS_TESTS "*_test.cpp")

add_executable(tests_fss ${MSL_FSS_TESTS} testmain.cpp)
target_include_directories(tests_fss PRIVATE ../../include)
target_link_libraries(tests_fss PRIVATE msl_fss Catch2::Catch2)
set_target_properties(tests_fss PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

add_test(NAME tests_fss COMMAND tests_fss)

if(RUN_TESTS)
    add_custom_command(TARGET tests_fss POST_BUILD
        COMMAND tests_fss
        COMMENT "Running msl_fss unit and integration tests")
endif(RUN_TESTS)