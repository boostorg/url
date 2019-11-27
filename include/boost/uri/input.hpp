//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/uri
//

#ifndef BOOST_URI_INPUT_HPP
#define BOOST_URI_INPUT_HPP

#include <boost/beast/core/string.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace beast {
namespace uri {

/** Represents an input character sequence.
*/
class input
{
    char const* it_ = nullptr;
    char const* end_ = nullptr;

public:
    /** Allows input sequence modifications to be rolled back
    */
    class guard;

    /// Constructor
    input() = default;

    /// Constructor
    input(input const&) = default;

    /// Assignment
    input& operator=(input const&) = default;

    /// Constructor
    explicit
    input(string_view s) noexcept
        : it_(s.data())
        , end_(s.data() + s.size())
    {
    }

    /// Returns an iterator to the end of the input sequence
    char const*
    end() const noexcept
    {
        return end_;
    }

    /// Returns `true` if no characters are remaining
    bool
    empty() const noexcept
    {
        return it_ >= end_;
    }

    /// Return the number of characters remaining
    std::size_t
    remain() const noexcept
    {
        return static_cast<std::size_t>(end_ - it_);
    }

    /// Return an iterator to the current character
    char const*
    get() const noexcept
    {
        return it_;
    }

    /// Return the current character
    char const&
    operator*() const noexcept
    {
        BOOST_ASSERT(it_ < end_);
        return *it_;
    }

    /// Return the character at a particular offset
    char const&
    operator[](int offset) const noexcept
    {
        BOOST_ASSERT(it_ + offset < end_);
        return it_[offset];
    }

    /// Advance the current character
    input&
    operator++() noexcept
    {
        BOOST_ASSERT(it_ < end_);
        ++it_;
        return *this;
    }

    /// Advance the current character
    input
    operator++(int) noexcept
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    input&
    operator+=(int n) noexcept
    {
        it_ += n;
        return *this;
    }
};

//------------------------------------------------------------------------------

class input::guard
{
    char const*& it_;
    char const* saved_;
    bool revert_ = true;

public:
    guard() = delete;
    guard(guard const&) = delete;
    guard& operator=(guard const&) = delete;

    /// Constructor
    explicit
    guard(input& in)
        : it_(in.it_)
        , saved_(in.it_)
    {
    }

    /// Constructor
    guard(guard&& other)
        : it_(other.it_)
        , saved_(other.saved_)
        , revert_(other.revert_)
    {
        other.revert_ = false;
    }

    /// Destructor
    ~guard()
    {
        if(revert_)
            it_ = saved_;
    }

    /// Returns an iterator to the beginning of the guarded sequence
    char const*
    begin() const noexcept
    {
        return saved_;
    }

    /// Commit to the current input iterator position
    void
    commit()
    {
        BOOST_ASSERT(revert_);
        revert_ = false;
    }
};

} // uri
} // beast
} // boost

#endif
