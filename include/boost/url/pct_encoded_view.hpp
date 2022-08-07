//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PCT_ENCODED_VIEW_HPP
#define BOOST_URL_PCT_ENCODED_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/pct_encoding.hpp>
#include <boost/url/detail/pct_encoded_view.hpp>
#include <type_traits>
#include <iterator>
#include <iosfwd>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
namespace grammar {
namespace detail {
class copied_strings_base;
} // detail
} // grammar
#endif

//------------------------------------------------

/** A reference to a valid, percent-encoded string

    These views reference strings in parts of URLs
    or other components that are percent-encoded.
    The special characters (those not in the
    allowed character set) are stored as three
    character escapes that consist of a percent
    sign ('%%') followed by a two-digit hexadecimal
    number of the corresponding unescaped character
    code, which may be part of a UTF-8 code point
    depending on the context.

    The view refers to the original character
    buffer and only decodes escaped sequences when
    needed. In particular these operations perform
    percent-decoding automatically without the
    need to allocate memory:

    @li Iteration of the string
    @li Accessing the encoded character buffer
    @li Comparison to encoded or plain strings

    However, in order to access the string as a
    as a contiguous character buffer with
    with percent-decoding applied, the caller
    must explicitly opt-in to an operation that
    is potentially allocating. These operations
    are:

    @li Conversion to `std::string`
    @li Appending to an existing character buffer
    @li Assigning to an existing character buffer

    These objects can only be constructed from
    strings that have a valid percent-encoding,
    otherwise construction fails. The caller is
    responsible for ensuring that the lifetime
    of the character buffer from which the view
    is constructed extends unmodified until the
    view is no longer accessed.
*/
class pct_encoded_view
{
    char const* p_ = nullptr;
    std::size_t n_ = 0;
    std::size_t dn_ = 0;
    bool plus_to_space_ = true;

    using traits_type = std::char_traits<char>;

    friend detail::access;
    friend class url_base;

    // unchecked constructor
    BOOST_URL_DECL
    explicit
    pct_encoded_view(
        string_view s,
        std::size_t n,
        pct_decode_opts opt = {}) noexcept;

    /** Return a view whose buffer does not overlap, copying if necessary
    */
    BOOST_URL_DECL
    pct_encoded_view
    maybe_copy(
        grammar::detail::copied_strings_base& sp) const;

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

    /** An iterator of constant, decoded characters.

        This iterator is used to access the encoded
        string as a bidirectional range of characters
        with percent-decoding applied. Escape sequences
        are not decoded until the iterator is
        dereferenced.
    */
#ifdef BOOST_URL_DOCS
    using iterator = __see_below__;
#else
    class iterator;
#endif

    /// @copydoc iterator
    using const_iterator = iterator;

    /** Constructor

        Default-constructed objects represent
        the empty string.
    */
    pct_encoded_view() noexcept = default;

    /** Constructor

        This constructs a view from the character
        buffer `s`, which must remain valid and
        unmodified until the view is no longer
        accessed.

        @par Preconditions
        `s` contains a valid percent-encoded string.

        @par Complexity
        Linear in `s.size()`.

        @param s The string

        @param opt A set of options which may
        be applied to the decoding algorithm.
        If this argument is left out, default
        settings will be used.

        @throw std::invalid_argument The percent-encoded is invalid
    */
    BOOST_URL_DECL
    explicit
    pct_encoded_view(
        string_view s,
        pct_decode_opts opt = {}) noexcept;

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

    /** Return an iterator to the beginning
    */
    const_iterator
    begin() const noexcept;

    /** Return an iterator to the end
    */
    const_iterator
    end() const noexcept;

    //--------------------------------------------
    //
    // Element Access
    //
    //--------------------------------------------

    /** Return the last decoded character

        @par Preconditions
        @code
        not empty()
        @endcode
    */
    const_reference
    back() const noexcept;

    /** Return the first decoded character

        @par Preconditions
        @code
        not empty()
        @endcode
    */
    const_reference
    front() const noexcept;

    /** Return the underlying encoded character buffer

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    string_view
    encoded() const noexcept
    {
        return {p_, n_};
    }

    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

    /** Return true if the view is empty
    */
    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    /** Return the number of decoded characters

        This function returns the number of characters
        that would be produced if percent-decoding was
        applied to the character buffer.

        @par Complexity
        Constant,

        @par Exception Safety
        Throws nothing.
    */
    size_type
    size() const noexcept
    {
        return dn_;
    }

    /** Return the decode options for this view
     */
    pct_decode_opts
    options() const noexcept
    {
        pct_decode_opts opt;
        opt.plus_to_space = plus_to_space_;
        return opt;
    }

