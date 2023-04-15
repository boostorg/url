#
# Copyright (c) 2023 alandefreitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#

# Files
source_group("" FILES fuzz_parse.cpp)

function(add_boost_url_fuzzer NAME SOURCE_FILES)
    # Fuzzer library
    add_library(fuzzerlib_${NAME} ${SOURCE_FILES})
    target_link_libraries(fuzzerlib_${NAME} PRIVATE Boost::url)
    set_property(TARGET fuzzerlib_${NAME} PROPERTY FOLDER "fuzzing")

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Fuzzer executable
        add_executable(fuzzer_${NAME} ${SOURCE_FILES})
        target_link_libraries(fuzzer_${NAME} PRIVATE Boost::url)
        target_compile_options(fuzzer_${NAME} PRIVATE -g -O2 -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=undefined)
        target_link_libraries(fuzzer_${NAME} PRIVATE -fsanitize=fuzzer -fuse-ld=lld)

        # Custom target to run fuzzer executable
        add_custom_target(fuzz_${NAME} fuzzer_${NAME} -rss_limit_mb=8192 -max_total_time=30 -timeout=30 DEPENDS fuzz_${NAME})
        set_property(TARGET fuzzer_${NAME} PROPERTY FOLDER "fuzzing")
    endif ()
endfunction()

add_boost_url_fuzzer(parse fuzz_parse.cpp)
