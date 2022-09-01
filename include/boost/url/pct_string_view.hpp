//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PCT_STRING_VIEW_HPP
#define BOOST_URL_PCT_STRING_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/decode_opts.hpp>
#include <boost/url/error_types.hpp>
#include <boost/url/string_view.hpp>
#include <iterator>
#include <string>
#include <type_traits>

namespace boost {
namespace urls {

//------------------------------------------------

#ifndef BOOST_URL_DOCS
class pct_string_view;
namespace detail {
string_view&
ref(pct_string_view& s) noexcept;
// unchecked
pct_string_view
make_pct_string_view(
    string_view s,
    std::size_t dn) noexcept;
// unchecked
pct_string_view
make_pct_string_view(
    string_view s) noexcept;
} // detail
#endif

//------------------------------------------------

/*  The contract of this type is:

    Just like string_view except the string
    always contains valid percent-encoding,
    plus a few odds and ends to make it nice.
*/

class pct_string_view final
{
    string_view s_;
    std::size_t dn_ = 0;

#ifndef BOOST_URL_DOCS
    friend
    pct_string_view
    detail::make_pct_string_view(
        string_view, std::size_t) noexcept;

    friend
    string_view&
    detail::ref(pct_string_view&) noexcept;
#endif

    // unchecked construction
    pct_string_view(
        string_view s,
        std::size_t dn) noexcept
        : s_(s)
        , dn_(dn)
    {
    }

public:
    // types
    typedef std::char_traits<char> traits_type;
    typedef char value_type;
    typedef char* pointer;
    typedef char const* const_pointer;
    typedef char& reference;
    typedef char const& const_reference;
    typedef char const* const_iterator;
    typedef const_iterator iterator;
    typedef std::reverse_iterator<
        const_iterator> const_reverse_iterator;
    typedef const_reverse_iterator reverse_iterator;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    static constexpr std::size_t npos = string_view::npos;

    //--------------------------------------------

    /** Constructor
    */
    constexpr pct_string_view() = default;

    /** Constructor
    */
    constexpr
    pct_string_view(
        pct_string_view const&) = default;

    /** Constructor
    */
    pct_string_view(
        char const* s)
        : pct_string_view(
            string_view(s))
    {
    }

    /** Constructor
    */
    pct_string_view(
        char const* s,
        std::size_t len)
        : pct_string_view(
            string_view(s, len))
    {
    }

    /** Constructor
    */
#ifdef BOOST_URL_DOCS
    pct_string_view(
        char const* first,
        char const* last);
#else
    template<class End>
    pct_string_view(
        char const* first,
        End last,
        typename std::enable_if<
            std::is_same<End,
            char const*>::value
                >::type* = 0)
        : pct_string_view(
            string_view(first, last))
    {
    }
#endif

    /** Constructor

        @par Exception Safety
        Exceptions thrown on invalid input.
    */
    BOOST_URL_DECL
    pct_string_view(
        string_view s);

    /** Assignment
    */
    pct_string_view& operator=(
        pct_string_view const&) = default;

    friend
    result<pct_string_view>
    make_pct_string_view(
        string_view s) noexcept;

    //--------------------------------------------

    /** Return the decoded size
    */
    std::size_t
    decoded_size() const noexcept
    {
        return dn_;
    }

    /** Return the string with percent-decoding
    */
    BOOST_URL_DECL
    std::string
    decode_to_string(
        decode_opts const& opt = {}) const;

    //--------------------------------------------

    /** Conversion
    */
    operator
    string_view() const noexcept
    {
        return s_;
    }

    /** Conversion
    */
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
    operator
    std::string_view() const noexcept
    {
        return std::string_view(s_);
    }
#endif

    /** Conversion
    */
    operator
    std::string() const noexcept
    {
        return std::string(s_);
    }

    //--------------------------------------------

    // iterator support

    BOOST_CONSTEXPR const_iterator begin() const noexcept
    {
        return s_.begin();
    }