    /** Copy a decoded substring to another character string

        This function copies a substring to the
        character array pointed to by the
        destination char string, where `rcount`
        is the smaller of `count` and
        `size() - pos`.

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @par Preconditions
        @code
        pos > size()
        @endcode

        @throw std::out_of_range `pos > size()`

        @param dest pointer to the destination character string
        @param count requested substring length
        @param pos position of the first character

        @return Number of characters copied
    */
    BOOST_URL_DECL
    size_type
    copy(
        char* dest,
        size_type count,
        size_type pos = 0) const;

    /** Append the range with percent-decoded applied to an existing string

        This function applies percent-decoding to each character
        in the referenced buffer and appends it to `s` which
        must satisfy the requirements of <em>MutableString</em>.
        In particular this expression must be valid:
        @code
        s.append( this->begin(), this->end() );
        @endcode

        Depending on the implementation of `MutableString`
        this allows the caller to recycle capacity that
        resides in an already-existing container when
        applying percent-decoding, as shown in this example:

        @par Example
        @code
        void f( pct_encoded_view s )
        {
            thread_local static std::string tmp;

            // Existing capacity of `tmp` will be reused first.
            // If this function is called repeatedly, then the
            // following three lines will almost never perform a
            // memory allocation:

            tmp = "The decoded value is '";
            s.append_to( tmp );
            tmp += "'\n";

            std::cout << tmp;
        }
        @endcode

        @par Mandates
        @code
        is_mutable_string_v< MutableString >
        @endcode

        @par Complexity
        Linear in `this->size()`, plus `s.append( this->begin(), this->end() )`.

        @return A string representing the
        entire contents of the decoded range.
    */
    template <class MutableString>
    MutableString&
    append_to(
        MutableString& s) const;

    /** Assign the range with percent-decoded applied to an existing string

        This function applies percent-decoding to each character
        in the referenced buffer and assigns it to `s` which
        must satisfy the requirements of <em>MutableString</em>.
        In particular this expression must be valid:
        @code
        s.assign( this->begin(), this->end() );
        @endcode

        @par Example
        @code
        void f( pct_encoded_view s )
        {
            thread_local static std::string tmp;

            // Existing capacity of `tmp` will be reused first.
            // If this function is called repeatedly, then the
            // following line will almost never perform a
            // memory allocation:

            s.assign_to( tmp );

            std::cout << tmp << "\n";
        }
        @endcode

        @par Mandates
        @code
        is_mutable_string_v< MutableString >
        @endcode

        Depending on the implementation of `MutableString`
        this allows the caller to recycle capacity that
        resides in an already-existing container when
        applying percent-decoding, as shown in this example:

        @par Complexity
        Linear in `this->size()`, plus `s.assign( this->begin(), this->end() )`.

        @return A string representing the
        entire contents of the decoded range.
    */
    template <class MutableString>
    MutableString&
    assign_to(
        MutableString& s) const;

    /** Return a std::string with percent-decoding applied

        This function applies percent-decoding to the
        referenced character buffer, returning the result
        as a `std::string`.

        @par Complexity
        Linear in `size()`.

        @par Exception Safety
        Memory allocations may throw.
    */
    std::string
    to_string() const
    {
        std::string r;
        assign_to(r);
        return r;
    }

    /// @copydoc to_string()
    explicit
    operator std::string() const
    {
        return to_string();
    }

    //--------------------------------------------
    //
    // Comparison
    //
    //--------------------------------------------

