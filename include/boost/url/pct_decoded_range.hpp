//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PCT_DECODED_RANGE_HPP
#define BOOST_URL_PCT_DECODED_RANGE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/pct_encoding.hpp>
#include <type_traits>
#include <iterator>
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
#include <string_view>
#endif

namespace boost {
namespace urls {

/** A view on a string as percent decoded characters

    Objects of this type represent a range of
    characters decoded from a percent encoded
    string. The object has no ownership of the
    underlying character buffer.

    These views are references to the encoded
    strings that allow cheap copies.

    The correctness of the underlying string
    is assumed. The values returned by
    `pct_decoded_range::iterator` when the
    underlying values are an invalid
    percent-encoded string is undefined behavior.

    The function @ref validate_pct_encoding
    can be used to validate the underlying string.

*/
class pct_decoded_range
{
    char const* p_{nullptr};
    std::size_t n_{0};
    std::size_t dn_{0};
    bool plus_to_space_{true};

    using traits_type = std::char_traits<char>;

public:
    /** Type of a decoded character
    */
    using value_type = char;

    /// @copydoc value_type
    using reference = char;

    /// @copydoc value_type
    using const_reference = char;

    /** The unsigned integer type used to represent size.
    */
    using size_type = std::size_t;

    /** The signed integer type used to represent differences.
    */
    using difference_type = std::ptrdiff_t;

    /** A read-only bidirectional iterator to the decoded range.

        This is a read-only bidirectional iterator to
        the decoded characters.

     */
#ifdef BOOST_URL_DOCS
    using const_iterator = __see_below__;
#else
    class const_iterator;
#endif

    /// @copydoc const_iterator
    using iterator = const_iterator;

    /** Constructor

        Default-constructed objects represent
        the empty string.

     */
    BOOST_CONSTEXPR
    pct_decoded_range(
        bool plus_to_space = true) noexcept
        : p_(nullptr)
        , n_(0)
        , dn_(0)
        , plus_to_space_(plus_to_space)
    {}

    /** Constructor
     */
    pct_decoded_range(
        char const* str,
        bool plus_to_space = true) noexcept
        : p_(str)
        , n_(traits_type::length(str))
        , dn_(pct_decode_bytes_unchecked(string_view(str)))
        , plus_to_space_(plus_to_space)
    {}

    /** Constructor
     */
    pct_decoded_range(
        char const* str,
        size_type len,
        bool plus_to_space = true) noexcept
        : p_(str)
        , n_(len)
        , dn_(pct_decode_bytes_unchecked(string_view(str, len)))
        , plus_to_space_(plus_to_space)
    {}

    /** Constructor
     */
#ifndef BOOST_URL_DOCS
    template <
        class End,
        typename std::enable_if<std::is_same<End, char const*>::value, int>::type = 0>
    BOOST_CXX14_CONSTEXPR
    pct_decoded_range(
        char const* first,
        End last,
        bool plus_to_space = true) noexcept
#else
    BOOST_CXX14_CONSTEXPR
    pct_decoded_range(
        char const* first,
        char const* last,
        bool plus_to_space = true) noexcept
#endif
        : p_(first)
        , n_(last - first)
        , dn_(pct_decode_bytes_unchecked(string_view(first, last)))
        , plus_to_space_(plus_to_space)
    {
        BOOST_ASSERT(last - first >= 0);
    }

    /** Constructor
     */
    pct_decoded_range(
        urls::string_view str,
        bool plus_to_space = true) noexcept
        : p_(str.data())
        , n_(str.size())
        , dn_(pct_decode_bytes_unchecked(str))
        , plus_to_space_(plus_to_space)
    {}

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
    /** Constructor
     */
    pct_decoded_range(
        std::string_view str,
        bool plus_to_space = true) noexcept
        : p_( str.data() )
        , n_( str.size() )
        , dn_(pct_decode_bytes_unchecked(str))
        , plus_to_space_(plus_to_space)
    {}
#endif

    /** Returns an iterator to the beginning

        Returns an iterator to the first decoded character of the string

        @return const_iterator to the first character
    */
    BOOST_URL_DECL
    const_iterator
    begin() const noexcept;

    /// @copydoc begin()
    BOOST_URL_DECL
    const_iterator
    cbegin() const noexcept;

    /** Returns an iterator to the end

        Returns an iterator to one past the last decoded character of the string.

        This character acts as a placeholder, attempting to access it
        results in undefined behavior.

        @return const_iterator to the character following the last character
    */
    BOOST_URL_DECL
    const_iterator
    end() const noexcept;

    /// @copydoc end()
    BOOST_URL_DECL
    const_iterator
    cend() const noexcept;

