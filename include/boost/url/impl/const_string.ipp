//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_CONST_STRING_IPP
#define BOOST_URL_IMPL_CONST_STRING_IPP

#include <boost/url/const_string.hpp>
#include <boost/url/detail/over_allocator.hpp>
#include <memory>
#include <new>

namespace boost {
namespace urls {

const_string::
factory::
~factory()
{
    p_->release();
}

const_string::
factory::
factory(
    factory const& other) noexcept
    : p_(other.p_)
{
    ++p_->refs;
}

auto
const_string::
factory::
operator=(
    factory const& other) noexcept ->
        factory&
{
    ++other.p_->refs;
    p_->release();
    p_ = other.p_;
    return *this;
}

const_string::
factory::
factory() noexcept
    : p_([]
    {
        using A = std::allocator<char>;

        struct default_impl final
            : public factory::base
        {
            void
            destroy() noexcept override
            {
            }

            result
            construct(std::size_t size) const override
            {
                struct string final
                    : const_string::base
                {
                    void
                    destroy(std::size_t size) noexcept override
                    {
                        detail::over_allocator<
                            string, A> a(size, A{});
                        this->~string();
                        a.deallocate(this, 1);
                    }
                };
                detail::over_allocator<
                    string, A> a(size, A{});
                auto p = ::new(a.allocate(1)) string;
                return result{ p, reinterpret_cast<
                    char*>(p + 1), size };
            }
        };

        struct deleter
        {
            default_impl* p;

            ~deleter()
            {
                p->release();
            }
        };

        static default_impl d{};
        static deleter dp{&d};

        ++d.refs;

        return &d;
    }())
{
}

const_string
const_string::
factory::
operator()(string_view s) const
{
    return (*this)(s.size(),
        [&s](
            std::size_t,
            char* dest)
        {
            std::memcpy(
                dest,
                s.data(),
                s.size());
        });
}

//------------------------------------------------

const_string::
const_string(
    result const& r) noexcept
    : string_view(
        r.data, r.size)
    , p_(r.p)
{
}

const_string::
~const_string()
{
    p_->release(size());
}

const_string::
const_string() noexcept
    : string_view()
    , p_([]
    {
        struct empty_string final : base
        {
            void
            destroy(std::size_t) noexcept override
            {
            }
        };

        struct deleter
        {
            empty_string* p;

            ~deleter()
            {
                p->release(0);
            }
        };

        static empty_string cs{};
        static deleter d{&cs};

        ++cs.refs;

        return &cs;
    }())
{
}

const_string::
const_string(
    const_string const& other) noexcept
    : string_view(other)
    , p_(other.p_)
{
    ++p_->refs;
}

const_string&
const_string::
operator=(
    const_string const& other) noexcept
{
    ++other.p_->refs;
    p_->release(size());
    p_ = other.p_;
    static_cast<string_view&>(
        *this) = other;
    return *this;
}

} // urls
} // boost

#endif
