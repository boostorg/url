#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
# Copyright (c) 2021 DMitry Arkhipov (grisumbras@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/url
#

if(NOT TARGET tests)
    add_custom_target(tests)
    set_property(TARGET tests PROPERTY FOLDER _deps)
endif()

set(BOOST_URL_TESTS_FILES
    CMakeLists.txt
    Jamfile
    test_rule.hpp
    authority_view.cpp
    error.cpp
    error_code.cpp
    grammar.cpp
    host_type.cpp
    ipv4_address.cpp
    ipv6_address.cpp
    params.cpp
    params_encoded.cpp
    params_encoded_view.cpp
    params_view.cpp
    pct_encoding.cpp
    pct_encoding_types.cpp
    query_param.cpp
    scheme.cpp
    segments.cpp
    segments_encoded.cpp
    segments_encoded_view.cpp
    segments_view.cpp
    static_pool.cpp
    static_url.cpp
    string.cpp
    string_view.cpp
    url.cpp
    url_view.cpp
    urls.cpp
    grammar/ascii.cpp
    grammar/charset.cpp
    grammar/error.cpp
    grammar/lut_chars.cpp
    grammar/parse.cpp
    grammar/range.cpp
    grammar/token_rule.cpp
    rfc/absolute_uri_rule.cpp
    rfc/authority_rule.cpp
    rfc/charsets.cpp
    rfc/fragment_rule.cpp
    rfc/hier_part_rule.cpp
    rfc/host_rule.cpp
    rfc/ip_literal_rule.cpp
    rfc/ipv_future_rule.cpp
    rfc/paths_rule.cpp
    rfc/pct_encoded_rule.cpp
    rfc/port_rule.cpp
    rfc/query_rule.cpp
    rfc/reg_name_rule.cpp
    rfc/relative_part_rule.cpp
    rfc/relative_ref_rule.cpp
    rfc/scheme_rule.cpp
    rfc/uri_rule.cpp
    rfc/uri_reference_rule.cpp
    rfc/userinfo_rule.cpp
    )

set(TEST_MAIN ../../extra/test_main.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${BOOST_URL_TESTS_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../extra PREFIX "_extra" FILES ${TEST_MAIN})
add_executable(boost_url_tests ${BOOST_URL_TESTS_FILES} ${TEST_MAIN})
target_include_directories(boost_url_tests PRIVATE . ../../extra/include)
target_link_libraries(boost_url_tests PRIVATE Boost::url)
add_test(NAME boost_url_tests COMMAND boost_url_tests)
add_dependencies(tests boost_url_tests)