    /** Accesses the first character

        Returns the decoded first character in the string.

        The behavior is undefined if `empty() == true`.

        @return reference to the first character
     */
    BOOST_URL_DECL
    const_reference
    front() const;

    /** Accesses the last character

        Returns the last decoded character in the string

        The behavior is undefined if `empty() == true`.

        @return reference to the first character
     */
    BOOST_URL_DECL
    const_reference
    back() const;

    /** Returns a pointer to the first encoded character of a string

        Returns a pointer to the underlying character array.

        The pointer is such that the range `[data(); data() + encoded_size())`
        is valid and the values in it correspond to the values of the
        string.

        @note Unlike `std::basic_string::data()` and string literals,
        `data()` may return a pointer to a buffer that is not
        null-terminated. Therefore, it is typically a mistake to pass
        data() to a routine that takes just a const char* and expects
        a null-terminated string.

        @return A pointer to the underlying character array.
     */
    BOOST_CONSTEXPR
    const char*
    encoded_data() const
    {
        return p_;
    }

    /** Returns the number of char elements in the string

        @return The number of char elements in the string
    */
    BOOST_CONSTEXPR
    size_type
    size() const noexcept
    {
        return dn_;
    }

    /// @copydoc size()
    BOOST_CONSTEXPR
    size_type
    length() const noexcept
    {
        return dn_;
    }

    /** Returns the number of encoded char elements in the string

        @return The number of encoded char elements in the string
    */
    BOOST_CONSTEXPR
    size_type
    encoded_size() const noexcept
    {
        return n_;
    }

    /// @copydoc encoded_size()
    BOOST_CONSTEXPR
    size_type
    encoded_length() const noexcept
    {
        return n_;
    }

    /** Returns the maximum number of characters

        The largest possible number of decoded
        chars objects that can be referred to by
        a pct_decoded_range.

        @return Maximum number of decoded characters
    */
    BOOST_CONSTEXPR
    size_type
    max_size() const noexcept
    {
        return size_type( -1 );
    }

    /** Checks whether the string is empty

        Checks if the string has no characters,
        i.e. whether `size() == 0`.

        @return `true` if the string is empty
    */
    BOOST_CONSTEXPR
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

    /** Copies characters

        Copies a substring to the character array pointed to by
        the destination char string, where `rcount` is the smaller
        of `count` and `size() - pos`.

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range If `pos > size()`

        @param dest pointer to the destination character string
        @param count requested substring length
        @param pos position of the first character

        @return Number of characters copied
    */
    BOOST_URL_DECL
    size_type
    copy(char* dest,
         size_type count,
         size_type pos = 0) const;

    /** Compares two strings

        The length of the sequences to compare is the smaller of
        `size()` and `other.size()`.

        The function compares the two strings as if by calling
        `char_traits<char>::compare(to_string().data(), v.data(), rlen)`.

        @param other string to compare

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_DECL
    int
    compare(string_view other) const;

    /// @copydoc compare(string_view)
    int
    compare(char const* other) const
    {
        return compare(string_view(other));
    }

    /** Compares two strings

        The length of the sequences to compare is the smaller of
        `size()` and `other.size()`.

        The function compares the two strings as if by calling
        `char_traits<char>::compare(to_string().data(), v.to_string().data(), rlen)`.

        @param other string to compare

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_DECL
    int
    compare(pct_decoded_range other) const;

    /** Returns a basic_string

        This overload allows the caller to recycle
        the input string.

        @return A string representing the
        entire contents of the decoded range.
    */
    template <class A>
    std::basic_string<char, std::char_traits<char>, A>&
    to_string(std::basic_string<char, std::char_traits<char>, A> & str) const
    {
        str.resize(size());
        copy(&str[0], size());
        return str;
    }

    /** Returns a basic_string

        This overload constructs a new string.

        @return A string representing the
        entire contents of the decoded range.
    */
    template <class A = std::allocator<char>>
    std::basic_string<char, std::char_traits<char>, A>
    to_string() const
    {
        std::basic_string<
            char, std::char_traits<char>, A> str(size(), ' ');
        return to_string(str);
    }

    /** Returns a basic_string

        @return A string representing the
        entire contents of the decoded range.
    */
    template <class A>
    explicit
    operator std::basic_string<char, std::char_traits<char>, A>() const
    {
        return to_string<A>();
    }

    /** Returns a basic_string

        This overload allows the caller to recycle
        the input string.

        @return A string representing the
        entire contents of the decoded range.
    */
    template <class A>
    std::basic_string<char, std::char_traits<char>, A>&
    append_to(std::basic_string<char, std::char_traits<char>, A> & str) const;

