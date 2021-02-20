//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_STORAGE_HPP
#define BOOST_URL_DETAIL_STORAGE_HPP

#include <boost/url/config.hpp>
#include <memory>

namespace boost {
namespace urls {
namespace detail {

struct storage
{
    virtual char* allocate( size_t ) = 0;
    virtual void deallocate( char*, size_t ) = 0;
};

template<class Allocator>
class alloc_storage
    : public storage
{
    Allocator a_;

    using traits =
        std::allocator_traits<
            Allocator>;

public:
    explicit
    alloc_storage(
        Allocator const& a)
        : a_(a)
    {
    }

    char* allocate( size_t s ) final override
    {
        return a_.allocate(s);
    }

    void deallocate( char* p, size_t s ) final override
    {
        a_.deallocate(p, s);
    }
};

template<class Allocator>
struct storage_member
{
    alloc_storage<Allocator> st_;

    explicit
    storage_member(
        Allocator const& alloc)
        : st_(alloc)
    {
    }
};

} // detail
} // urls
} // boost

#endif
