//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {

template <class T>
template <class U>
void
router<T>::
route(string_view path, U&& resource)
{
    BOOST_STATIC_ASSERT(
        std::is_same<T, U>::value        ||
        std::is_convertible<T, U>::value ||
        std::is_base_of<T, U>::value);
    using U_ = typename std::decay<U>::type;
    struct impl : any_resource
    {
        U_ u;

        explicit
        impl(U&& u_)
            : u(std::forward<U>(u_))
        {
        }

        void const*
        get() const noexcept override
        {
            return static_cast<T const*>(&u);
        }
    };
    any_resource const* p = new impl(
        std::forward<U>(resource));
    route_impl( path, p );
}

template <class T>
auto
router<T>::
match(pct_string_view request)
    -> match_results
{
    return { match_impl( request ) };
}

template <class T>
T const&
router<T>::match_results::
operator*() const
{
    BOOST_ASSERT(leaf_);
    BOOST_ASSERT(leaf_->resource);
    return *reinterpret_cast<
        T const*>(leaf_->resource->get());
}

template <class T>
T const&
router<T>::match_results::
value() const
{
    if( has_value() )
    {
        return operator*();
    }
    detail::throw_system_error(grammar::error::mismatch);
}

} // urls
} // boost