    BOOST_CONSTEXPR const_iterator end() const noexcept
    {
        return s_.end();
    }

    BOOST_CONSTEXPR const_iterator cbegin() const noexcept
    {
        return s_.cbegin();
    }

    BOOST_CONSTEXPR const_iterator cend() const noexcept
    {
        return s_.cend();
    }

#ifdef __cpp_lib_array_constexpr
    constexpr
#endif
    const_reverse_iterator rbegin() const noexcept
    {
        return s_.rbegin();
    }

#ifdef __cpp_lib_array_constexpr
    constexpr
#endif
    const_reverse_iterator rend() const noexcept
    {
        return s_.rend();
    }

#ifdef __cpp_lib_array_constexpr
    constexpr
#endif
    const_reverse_iterator crbegin() const noexcept
    {
        return s_.crbegin();
    }

#ifdef __cpp_lib_array_constexpr
    constexpr
#endif
    const_reverse_iterator crend() const noexcept
    {
        return s_.crend();
    }

    // capacity

    BOOST_CONSTEXPR size_type size() const noexcept
    {
        return s_.size();
    }

    BOOST_CONSTEXPR size_type length() const noexcept
    {
        return s_.length();
    }

    BOOST_CONSTEXPR size_type max_size() const noexcept
    {
        return s_.max_size();
    }

    BOOST_CONSTEXPR bool empty() const noexcept
    {
        return s_.empty();
    }
   
    // element access

    BOOST_CXX14_CONSTEXPR const_reference
        operator[]( size_type pos ) const noexcept
    {
        return s_[pos];
    }

    BOOST_CXX14_CONSTEXPR const_reference
        at( size_type pos ) const
    {
        return s_.at(pos);
    }

    BOOST_CXX14_CONSTEXPR const_reference
        front() const noexcept
    {
        return s_.front();
    }

    BOOST_CXX14_CONSTEXPR const_reference
        back() const noexcept
    {
        return s_.back();
    }

    BOOST_CONSTEXPR const_pointer
        data() const noexcept
    {
        return s_.data();
    }

    // string operations

    BOOST_CXX14_CONSTEXPR size_type copy(
        char* s, size_type n, size_type pos = 0 ) const
    {
        return s_.copy(s, n, pos);
    }

    BOOST_CXX14_CONSTEXPR string_view substr(
        size_type pos = 0, size_type n = string_view::npos ) const
    {
        return s_.substr(pos, n);
    }

    // comparison

    BOOST_CXX14_CONSTEXPR int
        compare( string_view str ) const noexcept
    {
        return s_.compare(str);
    }

    BOOST_CONSTEXPR int compare(
        size_type pos1, size_type n1, string_view str ) const
    {
        return s_.compare(pos1, n1, str);
    }

    BOOST_CONSTEXPR int compare(
        size_type pos1, size_type n1, string_view str,
        size_type pos2, size_type n2 ) const
    {
        return s_.compare(pos1, n1, str, pos2, n2);
    }

    BOOST_CONSTEXPR int compare(
        char const* s ) const noexcept
    {
        return s_.compare(s);
    }

    BOOST_CONSTEXPR int compare(
        size_type pos1, size_type n1, char const* s ) const
    {
        return s_.compare(pos1, n1, s);
    }

    BOOST_CONSTEXPR int compare(
        size_type pos1, size_type n1,
        char const* s, size_type n2 ) const
    {
        return s_.compare(pos1, n1, s, n2);
    }

    // starts_with

    BOOST_CONSTEXPR bool starts_with(
        string_view x ) const noexcept
    {
        return s_.starts_with(x);
    }

    BOOST_CONSTEXPR bool starts_with(
        char x ) const noexcept
    {
        return s_.starts_with(x);
    }

    BOOST_CONSTEXPR bool starts_with(
        char const* x ) const noexcept
    {
        return s_.starts_with(x);
    }

    // ends_with

    BOOST_CONSTEXPR bool ends_with(
        string_view x ) const noexcept
    {
        return s_.ends_with(x);
    }

