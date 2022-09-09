//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DECODE_VIEW_HPP
#define BOOST_URL_DECODE_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/decode_opts.hpp>
#include <boost/url/detail/decode_view.hpp>
#include <type_traits>
#include <iterator>
#include <iosfwd>

namespace boost {
namespace urls {

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
class decode_view
{
    char const* p_ = nullptr;
    std::size_t n_ = 0;
    std::size_t dn_ = 0;
    bool plus_to_space_ = true;

    friend struct detail::make_decode_view_t;

    friend detail::access;
    friend class url_base;

    // unchecked constructor
    BOOST_URL_DECL
    explicit
    decode_view(
        string_view s,
        std::size_t n,
        decode_opts opt = {}) noexcept;

public:
    /** The value type
    */
    using value_type = char;

    /** The reference type
    */
    using reference = char;

    /// @copydoc reference
    using const_reference = char;

    /** The unsigned integer type
    */
    using size_type = std::size_t;

    /** The signed integer type
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

    //--------------------------------------------
    //
    // Special Members
    //
    //--------------------------------------------

    /** Constructor

        Default-constructed views represent
        empty strings.

        @par Example
        @code
        decode_view ds;
        @endcode

        @par Postconditions
        @code
        this->empty() == true
        @endcode

        @par Exception Safety
        Throws nothing.
    */
    decode_view() noexcept = default;

    /** Constructor

        This constructs a view from the character
        buffer `s`, which must remain valid and
        unmodified until the view is no longer
        accessed.

        @par Example
        @code
        decode_view ds( "Program%20Files" );
        @endcode

        @par Postconditions
        @code
        this->encoded() == s
        @endcode

        @par Complexity
        Linear in `s.size()`.

        @par Exception Safety
        Exceptions thrown on invalid input.

        @param s The encoded string.

        @param opt The options for decoding. If
        this parameter is omitted, the default
        options will be used.

        @throw std::invalid_argument `s` is
        not a valid percent-encoded string.
    */
    BOOST_URL_DECL
    explicit
    decode_view(
        string_view s,
        decode_opts opt = {});

    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

    /** Return true if the string is empty

        @par Example
        @code
        assert( decode_view( "" ).empty() );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

    /** Return the number of decoded characters

        @par Example
        @code
        assert( decode_view( "Program%20Files" ).size() == 13 );
        @endcode

        @par Effects
        @code
        return std::distance( this->begin(), this->end() );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    size_type
    size() const noexcept
    {
        return dn_;
    }

    /** Return an iterator to the beginning

        @par Example
        @code
        auto it = this->begin();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    iterator
    begin() const noexcept;

    /** Return an iterator to the end

        @par Example
        @code
        auto it = this->end();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    iterator
    end() const noexcept;

    /** Return the first character

        @par Example
        @code
        assert( decode_view( "Program%20Files" ).front() == 'P' );
        @endcode

        @par Preconditions
        @code
        not this->empty()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    reference
    front() const noexcept;

    /** Return the last character

        @par Example
        @code
        assert( decode_view( "Program%20Files" ).back() == 's' );
        @endcode

        @par Preconditions
        @code
        not this->empty()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    reference
    back() const noexcept;

    /** Return the encoded string

        @par Example
        @code
        assert( decode_view( "Program%20Files" ).encoded() == "Program%20Files" );
        @endcode

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

    /** Return the decoding options
    */
    decode_opts
    options() const noexcept
    {
        decode_opts opt;
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
        void f( decode_view s )
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
        void f( decode_view s )
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
    compare(decode_view other) const noexcept;

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
        decode_view s0,
        decode_view s1) noexcept
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
        decode_view s0,
        decode_view s1) noexcept
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
        decode_view s0,
        decode_view s1) noexcept
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
        decode_view s0,
        decode_view s1) noexcept
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
        decode_view s0,
        decode_view s1) noexcept
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
        decode_view s0,
        decode_view s1) noexcept
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator==(
        decode_view s0,
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator!=(
        decode_view s0,
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator<(
        decode_view s0,
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator<=(
        decode_view s0,
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator>(
        decode_view s0,
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator>=(
        decode_view s0,
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator==(
        String const& s0,
        decode_view s1) noexcept
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator!=(
        String const& s0,
        decode_view s1) noexcept
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator<(
        String const& s0,
        decode_view s1) noexcept
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator<=(
        String const& s0,
        decode_view s1) noexcept
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator>(
        String const& s0,
        decode_view s1) noexcept
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
            decode_view>::value,
        bool
    >::type
#else
    bool
#endif
    operator>=(
        String const& s0,
        decode_view s1) noexcept
    {
        return s1.compare(string_view(s0)) <= 0;
    }

    //--------------------------------------------

    // hidden friend
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        decode_view const& s)
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
    decode_view const& s);

} // urls
} // boost

#include <boost/url/impl/decode_view.hpp>

#endif
