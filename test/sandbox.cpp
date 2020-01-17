//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#include <boost/url/config.hpp>
#include <boost/url/error.hpp>

#include "test_suite.hpp"

namespace boost {
namespace url {

namespace detail {

enum
{
    id_scheme = 0, // trailing ':'
    id_username,   // leading "//"
    id_password,   // leading ':', trailing '@'
    id_hostname,
    id_port,       // leading ':'
    id_path,
    id_query,      // leading '?'
    id_frag,        // leading '#'
    id_end         // one past the end
};

struct parts
{
    std::size_t offset[detail::id_end + 1];
    std::size_t nseg = 0;
    std::size_t nparam = 0;
    optional<unsigned short> port;

    parts()
    {
        std::fill(
            offset,
            offset + detail::id_end + 1, 0);
    }

    string_view
    get(int i, char const* s) const noexcept
    {
        return {
            s + offset[i],
            offset[i + 1] - offset[i] };
    }
};

} // detail

//----------------------------------------------------------

class view2
{
    //char const* s_;
    detail::parts pt_;
};

//----------------------------------------------------------

class basic_value
{
    char* s_ = nullptr;
    detail::parts pt_;

public:
    char const*
    c_str() const noexcept
    {
        if(! s_)
            return "";
        return s_;
    }

    std::size_t
    size() const noexcept
    {
        return pt_.offset[
            detail::id_end];
    }

    virtual
    std::size_t
    max_size() const noexcept = 0;

    virtual
    std::size_t
    capacity() const noexcept = 0;

private:
    virtual
    char*
    reserve(
        char* s,
        std::size_t n) = 0;
};

//----------------------------------------------------------

template<class Allocator>
class value2
{
    Allocator a_;
    std::size_t cap_ = 0;

public:
    std::size_t
    max_size() const noexcept
    {
        return a_.max_size();
    }

    std::size_t
    capacity() const noexcept
    {
        return cap_;
    }
};

//----------------------------------------------------------

template<std::size_t N>
class static_value
{
    char buf_[N];

public:
    std::size_t
    max_size() const noexcept
    {
        return N;
    }

    std::size_t
    capacity() const noexcept
    {
        return N;
    }

private:
    char*
    reserve(
        char* s,
        std::size_t n)
    {
        if(n > N)
            too_large::raise();
        return s;
    }
};

//----------------------------------------------------------

class sandbox_test
{
public:

    void
    doSandbox()
    {
        //detail::id_frag;
    }

    void
    run()
    {
        doSandbox();
    }
};

TEST_SUITE(sandbox_test, "boost.url.sandbox");

} // url
} // boost
