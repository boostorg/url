//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/pct_string_view.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

#if 0
//------------------------------------------------

struct any_string_buffer
{
    virtual ~any_string_buffer() = default;
    virtual char* prepare(std::size_t n) = 0;
};

//------------------------------------------------

struct string_token
    : any_string_buffer
{
    using return_type = std::string;

    return_type
    result() noexcept
    {
        return std::move(s_);
    }

private:
    char*
    prepare(std::size_t n) override
    {
        s_.resize(n);
        return &s_[0];
    }

    std::string s_;
};

//------------------------------------------------

struct assign_string_token
    : any_string_buffer
{
    using return_type = std::string&;

    assign_string_token(
        std::string& s) noexcept
        : s_(s)
    {
    }

    return_type
    result() noexcept
    {
        return s_;
    }

private:
    char*
    prepare(std::size_t n) override
    {
        s_.resize(n);
        return &s_[0];
    }

    std::string& s_;
};

assign_string_token
assign_to(std::string& s)
{
    return {s};
}

//------------------------------------------------

struct append_string_token
    : any_string_buffer
{
    using return_type = std::string&;

    append_string_token(
        std::string& s) noexcept
        : s_(s)
    {
    }

    return_type
    result() noexcept
    {
        return s_;
    }

private:
    char*
    prepare(std::size_t n) override
    {
        auto n0 = s_.size();
        if(n > s_.max_size() - n0)
            detail::throw_length_error(
                "append_string_token");
        s_.resize(n0 + n);
        return &s_[n0];
    }

    std::string& s_;
};

append_string_token
append_to(std::string& s)
{
    return {s};
}

//------------------------------------------------

void get_decoded_impl(
    any_string_buffer& dest)
{
    char* p = dest.prepare(3);
    p[0] = '%';
    p[1] = '2';
    p[2] = '5';
}

template<class StringToken = string_token>
auto
get_decoded(
    StringToken&& t = {}) ->
        typename StringToken::return_type
{
    get_decoded_impl(t);
    return t.result();
}
#endif

//------------------------------------------------

struct pct_string_view_test
{
    using S = pct_string_view;

    void
    testSpecial()
    {
        // pct_string_view()
        {
            BOOST_TEST(S() == "");
        }

        // pct_string_view(char const*)
        {
            BOOST_TEST(S("") == "");
            BOOST_TEST(S("x") == "x");
            BOOST_TEST(S("%25") == "%25");
        }

        // pct_string_view(char const*, std::size_t)
        {
            BOOST_TEST(S("", 0) == "");
            BOOST_TEST(S("x", 1) == "x");
            BOOST_TEST(S("%25", 3) == "%25");
        }

        // operator string_view
        {
            auto const f = [](string_view)
            {
            };
            f(S());
        }
    }

    void
    testRelation()
    {
        // ==
        BOOST_TEST(S("x") == S("x"));
        BOOST_TEST(S("x") == "x");
        BOOST_TEST("x" == S("x"));
        BOOST_TEST(S("x") == string_view("x"));
        BOOST_TEST(string_view("x") == S("x"));
        BOOST_TEST(std::string("x") == S("x"));
        BOOST_TEST(S("x") == std::string("x"));
    #if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        BOOST_TEST(S("x") == std::string_view("x"));
        BOOST_TEST(std::string_view("x") == S("x"));
    #endif

        // !=
        BOOST_TEST(S("x") != S("y"));
        BOOST_TEST(S("x") != "y");
        BOOST_TEST("x" != S("y"));
        BOOST_TEST(S("x") != string_view("y"));
        BOOST_TEST(string_view("x") != S("y"));
        BOOST_TEST(std::string("x") != S("y"));
        BOOST_TEST(S("x") != std::string("y"));
    #if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        BOOST_TEST(S("x") != std::string_view("y"));
        BOOST_TEST(std::string_view("x") != S("y"));
    #endif

        // <
        BOOST_TEST(S("x") < S("y"));
        BOOST_TEST(S("x") < "y");
        BOOST_TEST("x" < S("y"));
        BOOST_TEST(S("x") < string_view("y"));
        BOOST_TEST(string_view("x") < S("y"));
        BOOST_TEST(std::string("x") < S("y"));
        BOOST_TEST(S("x") < std::string("y"));
    #if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        BOOST_TEST(S("x") < std::string_view("y"));
        BOOST_TEST(std::string_view("x") < S("y"));
    #endif

        // <=
        BOOST_TEST(S("x") <= S("x"));
        BOOST_TEST(S("x") <= "x");
        BOOST_TEST("x" <= S("x"));
        BOOST_TEST(S("x") <= string_view("x"));
        BOOST_TEST(string_view("x") <= S("x"));
        BOOST_TEST(std::string("x") <= S("x"));
        BOOST_TEST(S("x") <= std::string("x"));
    #if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        BOOST_TEST(S("x") <= std::string_view("x"));
        BOOST_TEST(std::string_view("x") <= S("x"));
    #endif

        // >
        BOOST_TEST(S("y") > S("x"));
        BOOST_TEST(S("y") > "x");
        BOOST_TEST("y" > S("x"));
        BOOST_TEST(S("y") > string_view("x"));
        BOOST_TEST(string_view("y") > S("x"));
        BOOST_TEST(std::string("y") > S("x"));
        BOOST_TEST(S("y") > std::string("x"));
    #if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        BOOST_TEST(S("y") > std::string_view("x"));
        BOOST_TEST(std::string_view("y") > S("x"));
    #endif

        // >=
        BOOST_TEST(S("x") >= S("x"));
        BOOST_TEST(S("x") >= "x");
        BOOST_TEST("x" >= S("x"));
        BOOST_TEST(S("x") >= string_view("x"));
        BOOST_TEST(string_view("x") >= S("x"));
        BOOST_TEST(std::string("x") >= S("x"));
        BOOST_TEST(S("x") >= std::string("x"));
    #if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
        BOOST_TEST(S("x") >= std::string_view("x"));
        BOOST_TEST(std::string_view("x") >= S("x"));
    #endif

    }

    void
    run()
    {
        testSpecial();
        testRelation();
    }
};

TEST_SUITE(
    pct_string_view_test,
    "boost.url.pct_string_view");

} // urls
} // boost

/*

std::string     query()
pct_string_view encoded_query()
                set_query( string_view )
                set_encoded_query( pct_string_view )

1. u.set_query( u.query() )                     // works
2. u.set_query( u.encoded_query() )             // encodes the encoding
3. u.set_encoded_query( u.query() )             // sometimes works, sometimes throws
4. u.set_encoded_query( u.set_encoded_query() ) // works

*/