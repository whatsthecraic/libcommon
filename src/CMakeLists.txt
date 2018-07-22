include(CheckCXXCompilerFlag)

# Create a new library
add_library(common EXCLUDE_FROM_ALL
    backtrace.cpp backtrace.hpp
    database.cpp database.hpp
    error.cpp error.hpp
    optimisation.hpp
    profiler.cpp profiler.hpp details/profiler.hpp
    quantity.cpp quantity.hpp
    timer.cpp timer.hpp
)

# PAPI, for profiling
find_package(PAPI REQUIRED)
target_include_directories(common PRIVATE ${PAPI_INCLUDE_DIRS})
target_link_libraries(common PRIVATE ${PAPI_LIBRARIES})

# Support for backtrace
target_link_libraries(common PRIVATE libsqlite3 libbacktrace pthread)
target_compile_options(common PRIVATE -Wall -Werror)

if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (NOT CMAKE_BUILD_TYPE))
    CHECK_CXX_COMPILER_FLAG("-fno-limit-debug-info" no_limit_debug_info)
    if(no_limit_debug_info)
        target_compile_options(common PRIVATE "-fno-limit-debug-info")
    endif()
    CHECK_CXX_COMPILER_FLAG("-fno-omit-frame-pointer" no_omit_frame_pointer)
    if(no_omit_frame_pointer)
        target_compile_options(common PRIVATE "-fno-omit-frame-pointer")
    endif()
endif()
