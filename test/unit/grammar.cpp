//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#if 1
// Test that header file is self-contained.
#include <boost/url/grammar.hpp>

#else
#include <boost/url/string_view.hpp>
#include <boost/url/error_code.hpp>
#include <new>
#include <tuple>
#include <utility>
#include "test_suite.hpp"

#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

#if 0
namespace grammar_ {
namespace grammar = urls::grammar;
using grammar::error;
using grammar::is_rule;
using grammar::parse_tag;
#endif

//------------------------------------------------
//
// parse
//
//------------------------------------------------

struct none
{
};

//------------------------------------------------

template<class Rule>
struct not_empty_rule
{
    using value_type =
        typename Rule::value_type;

    explicit
    not_empty_rule(
        Rule const& r)
        : r_(r)
    {
    }

    friend
    void
    parse(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        not_empty_rule const& r,
        value_type& t)
    {
        auto const it0 = it;
        t = grammar::parse(
            it, end, ec, r.r_);
        if( ! ec.failed() &&
            it == it0)
        {
            // can't be empty
            ec = error::syntax;
        }
    }

private:
    Rule r_;
};

//------------------------------------------------
//
// token_rule
//
//------------------------------------------------

template<class CharSet>
struct token_rule_t
{
    using value_type = string_view;

    BOOST_STATIC_ASSERT(
        grammar::is_charset<CharSet>::value);

    constexpr
    token_rule_t() = default;

    explicit
    constexpr
    token_rule_t(
        CharSet const& cs) noexcept
        : cs_(cs)
    {
    }

    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        token_rule_t<CharSet> const& r,
        string_view& t) noexcept
    {
        auto const it0 = it;
        if(it == end)
        {
            ec = grammar::error::incomplete;
            return;
        }
        it = (grammar::find_if_not)(it, end, r.cs_);
        if(it == it0)
        {
            ec = grammar::error::syntax;
            return;
        }
        t = string_view(it0, it - it0);
        ec = {};
    }

private:
    CharSet cs_;
};

template<class CharSet>
constexpr
auto
token_rule(
    CharSet const& cs) noexcept ->
        token_rule_t<CharSet>
{
    return token_rule_t<CharSet>(cs);
}

//------------------------------------------------
//
// ows_comma_ows_rule
//
//------------------------------------------------

struct ws_t
{
    constexpr
    bool
    operator()(char c) const noexcept
    {
        return c == ' ' || c == '\t';
    }
};

constexpr ws_t ws{};

/*
    @par BNF
    @code
    ows-comma   = OWS "," OWS
    @endcode
*/
struct ows_comma_ows_rule_t
{
    using value_type = none;

    constexpr
    ows_comma_ows_rule_t() = default;

    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it0,
        char const* end,
        error_code& ec,
        ows_comma_ows_rule_t const&,
        none&) noexcept
    {
        auto it = it0;
        // OWS
        it = grammar::find_if_not(
            it, end, ws);
        if(it == end)
        {
            ec = error::syntax;
            return;
        }
        // ","
        if(*it != ',')
        {
            ec = error::syntax;
            return;
        }
        ++it;
        // OWS
        it = grammar::find_if_not(
            it, end, ws);
        it0 = it;
        ec = {};
    }
};

constexpr ows_comma_ows_rule_t ows_comma_ows_rule{};

//------------------------------------------------
//
// range
//
//------------------------------------------------

namespace detail {

template<class T>
class optional
{
    BOOST_STATIC_ASSERT(
        std::is_trivial<T>::value);

    union U
    {
        T t;

        constexpr U() = default;
        constexpr U(
            T const& t_) noexcept
            : t(t_)
        {
        }
    };
    U u_;
    bool b_ = false;

public:
    constexpr optional() = default;

    ~optional()
    {
        if(b_)
            u_.t.~T();
    }

    constexpr
    optional(
        T const& t) noexcept
        : u_(t)
        , b_(true)
    {
    }

    constexpr
    optional(
        optional const& other) noexcept = default;

    constexpr
    T const&
    operator*() const noexcept
    {
        return u_.t;
    }
};

} // detail

/** A forward range of parsed elements

    @tparam R The rule used to parse each element
*/
template<class R>
class range
{
    static_assert(
        is_rule<R>::value,
        "Rule requirements not met");

public:
    using value_type = typename R::value_type;
    using reference = value_type;
    using const_reference = value_type;
    using pointer = void const*;
    using size_type = std::size_t;
    using difference_type =
        std::ptrdiff_t;