    BOOST_CONSTEXPR bool ends_with(
        char x ) const noexcept
    {
        return s_.ends_with(x);
    }

    BOOST_CONSTEXPR bool ends_with(
        char const* x ) const noexcept
    {
        return s_.ends_with(x);
    }

    // find

    BOOST_CONSTEXPR size_type find(
        string_view str, size_type pos = 0 ) const noexcept
    {
        return s_.find(str, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type find(
        char c, size_type pos = 0 ) const noexcept
    {
        return s_.find(c, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type find(
        char const* s, size_type pos, size_type n ) const noexcept
    {
        return s_.find(s, pos, n);
    }

    BOOST_CONSTEXPR size_type find(
        char const* s, size_type pos = 0 ) const noexcept
    {
        return s_.find(s, pos);
    }

    // rfind

    BOOST_CONSTEXPR size_type rfind(
        string_view str, size_type pos = string_view::npos ) const noexcept
    {
        return s_.rfind(str, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type rfind(
        char c, size_type pos = string_view::npos ) const noexcept
    {
        return s_.rfind(c, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type rfind(
        char const* s, size_type pos, size_type n ) const noexcept
    {
        return s_.rfind(s, pos, n);
    }

    BOOST_CONSTEXPR size_type rfind(
        char const* s, size_type pos = string_view::npos ) const noexcept
    {
        return s_.rfind(s, pos);
    }

    // find_first_of

    BOOST_CXX14_CONSTEXPR size_type find_first_of(
        string_view str, size_type pos = 0 ) const noexcept
    {
        return s_.find_first_of(str, pos);
    }

    BOOST_CONSTEXPR size_type find_first_of(
        char c, size_type pos = 0 ) const noexcept
    {
        return s_.find_first_of(c, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type find_first_of(
        char const* s, size_type pos, size_type n ) const noexcept
    {
        return s_.find_first_of(s, pos, n);
    }

    BOOST_CXX14_CONSTEXPR size_type find_first_of(
        char const* s, size_type pos = 0 ) const noexcept
    {
        return s_.find_first_of(s, pos);
    }

    // find_last_of

    BOOST_CXX14_CONSTEXPR size_type find_last_of(
        string_view str, size_type pos = string_view::npos ) const noexcept
    {
        return s_.find_last_of(str, pos);
    }

    BOOST_CONSTEXPR size_type find_last_of(
        char c, size_type pos = string_view::npos ) const noexcept
    {
        return s_.find_last_of(c, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type find_last_of(
        char const* s, size_type pos, size_type n ) const noexcept
    {
        return s_.find_last_of(s, pos, n);
    }

    BOOST_CXX14_CONSTEXPR size_type find_last_of(
        char const* s, size_type pos = string_view::npos ) const noexcept
    {
        return s_.find_last_of(s, pos);
    }

    // find_first_not_of

    BOOST_CXX14_CONSTEXPR size_type find_first_not_of(
        string_view str, size_type pos = 0 ) const noexcept
    {
        return s_.find_first_not_of(str, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type find_first_not_of(
        char c, size_type pos = 0 ) const noexcept
    {
        return s_.find_first_not_of(c, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type find_first_not_of(
        char const* s, size_type pos, size_type n ) const noexcept
    {
        return s_.find_first_not_of(s, pos, n);
    }

    BOOST_CXX14_CONSTEXPR size_type find_first_not_of(
        char const* s, size_type pos = 0 ) const noexcept
    {
        return s_.find_first_not_of(s, pos);
    }

    // find_last_not_of

    BOOST_CXX14_CONSTEXPR size_type find_last_not_of(
        string_view str, size_type pos = string_view::npos ) const noexcept
    {
        return s_.find_last_not_of(str, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type find_last_not_of(
        char c, size_type pos = string_view::npos ) const noexcept
    {
        return s_.find_last_not_of(c, pos);
    }

    BOOST_CXX14_CONSTEXPR size_type find_last_not_of(
        char const* s, size_type pos, size_type n ) const noexcept
    {
        return s_.find_last_not_of(s, pos, n);
    }

    BOOST_CXX14_CONSTEXPR size_type find_last_not_of(
        char const* s, size_type pos = string_view::npos ) const noexcept
    {
        return s_.find_last_not_of(s, pos);
    }

    // contains

    BOOST_CONSTEXPR bool contains( string_view sv ) const noexcept
    {
        return s_.contains(sv);
    }

    BOOST_CXX14_CONSTEXPR bool contains( char c ) const noexcept
    {
        return s_.contains(c);
    }

    BOOST_CONSTEXPR bool contains( char const* s ) const noexcept
    {
        return s_.contains(s);
    }

    // relational operators

    template<class S0, class S1>
    using is_match = std::integral_constant<bool,
        std::is_convertible<S0, string_view>::value &&
        std::is_convertible<S1, string_view>::value && (
            std::is_same<typename std::decay<S0>::type,
                pct_string_view>::value ||
            std::is_same<typename std::decay<S1>::type,
                pct_string_view>::value)>;

    template<class S0, class S1>
    BOOST_CXX14_CONSTEXPR friend auto operator==(
        S0 const& s0, S1 const& s1) noexcept ->
        typename std::enable_if<
            is_match<S0, S1>::value, bool>::type
    {
        return string_view(s0) == string_view(s1);
    }

    template<class S0, class S1>
    BOOST_CXX14_CONSTEXPR friend auto operator!=(
        S0 const& s0, S1 const& s1) noexcept ->
        typename std::enable_if<
            is_match<S0, S1>::value, bool>::type
    {
        return string_view(s0) != string_view(s1);
    }

    template<class S0, class S1>
    BOOST_CXX14_CONSTEXPR friend auto operator<(
        S0 const& s0, S1 const& s1) noexcept ->
        typename std::enable_if<
            is_match<S0, S1>::value, bool>::type
    {
        return string_view(s0) < string_view(s1);
    }

    template<class S0, class S1>
    BOOST_CXX14_CONSTEXPR friend auto operator<=(
        S0 const& s0, S1 const& s1) noexcept ->
        typename std::enable_if<
            is_match<S0, S1>::value, bool>::type
    {
        return string_view(s0) <= string_view(s1);
    }

    template<class S0, class S1>
    BOOST_CXX14_CONSTEXPR friend auto operator>(
        S0 const& s0, S1 const& s1) noexcept ->
        typename std::enable_if<
            is_match<S0, S1>::value, bool>::type
    {
        return string_view(s0) > string_view(s1);
    }

    template<class S0, class S1>
    BOOST_CXX14_CONSTEXPR friend auto operator>=(
        S0 const& s0, S1 const& s1) noexcept ->
        typename std::enable_if<
            is_match<S0, S1>::value, bool>::type
    {
        return string_view(s0) >= string_view(s1);
    }

    //--------------------------------------------

    inline friend std::size_t hash_value(
        pct_string_view const& s) noexcept
    {
        return hash_value(s.s_);
    }

    // VFALCO No idea why this fails in msvc
    /*BOOST_CXX14_CONSTEXPR*/ void swap(
        pct_string_view& s ) noexcept
    {
        std::swap(s_, s.s_);
        std::swap(dn_, s.dn_);
    }
};

inline
std::ostream&
operator<<(
    std::ostream& os,
    pct_string_view const& s)
{
    return os << string_view(s);
}

//------------------------------------------------

#ifndef BOOST_URL_DOCS
namespace detail {
inline
string_view&
ref(pct_string_view& s) noexcept
{
    return s.s_;
}

inline
pct_string_view
make_pct_string_view(
    string_view s,
    std::size_t dn) noexcept
{
    return pct_string_view(s, dn);
}
} // detail
#endif

//------------------------------------------------

/** Return a validated percent-encoded string
*/
result<pct_string_view>
make_pct_string_view(
    string_view s) noexcept;

} // urls
} // boost

#endif
