//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_HPP
#define BOOST_URL_IMPL_PARAMS_HPP

#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params::iterator
{
    url* u_ = nullptr;
    std::size_t i_ = 0;
    string_value::allocator a_;

    friend class params;

    iterator(
        url& u,
        std::size_t i,
        string_value::allocator a) noexcept
        : u_(&u)
        , i_(i)
        , a_(a)
    {
    }

public:
    using value_type = params::value_type;
    using reference = params::reference;
    using const_reference = params::reference;
    using pointer = void*;
    using const_pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::random_access_iterator_tag;

    iterator() = default;

    iterator&
    operator++() noexcept
    {
        ++i_;
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    iterator&
    operator--() noexcept
    {
        --i_;
        return *this;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    BOOST_URL_DECL
    reference
    operator*() const;

    friend
    bool
    operator==(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return a.u_ == b.u_ &&
            a.i_ == b.i_;
    }

    friend
    bool
    operator!=(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return a.u_ != b.u_ ||
            a.i_ != b.i_;
    }

    // LegacyRandomAccessIterator

    iterator&
    operator+=(
        ptrdiff_t n) noexcept
    {
        i_ += n;
        return *this;
    }

    friend
    iterator
    operator+(
        iterator it,
        ptrdiff_t n) noexcept
    {
        return {
            *it.u_,
            it.i_ + n,
            it.a_ };
    }

    friend
    iterator
    operator+(
        ptrdiff_t n,
        iterator it) noexcept
    {
        return {
            *it.u_,
            it.i_ + n,
            it.a_ };
    }

    iterator&
    operator-=(
        ptrdiff_t n) noexcept
    {
        i_ -= n;
        return *this;
    }

    friend
    iterator
    operator-(
        iterator it,
        ptrdiff_t n) noexcept
    {
        return {
            *it.u_,
            it.i_ - n,
            it.a_ };
    }

    friend
    std::ptrdiff_t
    operator-(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return static_cast<
            std::ptrdiff_t>(
                a.i_) - b.i_;
    }

    reference
    operator[](ptrdiff_t n) const
    {
        return *(*this + n);
    }

    friend
    bool
    operator<(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(
            a.u_ == b.u_);
        return a.i_ < b.i_;
    }

    friend
    bool
    operator>(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(
            a.u_ == b.u_);
        return b < a;
    }

    friend
    bool
    operator>=(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(
            a.u_ == b.u_);
        return !(a < b);
    }

    friend
    bool
    operator<=(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(
            a.u_ == b.u_);
        return !(a > b);
    }
};

//------------------------------------------------
//
// Members
//
//------------------------------------------------

template<class FwdIt>
void
params::
assign(FwdIt first, FwdIt last,
    std::forward_iterator_tag)
{
    using detail::
        make_plain_params_iter;
    u_->edit_params(
        0,
        size(),
        make_plain_params_iter(
            first, last),
        make_plain_params_iter(
            first, last));
}

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

auto
params::
at(std::size_t pos) const ->
    reference
{
    if(pos >= size())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return (*this)[pos];
}

auto
params::
front() const ->
    reference
{
    return (*this)[0];
}

auto
params::
back() const ->
    reference
{
    return (*this)[size() - 1];
}

//--------------------------------------------
//
// Iterators
//
//--------------------------------------------

auto
params::
begin() const noexcept ->
    iterator
{
    return { *u_, 0, a_ };
}

auto
params::
end() const noexcept ->
    iterator
{
    return { *u_, size(), a_ };
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

std::size_t
params::
size() const noexcept
{
    return u_->param_count();
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

void
params::
clear() noexcept
{
    erase(begin(), end());
}

//------------------------------------------------

auto
params::
insert(
    iterator before,
    value_type const& v) ->
        iterator
{
    return insert(
        before, &v, &v + 1);
}

auto
params::
insert(
    iterator before,
    std::initializer_list<
        value_type> init) ->
            iterator
{
    return insert(before,
        init.begin(), init.end());
}

template<class FwdIt>
auto
params::
insert(
    iterator before,
    FwdIt first,
    FwdIt last) ->
        iterator
{
    return insert(before, first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

template<class FwdIt>
auto
params::
insert(
    iterator before,
    FwdIt first,
    FwdIt last,
    std::forward_iterator_tag) ->
        iterator
{
    using detail::
        make_plain_params_iter;
    BOOST_ASSERT(before.u_ == u_);
    u_->edit_params(
        before.i_,
        before.i_,
        make_plain_params_iter(
            first, last),
        make_plain_params_iter(
            first, last));
    return before;
}

//------------------------------------------------

auto
params::
replace(
    iterator pos,
    value_type const& value) ->
        iterator
{
    return replace(
        pos,
        pos + 1,
        &value,
        &value + 1);
}

template<class FwdIt>
auto
params::
replace(
    iterator from,
    iterator to,
    FwdIt first,
    FwdIt last) ->
        iterator
{
    using detail::
        make_plain_params_iter;
    BOOST_ASSERT(from.u_ == u_);
    BOOST_ASSERT(to.u_ == u_);
    u_->edit_params(
        from.i_,
        to.i_,
        make_plain_params_iter(
            first, last),
        make_plain_params_iter(
            first, last));
    return from;
}

auto
params::
replace(
    iterator from,
    iterator to,
    std::initializer_list<
        value_type> init) ->
    iterator
{
    return replace(
        from,
        to,
        init.begin(),
        init.end());
}

auto
params::
remove_value(
    iterator pos) ->
        iterator
{
    BOOST_ASSERT(pos.u_ == u_);
    auto r = u_->get_param(pos.i_);
    value_type v{
        string_view{
            u_->s_ + r.pos + 1,
            r.nk - 1},
        string_view{},
        false};
    u_->edit_params(
        pos.i_,
        pos.i_ + 1,
        detail::make_enc_params_iter(
            &v, &v + 1),
        detail::make_enc_params_iter(
            &v, &v + 1));
    return pos;
}

auto
params::
replace_value(
    iterator pos,
    string_view value) ->
        iterator
{
    (void)pos;
    (void)value;
    // VFALCO TODO
    return {};
}

template<class Value>
auto
params::
replace_value(
    iterator pos,
    Value const& value) ->
        typename std::enable_if<
            is_stringlike<Value>::value,
            iterator>::type
{
    return replace_value(
        pos, to_string_view(value));
}

//------------------------------------------------

auto
params::
emplace_at(
    iterator pos,
    string_view key,
    string_view value) ->
        iterator
{
    using detail::
        make_plain_params_iter;
    value_type v{
        key, value, true };
    BOOST_ASSERT(pos.u_ == u_);
    u_->edit_params(
        pos.i_,
        pos.i_ + 1,
        make_plain_params_iter(
            &v, &v + 1),
        make_plain_params_iter(
            &v, &v + 1));
    return pos;
}

template<
    class Key,
    class Value>
auto
params::
emplace_at(
    iterator pos,
    Key const& key,
    Value const& value) ->
        typename std::enable_if<
            is_stringlike<Key>::value &&
            is_stringlike<Value>::value,
            iterator>::type
{
    return emplace_at(
        pos,
        to_string_view(key),
        to_string_view(value));
}

auto
params::
emplace_at(
    iterator pos,
    string_view key) ->
        iterator
{
    BOOST_ASSERT(pos.u_ == u_);
    auto r = u_->get_param(pos.i_);
    value_type v{key,
        string_view{}, false};
    u_->edit_params(
        pos.i_,
        pos.i_ + 1,
        detail::make_enc_params_iter(
            &v, &v + 1),
        detail::make_enc_params_iter(
            &v, &v + 1));
    return pos;
}

template<class Key>
auto
params::
emplace_at(
    iterator pos,
    Key const& key) ->
        typename std::enable_if<
            is_stringlike<Key>::value,
            iterator>::type
{
    return emplace_at(
        pos, to_string_view(key));
}

auto
params::
emplace_before(
    iterator before,
    string_view key,
    string_view value) ->
        iterator
{
    return insert(
        before,
        value_type{
            key,
            value,
            true});
}

template<
    class Key,
    class Value>
auto
params::
emplace_before(
    iterator before,
    Key const& key,
    Value const& value) ->
        typename std::enable_if<
            is_stringlike<Key>::value &&
            is_stringlike<Value>::value,
            iterator>::type
{
    return emplace_before(
        before,
        to_string_view(key),
        to_string_view(value));
}

auto
params::
emplace_before(
    iterator before,
    string_view key) ->
        iterator
{
    return insert(
        before,
        value_type{
            to_string_view(key),
            string_view{},
            false});
}

template<class Key>
auto
params::
emplace_before(
    iterator before,
    Key const& key) ->
        typename std::enable_if<
            is_stringlike<Key>::value,
            iterator>::type
{
    return emplace_before(
        before,
        to_string_view(key));
}

//------------------------------------------------

auto
params::
erase(iterator pos) ->
    iterator
{
    return erase(pos, pos + 1);
}

auto
params::
erase(
    iterator first,
    iterator last) ->
        iterator
{
    BOOST_ASSERT(first.u_ == u_);
    BOOST_ASSERT(last.u_ == u_);
    string_view s;
    u_->edit_params(
        first.i_,
        last.i_,
        detail::enc_query_iter(s),
        detail::enc_query_iter(s));
    return first;
}

//------------------------------------------------

auto
params::
emplace_back(
    string_view key,
    string_view value) ->
        iterator
{
    return insert(
        end(), value_type{
            key, value, true});
}

template<
    class Key,
    class Value>
auto
params::
emplace_back(
    Key const& k,
    Value const& v) ->
        typename std::enable_if<
            is_stringlike<Key>::value &&
            is_stringlike<Value>::value,
            iterator>::type
{
    return emplace_back(
        to_string_view(k),
        to_string_view(v));
}

template<class Key>
auto
params::
emplace_back(
    Key const& key) ->
        typename std::enable_if<
            is_stringlike<Key>::value,
            iterator>::type
{
    return insert(
        end(), value_type{
            key, string_view{},
                false});
}

void
params::
push_back(
    value_type const& v)
{
    insert(end(), v);
}


void
params::
pop_back() noexcept
{
    erase(end() - 1);
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

auto
params::
find(string_view key) const noexcept ->
    iterator
{
    return find(begin(), key);
}

template<class Key>
auto
params::
find(Key const& key) const noexcept ->
    typename std::enable_if<
        is_stringlike<Key>::value,
        iterator>::type
{
    return find(to_string_view(key));
}

template<class Key>
auto
params::
find(
    iterator from,
    Key const& key) const noexcept ->
        typename std::enable_if<
            is_stringlike<Key>::value,
            iterator>::type
{
    return find(from,
        to_string_view(key));
}

} // urls
} // boost

#endif