    using fn = void(*)(
        char const*&,
        char const*,
        error_code&,
        R const&,
        value_type&);

    class iterator;
    using const_iterator = iterator;

    ~range() = default;
    range() = default;
    range(
        range const&) noexcept = default;
    range& operator=(
        range const& v) noexcept = default;

    iterator begin() const noexcept;
    iterator end() const noexcept;

    /** Return the parsed string
    */
    string_view
    string() const noexcept
    {
        return s_;
    }

    /** Return the number of elements in the range
    */
    std::size_t
    size() const noexcept
    {
        return n_;
    }

    /** Return true if the range is empty
    */
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

private:
    string_view s_;
    std::size_t n_ = 0;
    urls::detail::optional<R> r_;
    fn begin_ = nullptr;
    fn increment_ = nullptr;

    range(
        string_view s,
        std::size_t n,
        R const& r,
        fn begin,
        fn increment) noexcept
        : s_(s)
        , n_(n)
        , r_(r)
        , begin_(begin)
        , increment_(increment)
    {
    }

    template<class R_>
    friend
    range<R_>
    parse_range(
        char const*& it,
        char const* end,
        error_code& ec,
        R_ const& r,
        typename range<R_>::fn begin,
        typename range<R_>::fn increment,
        std::size_t N,
        std::size_t M);
};

template<class R>
class range<R>::iterator
{
public:
    using value_type = typename R::value_type;
    using reference = value_type const&;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() = default;
    iterator(
        iterator const&) noexcept = default;
    iterator& operator=(
        iterator const&) noexcept = default;

    reference
    operator*() const noexcept
    {
        return t_;
    }

    bool
    operator==(
        iterator other) const noexcept
    {
        // can't compare iterators
        // from different containers!
        BOOST_ASSERT(r_ == other.r_);

        return p_ == other.p_;
    }

    bool
    operator!=(
        iterator other) const noexcept
    {
        return !(*this == other);
    }

    iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(p_ != nullptr);
        error_code ec;
        auto const end =
            r_->s_.data() +
            r_->s_.size();
        r_->increment_(p_, end,
            ec, *r_->r_, t_);
        if(ec == error::end)
            p_ = nullptr;
        else
            BOOST_ASSERT(! ec.failed());
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

private:
    friend class range<R>;

    range<R> const* r_ = nullptr;
    char const* p_ = nullptr;
    typename R::value_type t_;

    iterator(
        range<R> const& r) noexcept
        : r_(&r)
        , p_(r.s_.data())
    {
        error_code ec;
        auto const end =
            r_->s_.data() +
            r_->s_.size();
        r_->begin_(p_, end,
            ec, *r_->r_, t_);
        if(ec == error::end)
            p_ = nullptr;
        else
            BOOST_ASSERT(! ec.failed());
    }

    constexpr
    iterator(
        range<R> const& r,
        int) noexcept
        : p_(nullptr)
        , r_(&r)
    {
    }
};

template<class T>
auto
range<T>::
begin() const noexcept ->
    iterator
{
    return { *this };
}

template<class T>
auto
range<T>::
end() const noexcept ->
    iterator
{
    return { *this, 0 };
}

template<class R>
range<R>
parse_range(
    char const*& it,
    char const* end,
    error_code& ec,
    R const& r,
    typename range<R>::fn begin,
    typename range<R>::fn increment,
    std::size_t N = 0,
    std::size_t M = std::size_t(-1))
{
    typename R::value_type t;
    std::size_t n = 0;
    auto const it0 = it;
    begin(it, end, ec, r, t);
    if(ec.failed())
    {
        if(ec != error::end)
            return {};
        if(n < N)
        {
            // too few
            ec = error::syntax;
            return {};
        }

        // good
        ec = {};
        return {
            string_view(it0, it - it0),
                n, r, begin, increment};
    }

    for(;;)
    {
        ++n;
        increment(it, end, ec, r, t);
        if(ec.failed())
        {
            if(ec != error::end)
                return {};
            ec = {};
            break;
        }
        if(n > M)
        {
            // too many
            ec = error::syntax;
            return {};
        }
    }

    // good
    return {
        string_view(it0, it - it0),
            n, r, begin, increment};
}

