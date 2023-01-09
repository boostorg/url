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
route(string_view pattern, U&& v)
{
    BOOST_STATIC_ASSERT(
        std::is_same<T, U>::value ||
        std::is_convertible<U, T>::value);
    data_.emplace_back(std::forward<U>(v));
    route_impl( pattern, data_.size() - 1 );
}

template <class T>
template <std::size_t N>
T const*
router<T>::
match(string_view request, matches_storage<N>& m) const noexcept
{
    string_view matches[N];
    string_view ids[N];
    string_view* matches_it = matches;
    string_view* ids_it = ids;
    std::size_t idx = match_impl(
        request, matches_it, ids_it );
    if (idx != std::size_t(-1))
    {
        BOOST_ASSERT(matches_it >= matches);
        m = { matches, ids,
              static_cast<std::size_t>(
                  matches_it - matches) };
        return &data_[idx];
    }
    m = {};
    return nullptr;
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
    matches_storage<M> m;
    auto p = match(request, m);
    if (!p)
    {
        BOOST_URL_RETURN_EC(
            grammar::error::mismatch);
    }
    detail::push_fn<std::tuple<Args&&...>> f(
        m.matches_storage_, std::forward_as_tuple(
            std::forward<Args>(args)...));
    mp11::mp_for_each<mp11::mp_iota_c<M>>(f);
    if (f.any_fail)
    {
        BOOST_URL_RETURN_EC(
            grammar::error::mismatch);
    }
    return *p;
}

} // urls
} // boost
