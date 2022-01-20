//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_STRING_HPP
#define BOOST_URL_IMPL_STRING_HPP

namespace boost {
namespace urls {

const_string::
~const_string()
{
    decrement_refs();
}

template<class Allocator>
const_string::
const_string(
    std::size_t n,
    Allocator const& a,
    char*& dest)
    : refs_(nullptr),
      alloc_(a)
{
    if (n == 0)
    {
        static_cast<string_view&>(*this) =
            {nullptr, n};
        return;
    }
    using U = typename boost::type_with_alignment<
        alignof(ref_counter_t)>::type;
    auto constexpr S = sizeof(U);
    using A = allocator_rebind_t<
        any_allocator<ref_counter_t>, U>;
    A aa{alloc_};
    refs_ = reinterpret_cast<ref_counter_t*>(
        allocator_allocate(
          aa, (sizeof(ref_counter_t) + n + S - 1) / S));
    allocator_construct(alloc_, refs_, 1);
    dest = reinterpret_cast<char*>(refs_ + 1);
    static_cast<string_view&>(*this) = {dest, n};
}

template<class Allocator>
const_string::
const_string(
    string_view s,
    Allocator const& a)
{
    char* dest;
    *this = const_string(s.size(), a, dest);
    std::memcpy(dest, s.data(), s.size());
}

const_string::
const_string(
    const_string const& other)
    : string_view(other)
    , refs_(other.refs_)
    , alloc_(other.alloc_)
{
    if (refs_) {
        refs_->fetch_add(
            1, std::memory_order_relaxed);
    }
}

const_string&
const_string::
operator=(
    const_string const& other) & noexcept
{
    if (other.refs_) {
        other.refs_->fetch_add(
            1, std::memory_order_relaxed);
    }
    decrement_refs();
    refs_  = other.refs_;
    alloc_ = other.alloc_;
    static_cast<string_view&>(*this)
        = static_cast<const string_view&>(other);
    return *this;
}

inline
void
const_string::
decrement_refs()
{
    if (refs_
        && refs_->fetch_sub(1,
               std::memory_order_release) == 1)
    {
        std::atomic_thread_fence(
            std::memory_order_acquire);
        allocator_destroy(alloc_, refs_);
        using U = typename boost::type_with_alignment<
            alignof(ref_counter_t)>::type;
        auto constexpr S = sizeof(U);
        using A = allocator_rebind_t<
            any_allocator<ref_counter_t>, U>;
        A aa{ alloc_ };
        allocator_deallocate(
            aa, reinterpret_cast<U*>(refs_),
            (sizeof(ref_counter_t) + size() + S - 1) / S);
    }
}


} // urls
} // boost

#endif