//------------------------------------------------

/** Rule for a comma-delimited list of elements

    This rule defines a list containing
    at least n and at most m of Element,
    each separated by at least one comma
    and optional whitespace.

    @par BNF
    @code
    #element       => [ 1#element ]
    1#element      => element *( OWS "," OWS element )
    <n>#<m>element => element <n-1>*<m-1>( OWS "," OWS element )
    @endcode

    Senders must emit compliant values, but
    receivers should accept values generated
    with the legacy production rules:

    @par Legacy BNF
    @code
    #element => [ element ] *( OWS "," OWS [ element ] )
    @endcode

    @tparam R The rule to use for elements
    @tparam N The minimum number of elements, which may be zero
    @tparam M The maximum number of elements.

    @par Specification
    @li <a href="https://www.rfc-editor.org/rfc/rfc9110.html#section-5.6.1"
        >5.6.1. Lists (#rule ABNF Extension) (rfc9110)</a>
*/
template<
    class R,
    std::size_t N = 0,
    std::size_t M = std::size_t(-1)>
struct list_rule_t
{
    BOOST_STATIC_ASSERT(M >= N);

    using value_type = range<R>;

    constexpr
    list_rule_t(
        R const& r)
        : r_(r)
    {
    }

    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        list_rule_t const& r,
        value_type& t)
    {
        t = parse_range(
            it, end, ec, r.r_,
            increment,
            increment,
            N,
            M);
    }

private:
    static
    void
    increment(
        char const*& it,
        char const* end,
        error_code& ec,
        R const& r,
        typename R::value_type& t)
    {
        // *( OWS "," OWS )
        for(;;)
        {
            parse(
                it, end, ec,
                    ows_comma_ows_rule);
            if(ec.failed())
                break;
        }
        // element
        t = parse(
            it, end, ec, r);
        if(! ec.failed())
        {
            // *( OWS "," OWS )
            for(;;)
            {
                parse(
                    it, end, ec,
                        ows_comma_ows_rule);
                if(ec.failed())
                    break;
            }
            ec = {};
            return;
        }
        ec = error::end;
        return;
    }

    R r_;
};

template<
    class R,
    std::size_t N = 0,
    std::size_t M = std::size_t(-1)>
constexpr
auto
list_rule(R const& r) ->
    list_rule_t<R, N, M>
{
    return list_rule_t<R, N, M>{ r };
}

//------------------------------------------------

constexpr auto transfer_coding =
    token_rule(grammar::alnum_chars);

constexpr auto transfer_encoding =
    list_rule(transfer_coding);

//------------------------------------------------

struct grammar_test
{
    void
    testList()
    {
        auto const bad = [](
            string_view s)
        {
            error_code ec;
            parse(
                s, ec, transfer_encoding);
            BOOST_TEST(ec.failed());
        };

        auto const ok = [](
            string_view s,
            std::initializer_list<
                string_view> init)
        {
            error_code ec;
            auto v = parse(
                s, ec, transfer_encoding);
            if(! BOOST_TEST(! ec.failed()))
                return;
            if(! BOOST_TEST_EQ(
                v.size(), init.size()))
                return;
            auto it0 = init.begin();
            auto it1 = v.begin();
            while(it0 != init.end())
            {
                BOOST_TEST_EQ(*it0, *it1);
                ++it0;
                ++it1;
            }
        };

        ok( "", {} );

        bad( " " );
        bad( "\t" );
        bad( " \t" );

        ok( ",", {} );
        ok( " ,", {} );
        ok( "\t,", {} );
        ok( " \t,", {} );
        ok( ", ", {} );
        ok( ",\t", {} );
        ok( ", \t", {} );
        ok( "\t , \t", {} );

        ok( "x", { "x" } );
        ok( "x,y", { "x", "y" } );
        ok( "x,y,z", { "x", "y", "z" } );
        ok( "x,,", { "x" } );
        ok( ",,x", { "x" } );
        ok( " ,\t ,  x  ,", { "x" } );
        ok( "x , , , y, ,   ,z,    ", { "x", "y", "z" } );
    }

    void
    run()
    {
        testList();
    }
};

TEST_SUITE(
    grammar_test,
    "boost.url.grammar");

} // grammar_
} // urls
} // boost

#endif