    /** Return the result of comparing to another string

        The length of the sequences to compare is the smaller of
        `size()` and `other.size()`.

        The function compares the two strings as if by calling
        `char_traits<char>::compare(to_string().data(), v.data(), rlen)`.
        This means the comparison is performed with
        percent-decoding applied to the current string.

        @param other string to compare

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_DECL
    int
    compare(string_view other) const noexcept;

    /** Return the result of comparing to another string

        The length of the sequences to compare is the smaller of
        `size()` and `other.size()`.

        The function compares the two strings as if by calling
        `char_traits<char>::compare(to_string().data(), v.to_string().data(), rlen)`.
        This means the comparison is performed with
        percent-decoding applied to the current string.

        @param other string to compare

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_DECL
    int
    compare(pct_encoded_view other) const noexcept;

    //--------------------------------------------

    /** Return the result of comparing s0 and s1

        This function compares the two strings that
        result after applying percent-decoding to
        `s0` and `s1`. Does not allocate.

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    friend
    bool
    operator==(
        pct_encoded_view s0,
        pct_encoded_view s1) noexcept
    {
        return s0.compare(s1) == 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the two strings that
        result after applying percent-decoding to
        `s0` and `s1`. Does not allocate.

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    friend
    bool
    operator!=(
        pct_encoded_view s0,
        pct_encoded_view s1) noexcept
    {
        return s0.compare(s1) != 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the two strings that
        result after applying percent-decoding to
        `s0` and `s1`. Does not allocate.

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    friend
    bool
    operator<(
        pct_encoded_view s0,
        pct_encoded_view s1) noexcept
    {
        return s0.compare(s1) < 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the two strings that
        result after applying percent-decoding to
        `s0` and `s1`. Does not allocate.

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    friend
    bool
    operator<=(
        pct_encoded_view s0,
        pct_encoded_view s1) noexcept
    {
        return s0.compare(s1) <= 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the two strings that
        result after applying percent-decoding to
        `s0` and `s1`. Does not allocate.

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    friend
    bool
    operator>(
        pct_encoded_view s0,
        pct_encoded_view s1) noexcept
    {
        return s0.compare(s1) > 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the two strings that
        result after applying percent-decoding to
        `s0` and `s1`. Does not allocate.

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    friend
    bool
    operator>=(
        pct_encoded_view s0,
        pct_encoded_view s1) noexcept
    {
        return s0.compare(s1) >= 0;
    }

    //--------------------------------------------

    /** Return the result of comparing s0 and s1

        This function compares the string that
        results from applying percent-decoding to
        `s0`, to the string `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator==(
        pct_encoded_view s0,
        String const& s1) noexcept
    {
        return s0.compare(string_view(s1)) == 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string that
        results from applying percent-decoding to
        `s0`, to the string `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator!=(
        pct_encoded_view s0,
        String const& s1) noexcept
    {
        return s0.compare(string_view(s1)) != 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string that
        results from applying percent-decoding to
        `s0`, to the string `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator<(
        pct_encoded_view s0,
        String const& s1) noexcept
    {
        return s0.compare(string_view(s1)) < 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string that
        results from applying percent-decoding to
        `s0`, to the string `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator<=(
        pct_encoded_view s0,
        String const& s1) noexcept
    {
        return s0.compare(string_view(s1)) <= 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string that
        results from applying percent-decoding to
        `s0`, to the string `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator>(
        pct_encoded_view s0,
        String const& s1) noexcept
    {
        return s0.compare(string_view(s1)) > 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string that
        results from applying percent-decoding to
        `s0`, to the string `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator>=(
        pct_encoded_view s0,
        String const& s1) noexcept
    {
        return s0.compare(string_view(s1)) >= 0;
    }

    //--------------------------------------------

    /** Return the result of comparing s0 and s1

        This function compares the string `s0`, to
        the string that results from applying
        percent-decoding to `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator==(
        String const& s0,
        pct_encoded_view s1) noexcept
    {
        return s1.compare(string_view(s0)) == 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string `s0`, to
        the string that results from applying
        percent-decoding to `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator!=(
        String const& s0,
        pct_encoded_view s1) noexcept
    {
        return s1.compare(string_view(s0)) != 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string `s0`, to
        the string that results from applying
        percent-decoding to `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator<(
        String const& s0,
        pct_encoded_view s1) noexcept
    {
        return s1.compare(string_view(s0)) > 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string `s0`, to
        the string that results from applying
        percent-decoding to `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator<=(
        String const& s0,
        pct_encoded_view s1) noexcept
    {
        return s1.compare(string_view(s0)) >= 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string `s0`, to
        the string that results from applying
        percent-decoding to `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator>(
        String const& s0,
        pct_encoded_view s1) noexcept
    {
        return s1.compare(string_view(s0)) < 0;
    }

    /** Return the result of comparing s0 and s1

        This function compares the string `s0`, to
        the string that results from applying
        percent-decoding to `s1`. Does not allocate.

        @par Constraints
        @code
        std::is_convertible_v< String, string_view >
        @endcode

        @par Complexity
        Linear in `s0.size() + s1.size()`.

        @par Exception Safety
        Throws nothing.
    */
    template <class String>
    friend
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_convertible<
            String, string_view>::value &&
        ! std::is_same<typename
            std::decay<String>::type,
            pct_encoded_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator>=(
        String const& s0,
        pct_encoded_view s1) noexcept
    {
        return s1.compare(string_view(s0)) <= 0;
    }

    //--------------------------------------------

    // hidden friend
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        pct_encoded_view const& s)
    {
        // hidden friend
        s.write(os);
        return os;
    }

private:
    BOOST_URL_DECL
    void
    write(std::ostream& os) const;
};

/** Format the string with percent-decoding applied to the output stream

    This function serializes the decoded view
    to the output stream.

    @return A reference to the output stream, for chaining

    @param os The output stream to write to

    @param s The decoded view to write
*/
inline
std::ostream&
operator<<(
    std::ostream& os,
    pct_encoded_view const& s);

} // urls

//------------------------------------------------

#ifndef BOOST_URL_DOCS
namespace filesystem {
namespace path_traits {
template <class T>
struct is_pathable;

template <>
struct is_pathable<urls::pct_encoded_view>
{
    static const bool value = true;
};
} // path_traits
} // filesystem
#endif

} // boost

#include <boost/url/impl/pct_encoded_view.hpp>

#endif
