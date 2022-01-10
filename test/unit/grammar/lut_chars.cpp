//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/lut_chars.hpp>

#include "test_suite.hpp"
#include "test_rule.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct lut_chars_test
{
    void
    test_lut_chars()
    {
        // lut_chars(char const*)
        {
            constexpr lut_chars digits_ = "0123456789";

            (void)digits_;
        }

        // lut_chars(Pred)
        {
            struct is_digit_
            {
                constexpr bool
                operator()(char c ) const noexcept
                {
                    return c >= '0' && c <= '9';
                }
            };

            constexpr lut_chars digits_( is_digit_{} );

            (void)digits_;
        }

        // operator+
        {
            constexpr lut_chars alpha_chars_(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz");

            constexpr lut_chars alnum_chars_ = alpha_chars_ + "0123456789";
        }

        // operator-
        {
            constexpr lut_chars consonants = lut_chars("abcdefghijklmnopqrstuvwxyz") - "aeiou";

            (void)consonants;
        }

        // operator~
        {
            constexpr lut_chars not_vowels = ~lut_chars( "aAeEiIoOuU" );

            (void)not_vowels;
        }
    }

    void
    run()
    {
    }
};

TEST_SUITE(
    lut_chars_test,
    "boost.url.grammar.lut_chars");

} // grammar
} // urls
} // boost
