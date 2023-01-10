//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_MATCHES_STORAGE_HPP
#define BOOST_URL_MATCHES_STORAGE_HPP

#include <boost/url/detail/config.hpp>
#include "detail/matches_storage.hpp"

namespace boost {
namespace urls {

/// A range type with the match results
template <std::size_t N = 20>
class matches_storage
    : public detail::matches_base
{
    string_view matches_storage_[N];
    string_view ids_storage_[N];

    template <class T>
    friend class router;

    matches_storage(
        string_view matches[N],
        string_view ids[N],
        std::size_t n)
        : matches_base(n)
    {
        for (std::size_t i = 0; i < n; ++i)
        {
            matches_storage_[i] = matches[i];
            ids_storage_[i] = ids[i];
        }
    }

protected:
    virtual
    string_view const*
    matches() const override
    {
        return matches_storage_;
    }

    virtual
    string_view const*
    ids() const override
    {
        return ids_storage_;
    }

public:
    matches_storage() = default;
};

/// Default type for storing route match results
using matches = matches_storage<20>;

} // urls
} // boost

#endif

