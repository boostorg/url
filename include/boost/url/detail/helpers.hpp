//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_HELPERS_HPP
#define BOOST_URL_DETAIL_HELPERS_HPP

namespace boost {
namespace urls {
namespace detail {

// mix-in to provide part
// constants and variables
struct parts_helper
{
    using pos_t = std::size_t;

    enum
    {
        id_scheme = -1, // trailing ':'
        id_user,        // leading "//"
        id_pass,        // leading ':', trailing '@'
        id_host,
        id_port,        // leading ':'
        id_path,
        id_query,       // leading '?'
        id_frag,        // leading '#'
        id_end          // one past the end
    };

    static
    constexpr
    pos_t zero_ = 0;

    static
    constexpr
    char const* const empty_ = "";
};

//------------------------------------------------

// mix-in that helps with
// type-erasing iterators
struct any_helper
{
    // type-erased forward
    // iterator with value_type T
    template<class T>
    struct BOOST_SYMBOL_VISIBLE
        any_fwdit
    {
        using value_type = T;

        virtual
        ~any_fwdit()
        {
        }

        virtual
        any_fwdit&
        operator++() noexcept = 0;

        virtual
        value_type operator*(
            ) const noexcept = 0;

        virtual
        any_fwdit&
        operator=(
            any_fwdit const& other
                ) noexcept = 0;

        virtual
        bool
        operator==(
            any_fwdit const& other
                ) const noexcept = 0;

        bool
        operator!=(
            any_fwdit const& other
                ) const noexcept
        {
            return !(*this == other);
        }
    };

    template<class FwdIt, class T>
    class any_fwdit_impl :
        public any_fwdit<T>
    {
        FwdIt it_;

    public:
        explicit
        any_fwdit_impl(
            FwdIt const& it) noexcept
            : it_(it)
        {
        }

        ~any_fwdit_impl()
        {
        }

        any_fwdit<T>&
        operator++() noexcept override
        {
            ++it_;
            return *this;
        }

        T
        operator*() const noexcept override
        {
            return *it_;
        }

        any_fwdit<T>&
        operator=(
            any_fwdit<T> const& other)
                noexcept override
        {
            it_ = static_cast<
                any_fwdit_impl const&>(
                    other).it_;
            return *this;
        }

        bool
        operator==(
            any_fwdit<T> const& other)
                const noexcept override
        {
            return it_ == static_cast<
                any_fwdit_impl const&>(
                    other).it_;
        }
    };
};

} // detail
} // urls
} // boost

#endif