    /** Compares two decoded strings

        @param lhs A string to compare
        @param rhs A string to compare

        @return true if the corresponding comparison holds
      */
    friend
    bool
    operator==( pct_decoded_range lhs,
                pct_decoded_range rhs ) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    /// @copydoc operator==
    friend
    bool
    operator==( pct_decoded_range lhs,
                string_view rhs ) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    /// @copydoc operator==
    friend
    bool
    operator==( string_view lhs,
                pct_decoded_range rhs ) noexcept
    {
        return rhs.compare(lhs) == 0;
    }

    /** Compares two strings

        @param lhs A string to compare
        @param rhs A string to compare

        @return true if the corresponding comparison holds
      */
    friend
    bool
    operator!=( pct_decoded_range lhs,
                pct_decoded_range rhs ) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    /// @copydoc operator!=
    friend
    bool
    operator!=( pct_decoded_range lhs,
                string_view rhs ) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    /// @copydoc operator!=
    friend
    bool
    operator!=( string_view lhs,
                pct_decoded_range rhs ) noexcept
    {
        return rhs.compare(lhs) != 0;
    }

    /** Compares two strings

        @param lhs A string to compare
        @param rhs A string to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator<( pct_decoded_range lhs,
               pct_decoded_range rhs ) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    /// @copydoc operator<
    friend
    bool
    operator<( pct_decoded_range lhs,
               string_view rhs ) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    /// @copydoc operator<
    friend
    bool
    operator<( string_view lhs,
               pct_decoded_range rhs ) noexcept
    {
        return rhs.compare(lhs) > 0;
    }

    /** Compares two strings

        @param lhs A string to compare
        @param rhs A string to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator<=( pct_decoded_range lhs,
                pct_decoded_range rhs ) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    /// @copydoc operator<=
    friend
    bool
    operator<=( pct_decoded_range lhs,
                string_view rhs ) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    /// @copydoc operator<=
    friend
    bool
    operator<=( string_view lhs,
                pct_decoded_range rhs ) noexcept
    {
        return rhs.compare(lhs) >= 0;
    }

    /** Compares two strings

        @param lhs A string to compare
        @param rhs A string to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator>( pct_decoded_range lhs,
               pct_decoded_range rhs ) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    /// @copydoc operator>
    friend
    bool
    operator>( pct_decoded_range lhs,
               string_view rhs ) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    /// @copydoc operator>
    friend
    bool
    operator>( string_view lhs,
               pct_decoded_range rhs ) noexcept
    {
        return rhs.compare(lhs) < 0;
    }

    /** Compares two strings

        @param lhs A string to compare
        @param rhs A string to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator>=( pct_decoded_range lhs,
                pct_decoded_range rhs ) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    /// @copydoc operator>=
    friend
    bool
    operator>=( pct_decoded_range lhs,
                string_view rhs ) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    /// @copydoc operator>=
    friend
    bool
    operator>=( string_view lhs,
                pct_decoded_range rhs ) noexcept
    {
        return rhs.compare(lhs) <= 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
      */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator==( pct_decoded_range const& lhs,
                StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) == 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
      */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator!=( pct_decoded_range const& lhs,
                StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) != 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator<( pct_decoded_range const& lhs,
               StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) < 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator<=( pct_decoded_range const& lhs,
                StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) <= 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator>( pct_decoded_range const& lhs,
               StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) > 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator>=( pct_decoded_range const& lhs,
                StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) >= 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
      */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator==( StringViewable const& lhs,
                pct_decoded_range const& rhs ) noexcept
    {
        return rhs.compare(lhs) == 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
      */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator!=( StringViewable const& lhs,
                pct_decoded_range const& rhs ) noexcept
    {
        return rhs.compare(lhs) != 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator<( StringViewable const& lhs,
               pct_decoded_range const& rhs ) noexcept
    {
        return rhs.compare(lhs) > 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator<=( StringViewable const& lhs,
                pct_decoded_range const& rhs ) noexcept
    {
        return rhs.compare(lhs) >= 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator>( StringViewable const& lhs,
               pct_decoded_range const& rhs ) noexcept
    {
        return rhs.compare(lhs) < 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator>=( StringViewable const& lhs,
                pct_decoded_range const& rhs ) noexcept
    {
        return rhs.compare(lhs) <= 0;
    }

    friend BOOST_URL_DECL
    std::ostream&
    operator<<(std::ostream& os,
               pct_decoded_range const& cs);
};

} // urls
} // boost

#include <boost/url/impl/pct_decoded_range.hpp>

#endif
