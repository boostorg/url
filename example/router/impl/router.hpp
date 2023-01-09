//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#include <boost/url/detail/except.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/grammar/unsigned_rule.hpp>
#include <boost/mp11/algorithm.hpp>

namespace boost {
namespace urls {

template <class T>
template <class U>
void
router<T>::
route(string_view path, U&& resource)
{
    BOOST_STATIC_ASSERT(
        std::is_same<T, U>::value ||
        std::is_convertible<U, T>::value);
    data_.emplace_back(std::forward<U>(resource));
    route_impl( path, data_.size() - 1 );
}

template <class T>
template <std::size_t N>
bool
router<T>::
match(string_view request, match_results<N>& m) const noexcept
{
    string_view matches[N];
    string_view ids[N];
    string_view* matches_it = matches;
    string_view* ids_it = ids;
    auto p = match_impl( request, matches_it, ids_it );
    if (p)
    {
        m = { data_.data(), p, matches, ids,
              static_cast<std::size_t>(
                  matches_it - matches) };
        return true;
    }
    m = {};
    return false;
}

namespace detail {
template <class Tuple>
struct push_fn
{
    string_view* it;
    Tuple tp;
    bool any_fail = false;

    push_fn(string_view* matches, Tuple&& t)
        : it(matches)
        , tp(std::move(t))
    {}

    template <std::size_t I>
    void
    operator()( mp11::mp_size_t<I> ){
        convert(*it++, std::get<I>(tp));
    }

private:
    void
    convert(string_view match, string_view& res)
    {
        res = match;
    }

    void
    convert(pct_string_view match, std::string& res)
    {
        match.decode({}, string_token::assign_to(res));
    }

    template <
        class T,
        typename std::enable_if<
            std::is_integral<T>::value,
            int>::type = 0>
    void
    convert(pct_string_view match, T& res)
    {
        auto dmatch = *match;
        static constexpr T Digits10 =
            std::numeric_limits<
                T>::digits10 + 1;
        if (dmatch.size() > Digits10)
        {
            any_fail = true;
            return;
        }
        char digits[Digits10];
        auto it0 = dmatch.begin();
        char* it1 = digits;
        auto end0 = dmatch.end();
        while (it0 != end0)
            *it1++ = *it0++;
        auto rv = grammar::parse(
            {digits, const_cast<char const*>(it1)},
            grammar::unsigned_rule<T>{});
        if (!rv)
            any_fail = true;
        else
            res = *rv;
    }
};
}

template <class T>
template <class ...Args>
result<T>
router<T>::
match_to(string_view request, Args&... args) const
{
    static constexpr std::size_t M = sizeof...(Args);
    string_view matches[M];
    string_view ids[M];
    string_view* matches_it = matches;
    string_view* ids_it = ids;
    auto p = match_impl( request, matches_it, ids_it );
    if (!p)
    {
        BOOST_URL_RETURN_EC(
            grammar::error::mismatch);
    }
    auto n = static_cast<std::size_t>(
        matches_it - matches);
    if (n != M)
        return false;
    detail::push_fn<std::tuple<Args&&...>> f(
        matches, std::forward_as_tuple(
            std::forward<Args>(args)...));
    mp11::mp_for_each<mp11::mp_iota_c<M>>(f);
    if (f.any_fail)
    {
        BOOST_URL_RETURN_EC(
            grammar::error::mismatch);
    }
    return data_[p->resource_idx];
}

template <class T>
template <std::size_t N>
T const&
router<T>::match_results<N>::
operator*() const
{
    BOOST_ASSERT(leaf_);
    BOOST_ASSERT(leaf_->resource_idx != node::npos);
    return data_[leaf_->resource_idx];
}

template <class T>
template <std::size_t N>
T const&
router<T>::match_results<N>::
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
