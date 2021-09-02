//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_CHAR_SET_HPP
#define BOOST_URL_CHAR_SET_HPP

#include <boost/url/detail/config.hpp>

namespace boost {
namespace urls {
namespace bnf {

template<class Derived>
class char_set
{
public:
    bool
    operator()(char c) const noexcept
    {
        return static_cast<Derived
            const*>(this)->contains(c);
    }

    template<class Char>
    Char*
    skip(
        Char* first,
        char const* const end)
            const noexcept
    {
        auto const& self =
            *static_cast<Derived
                const*>(this);
        while(first != end)
        {
            if(! self.contains(
                    *first))
                break;
            ++first;
        }
        return first;
    }
};

/** A table-driven character set
*/
class char_set_table
    : public char_set<char_set_table>
{
    char const* const tab_;

public:
    char_set_table(
        char const* tab) noexcept
        : tab_(tab)
    {
    }

    bool
    contains(
        char c) const noexcept
    {
        auto const u = static_cast<
            unsigned char>(c);
        return tab_[u] != 0;
    }
};

//------------------------------------------------

template<bool (*F)(char)>
class char_set_function
    : public char_set<char_set_function<F>>
{
public:
    bool
    contains(
        char c) const noexcept
    {
        auto const u = static_cast<
            unsigned char>(c);
        return F(u) != 0;
    }
};

} // bnf
} // urls
} // boost

#endif
