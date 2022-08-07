//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/http_proto
//

#ifndef BOOST_URL_GRAMMAR_DETAIL_COPIED_STRINGS_HPP
#define BOOST_URL_GRAMMAR_DETAIL_COPIED_STRINGS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <cstdlib>

namespace boost {
namespace urls {
namespace grammar {
namespace detail {

/** Common functionality for copied strings

    This base class is used by the library
    to provide the functionality for copied
    strings.
    Users should not use this class directly.
    Instead, construct an instance of
    @ref copied_strings instead.
*/
class copied_strings_base
{
public:
    /** Destructor

        Destruction invalidates any strings
        previously returned by this object.
    */
    BOOST_URL_DECL
    ~copied_strings_base();

    /** Return a string, or a copy if it overlaps the protected buffer

        This function checks if the passed string
        view overlaps the protected character buffer
        set on construction. If there is no overlap,
        the same view is returned. However if the
        character buffer would overlap, then a copy
        is returned instead.
    */
    BOOST_URL_DECL
    string_view
    maybe_copy(
        string_view s);

private:
    template<std::size_t>
    friend class copied_strings;

    struct dynamic_buf
    {
        dynamic_buf* next;
    };

    bool
    is_overlapping(
        string_view s) const noexcept;

    /** Constructor
    */
    BOOST_URL_DECL
    copied_strings_base(
        string_view s,
        char* local_buf,
        std::size_t local_size) noexcept;

    string_view s_;
    char* local_buf_;
    std::size_t local_remain_;
    dynamic_buf* dynamic_list_ = nullptr;
};

//------------------------------------------------

/** Helper to copy strings if they overlap a protected character buffer

    Objects of this type are declared on the
    stack as local variables in functions which
    accept @ref string_view parameters that
    modify an underlying character buffer. The
    purpose is to make a copy of the parameter
    if the parameter overlaps the protected
    character buffer.

    @par Example
    In this example we implement the append
    member function. The use of copied strings
    handles the case where the passed string `s`
    points into `s_`.
    @code
    struct container
    {
        std::string s_;

        void append( string_view s )
        {
            copied_strings<4096> cs( s_ );
            s = cs.maybe_copy( s );
            s_.append( s.data(), s.size() );
        }
    };
    @endcode

    @tparam BufferSize The number of bytes of
    inline storage. This is how many characters
    can be copied before dynamic allocation is
    required.
*/
template<
    std::size_t BufferSize>
class copied_strings
    : public copied_strings_base
{
    char buf_[BufferSize];

public:
    /** Constructor

        This constructs storage for copying
        strings that overlap the protected
        character buffer.

        @param s The character buffer to protect
    */
    explicit
    copied_strings(
        string_view s) noexcept
        : copied_strings_base(
            s, buf_, sizeof(buf_))
    {
    }
};

} // detail
} // grammar
} // urls
} // boost

#endif
