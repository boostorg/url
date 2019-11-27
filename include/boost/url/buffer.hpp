//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_BUFFER_HPP
#define BOOST_URL_BUFFER_HPP

#include <boost/beast/url/view.hpp>
#include <cstdlib>
#include <iterator>

namespace boost {
namespace beast {
namespace url {

class buffer : public view
{
protected:
    char* data_ = nullptr;
    std::size_t capacity_ = 0;

    buffer() = default;

    buffer(char* data, std::size_t capacity)
        : data_(data)
        , capacity_(capacity)
    {
        BOOST_ASSERT(data != nullptr);
        BOOST_ASSERT(capacity > 0);
        this->p_.data = data_;
        data_[0] = '\0';
    }

    void
    reset(char* data, std::size_t capacity)
    {
        BOOST_ASSERT(data != nullptr);
        BOOST_ASSERT(capacity > static_cast<
            std::size_t>(this->p_.size));
        data_ = data;
        capacity_ = capacity;
        this->p_.data = data;
        data_[this->p_.size] = '\0'; // null terminate
    }

    virtual
    void
    grow(std::size_t at_least) = 0;

public:
    class const_iterator;

    buffer(buffer const&) = delete;
    buffer& operator=(buffer const&) = delete;
    virtual ~buffer() = default;

    /** Returns the maximum number of characters which may be stored.

        This does not include the null which terminates all
        sequences.
    */
    virtual
    std::size_t
    max_size() const = 0;

    /** Returns the total number of characters which may be stored without reallocating.

        This does not include the null which terminates all
        sequences.
    */
    std::size_t
    capacity() const
    {
        return capacity_;
    }

    /// Return an const_iterator to the beginning of the sequence
    const_iterator
    begin() noexcept;

    /** Return an const_iterator to one past the end of the sequence

        This does not include the null which terminates all
        sequences.
    */
    const_iterator
    end() noexcept;

    parts&
    parts() noexcept
    {
        return this->p_;
    }

    void
    clear()
    {
        this->p_ = {};
        if(capacity_ > 0)
        {
            this->p_.data = data_;
            data_[0] = '\0';
        }
    }

    parts::piece
    piece_from(const_iterator const& it) const;

    void
    append(char c)
    {
        if(this->p_.size + 1U >= capacity_)
            grow(1);
        data_[this->p_.size++] = c;
        data_[this->p_.size] = '\0';
    }

    void
    append(char const* s, unsigned short len)
    {
        if(static_cast<std::size_t>(
                this->p_.size + len) >= capacity_)
            grow(len);
        std::memcpy(&data_[this->p_.size], s, len);
        this->p_.size += len;
        data_[this->p_.size] = '\0';
    }

    char*
    prepare(unsigned short len)
    {
        if(static_cast<std::size_t>(
                this->p_.size + len) >= capacity_)
            grow(len);
        auto s = &data_[this->p_.size];
        this->p_.size += len;
        data_[this->p_.size] = '\0';
        return s;
    }
};

//------------------------------------------------------------------------------

class buffer::const_iterator
{
    friend class buffer;

    buffer* b_ = nullptr;
    unsigned short offset_ = 0;

    const_iterator(buffer& b, unsigned short offset)
        : b_(&b)
        , offset_(offset)
    {
    }

public:
    using value_type = char;
    using pointer = char const*;
    using reference = char const&;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::random_access_iterator_tag;

    const_iterator() = default;
    const_iterator(const_iterator const&) = default;
    const_iterator& operator=(const_iterator const&) = default;

    value_type
    operator*()
    {
        BOOST_ASSERT(b_);
        return *(b_->data_ + offset_);
    }

    const_iterator&
    operator++()
    {
        BOOST_ASSERT(*this < b_->end());
        offset_++;
        return *this;
    }

    const_iterator
    operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    const_iterator&
    operator--()
    {
        BOOST_ASSERT(*this > b_->begin());
        offset_--;
        return *this;
    }

    const_iterator
    operator--(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    friend
    bool
    operator==(
        const_iterator const& lhs,
        const_iterator const& rhs)
    {
        BOOST_ASSERT(lhs.b_ == rhs.b_);
        return lhs.offset_ == rhs.offset_;
    }

    friend
    bool
    operator!=(
        const_iterator const& lhs,
        const_iterator const& rhs)
    {
        BOOST_ASSERT(lhs.b_ == rhs.b_);
        return lhs.offset_ != rhs.offset_;
    }

    friend
    bool
    operator<(
        const_iterator const& lhs,
        const_iterator const& rhs)
    {
        BOOST_ASSERT(lhs.b_ == rhs.b_);
        return lhs.offset_ < rhs.offset_;
    }

    friend
    bool
    operator<=(
        const_iterator const& lhs,
        const_iterator const& rhs)
    {
        BOOST_ASSERT(lhs.b_ == rhs.b_);
        return lhs.offset_ <= rhs.offset_;
    }

    friend
    bool
    operator>(
        const_iterator const& lhs,
        const_iterator const& rhs)
    {
        BOOST_ASSERT(lhs.b_ == rhs.b_);
        return lhs.offset_ > rhs.offset_;
    }

    friend
    bool
    operator>=(
        const_iterator const& lhs,
        const_iterator const& rhs)
    {
        BOOST_ASSERT(lhs.b_ == rhs.b_);
        return lhs.offset_ >= rhs.offset_;
    }
};

//------------------------------------------------------------------------------

inline
auto
buffer::
begin() noexcept ->
    const_iterator
{
    return {*this, 0};
}

inline
auto
buffer::
end() noexcept ->
    const_iterator
{
    return {*this, p_.size};
}

inline
parts::piece
buffer::
piece_from(const_iterator const& it) const
{
    BOOST_ASSERT(it.b_ == this);
    parts::piece result;
    result.offset = it.offset_;
    result.size = static_cast<unsigned short>(
        this->p_.size - result.offset);
    return result;
}

//------------------------------------------------------------------------------

template<std::size_t N>
class static_buffer : public buffer
{
    char buf_[N + 1]; // room for null

    void
    grow(std::size_t) override
    {
        BOOST_THROW_EXCEPTION(std::length_error{
            "static_buffer overflow"});
    }

public:
    static_buffer()
        : buffer(buf_, N)
    {
    }

    std::size_t
    max_size() const override
    {
        return N;
    }
};

} // url
} // beast
} // boost

#include <memory>
#include <type_traits>

namespace boost {
namespace beast {
namespace url {

template<class Allocator>
class dynamic_buffer : public buffer
{
    static_assert(std::is_same<char,
        typename std::allocator_traits<
            Allocator>::value_type>::value,
        "Allocator requirements not met");

    void
    grow(std::size_t at_least) override
    {
        boost::ignore_unused(at_least);
        BOOST_THROW_EXCEPTION(std::length_error{
            "static_buffer overflow"});
    }

public:
    std::size_t
    max_size() const override
    {
        return 0xffffffff;
    }
};

} // url
} // beast
} // boost

#endif
