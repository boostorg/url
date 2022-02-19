//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2019 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_CONST_STRING_HPP
#define BOOST_URL_CONST_STRING_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/except.hpp>
#ifndef BOOST_URL_NO_CXX17_HDR_STRING_VIEW
#include <string_view>
#endif

namespace boost {
namespace urls {

/** A constant string with shared ownership

    Objects of this type represent read-only
    strings with ownership of the character
    buffer.

    They are reference counted to allow cheap copies.

    The type is derived from `string_view`,
    which provides compatibility with strings
    in terms of comparisons and conversions.
*/
class const_string
{
    static constexpr
    std::size_t
    builtin_capacity = 32;

    const char* str_{nullptr};
    std::size_t len_{0};

    struct base;
    struct result;

    union data_t
    {
        base* p_;
        char buf_[builtin_capacity];

        data_t() = default;

        explicit
        data_t(base* p)
            : p_(p)
        {
        }
    };

    data_t data_;

    static constexpr
    bool
    is_small(std::size_t n) noexcept
    {
        return n <= builtin_capacity;
    }

    BOOST_URL_DECL
    explicit
    const_string(
        result const& r) noexcept;

public:
    class factory;

    using value_type = char;
    using pointer = char*;
    using const_pointer = const char*;
    using reference = char&;
    using const_reference = const char&;
    using const_iterator = const char*;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr size_type npos = size_type(-1);

    /** Destructor

        Releases ownership of the string buffer.
        If this is the last owner, the storage
        is deallocated.
    */
    BOOST_URL_DECL
    ~const_string();

    /** Constructor

        Default-constructed objects represent
        the empty string.
    */
    BOOST_URL_DECL
    const_string() noexcept;

    /** Constructor

        Copies do not allocate. The newly
        constructed string acquires shared
        ownership of the underlying character
        buffer.

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    const_string(
        const_string const& other) noexcept;

    /** Assignment

        Assignment does not allocate. The newly
        assigned string acquires shared
        ownership of the underlying character
        buffer.
    */
    BOOST_URL_DECL
    const_string&
    operator=(
        const_string const& other) noexcept;

    /** Constructor

        Constructs a string with the specified
        size. The function object `init` is
        used to initialize the contents of
        the string, and will be invoked with
        this equivalent signature:

        @code
        void (std::size_t size, char* dest );
        @endcode

        @param size The size of the resulting string

        @param a The allocator to use.

        @param init The function object to invoke.
    */
    template<
        class Allocator,
        class InitFn>
    const_string(
        std::size_t size,
        Allocator const& a,
        InitFn const& init);

    /** Constructor

        Constructs a copy of the string `s`.
        If memory allocation is required, the
        specified allocator `a` will be used.

        @param s The string to copy

        @param a The allocator to use.
    */
    template<class Allocator>
    const_string(
        string_view s,
        Allocator const& a);

   /** Constructor

        Constructs a copy of the string `str`.
        If memory allocation is required, the
        specified allocator `a` will be used.

        @param str The string to copy

        @param a The allocator to use.
    */
    template<class Allocator>
    const_string(
        char const* str,
        Allocator const& a)
    : const_string(string_view(str), a)
    {
    }

    /** Constructor

        Constructs a copy of the string `str`.
        If memory allocation is required, the
        specified allocator `a` will be used.

        @param str The string to copy

        @param a The allocator to use.
    */
    template<class Allocator>
    const_string(
        char const* str,
        size_type len,
        Allocator const& a)
    : const_string(string_view(str, len), a)
    {
    }

#if !defined(BOOST_URL_NO_CXX17_HDR_STRING_VIEW)
    /** Constructor

        Constructs a copy of the string `s`.
        If memory allocation is required, the
        specified allocator `a` will be used.

        @param s The string to copy

        @param a The allocator to use.
    */
    template<class Allocator>
    const_string(
        std::string_view s,
        Allocator const& a)
    : const_string(string_view(s.data(), s.size()), a)
    {
    }
#endif

    /** Constructor

        Constructs a copy of the string `s`.
        If memory allocation is required, the
        specified allocator `a` will be used.

        @param s The string to copy

        @param a The allocator to use.
    */
    template<class Alloc1, class Alloc2>
    const_string(
        const std::basic_string<
            char, std::char_traits<char>, Alloc1
        >& s,
        Alloc2 const& a)
    : const_string(string_view(s.data(), s.size()), a)
    {
    }

    /** Returns an iterator to the beginning

        Returns an iterator to the first character of the string

        @return const_iterator to the first character
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_iterator
    begin() const noexcept
    {
        return str_;
    }

    /** Returns an iterator to the beginning

        Returns an iterator to the first character of the string

        @return const_iterator to the first character
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_iterator
    cbegin() const noexcept
    {
        return str_;
    }

    /** Returns an iterator to the end

        Returns an iterator to one past the last character of the string.

        This character acts as a placeholder, attempting to access it
        results in undefined behavior.

        @return const_iterator to the character following the last character
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_iterator
    end() const noexcept
    {
        return str_ + len_;
    }

    /** Returns an iterator to the end

        Returns an iterator to one past the last character of the string.

        This character acts as a placeholder, attempting to access it
        results in undefined behavior.

        @return const_iterator to the character following the last character
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_iterator
    cend() const noexcept
    {
        return str_ + len_;
    }

    /** Returns a reverse iterator to the beginning

        Returns an iterator to the first char of the reversed string

        It corresponds to the last character of the non-reversed string

        @return const_reverse_iterator to the first character
    */
    inline
    const_reverse_iterator
    rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    /** Returns a reverse iterator to the beginning

        Returns an iterator to the first char of the reversed string

        It corresponds to the last character of the non-reversed string

        @return const_reverse_iterator to the first character
    */
    inline
    const_reverse_iterator
    crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    /** Returns a reverse iterator to the end

        Returns a reverse iterator to the character following the
        last character of the reversed string.

        It corresponds to the character preceding the first character
        of the non-reversed string.

        This character acts as a placeholder, attempting to access
        it results in undefined behavior.

        @return const_reverse_iterator to the character following
        the last character
    */
    inline
    const_reverse_iterator
    rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    /** Returns a reverse iterator to the end

        Returns a reverse iterator to the character following the
        last character of the reversed string.

        It corresponds to the character preceding the first character
        of the non-reversed string.

        This character acts as a placeholder, attempting to access
        it results in undefined behavior.

        @return const_reverse_iterator to the character following
        the last character
    */
    inline
    const_reverse_iterator
    crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    /** Accesses the specified character

        Returns a const reference to the character at specified
        location pos.

        No bounds checking is performed: the behavior is
        undefined if `pos >= size()`

        @note Unlike `std::basic_string::operator[]`, this function
        has undefined behavior instead of returning char()

        @param pos position of the character to return

        @return Const reference to the requested character
     */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_reference
    operator[]( size_type pos ) const
    {
        return str_[pos];
    }

    /** Accesses the specified character with bounds checking

        Returns a const reference to the character at specified
        location pos.

        Bounds checking is performed, exception of type
        std::out_of_range will be thrown on invalid access.

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range Invalid position

        @param pos position of the character to return

        @return Const reference to the requested character
     */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_reference
    at( size_type pos ) const
    {
        if(pos >= size())
            detail::throw_out_of_range(
                BOOST_CURRENT_LOCATION);
        return *(str_ + pos);
    }

    /** Accesses the first character

        Returns reference to the first character in the string.

        Equivalent to `operator[](0)`.

        The behavior is undefined if `empty() == true`.

        @return reference to the first character
     */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_reference
    front() const
    {
        BOOST_ASSERT( !empty() );
        return *str_;
    }

    /** Accesses the last character

        Returns reference to the last character in the string

        Equivalent to `operator[](size() - 1)`.

        The behavior is undefined if `empty() == true`.

        @return reference to the first character
     */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_reference
    back() const
    {
        BOOST_ASSERT( !empty() );
        return str_[len_ - 1];
    }

    /** Returns a pointer to the first character of a string

        Returns a pointer to the underlying character array.

        The pointer is such that the range `[data(); data() + size())`
        is valid and the values in it correspond to the values of the
        string.

        @note Unlike `std::basic_string::data()` and string literals,
        `data()` may return a pointer to a buffer that is not
        null-terminated. Therefore, it is typically a mistake to pass
        data() to a routine that takes just a const char* and expects
        a null-terminated string.

        @return A pointer to the underlying character array.
     */
    BOOST_URL_CONST_STRING_CONSTEXPR
    const_pointer
    data() const
    {
        return str_;
    }

    /** Returns the number of char elements in the string

        @return The number of char elements in the string
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    size_type
    size() const noexcept
    {
        return len_;
    }

    /** Returns the number of char elements in the string

        @return The number of char elements in the string
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    size_type
    length() const noexcept
    {
        return len_;
    }

    /** Returns the maximum number of characters

        The largest possible number of char-like objects that
        can be referred to by a const_string

        @return Maximum number of characters
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
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
    BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    empty() const noexcept
    {
        return len_ == 0;
    }

    /** Copies characters

        Copies a substring to the character array pointed to by
        the destination char string, where `rcount` is the smaller
        of `count` and `size() - pos`.

        Equivalent to
        `std::char_traits<char>::copy(dest, data() + pos, rcount)`

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range If `pos > size()`

        @param dest pointer to the destination character string
        @param count requested substring length
        @param pos position of the first character

        @return Number of characters copied
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    size_type
    copy(char* dest,
         size_type count,
         size_type pos = 0) const
    {
        return operator string_view().copy(dest, count, pos);
    }

    /** Returns a substring

        Returns a string view of a substring, where its size is the
        smaller of `count` and `size() - pos`.

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range If `pos > size()`

        @param dest pointer to the destination character string
        @param count requested substring length
        @param pos position of the first character

        @return Number of characters copied
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    string_view
    substr(size_type pos = 0,
           size_type count = npos) const
    {
        return operator string_view().substr(pos, count);
    }

    /** Compares two strings

        The length of the sequences to compare is the smaller of
        `size()` and `other.size()`.

        The function compares the two strings by calling
        `char_traits<char>::compare(data(), v.data(), rlen)`.

        @param other string to compare

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    int
    compare(string_view other) const
    {
        return operator string_view().compare(other);
    }

    /** Compares two strings

        Equivalent to `substr(pos1, count1).compare(other)`

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range If `pos1 > size()`

        @param pos1 position of the first character in this string to compare
        @param count1 number of characters of this string to compare
        @param other string to compare

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    int
    compare(
        size_type pos1,
        size_type count1,
        string_view other) const
    {
        return substr(pos1, count1).compare(other);
    }

    /** Compares two strings

        Equivalent to
        `substr(pos1, count1).compare(other.substr(pos2, count2))`

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range If `pos1 > size()` or `pos2 > other.size()`

        @param pos1 position of the first character in this string to compare
        @param count1 number of characters of this string to compare
        @param other string to compare
        @param pos2 position of the first character in the string to compare
        @param count2 number of characters of the given string to compare

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    int
    compare(
        size_type pos1,
        size_type count1,
        string_view other,
        size_type pos2,
        size_type count2) const
    {
        return substr(pos1, count1).compare(
          other.substr(pos2, count2));
    }

    /** Compares two strings

        Equivalent to `compare(string_view(s))`

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range If `pos1 > size()` or `pos2 > other.size()`

        @param s pointer to the character string to compare to

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    int
    compare(const char* s) const
    {
        return compare(string_view(s));
    }

    /** Compares two strings

        Equivalent to `substr(pos1, count1).compare(string_view(s))`

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range If `pos1 > size()`

        @param pos1 position of the first character in this string to compare
        @param count1 number of characters of this string to compare
        @param s pointer to the character string to compare to

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    int
    compare(
        size_type pos1,
        size_type count1,
        const char* s) const
    {
        return substr(pos1, count1).compare(
          string_view(s));
    }

    /** Compares two strings

        Equivalent to
        `substr(pos1, count1).compare(string_view(s, count2))`

        @par Exception Safety
        Strong guarantee.
        Exceptions thrown on invalid positions.

        @throw out_of_range If `pos1 > size()` or `pos2 > other.size()`

        @param pos1 position of the first character in this string to compare
        @param count1 number of characters of this string to compare
        @param s pointer to the character string to compare to
        @param count2 number of characters of the given string to compare

        @return Negative value if this string is less than the other
        character sequence, zero if the both character sequences are
        equal, positive value if this string is greater than the other
        character sequence
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    int
    compare(
        size_type pos1,
        size_type count1,
        const char* s,
        size_type count2) const
    {
        return substr(pos1, count1).compare(
          string_view(s, count2));
    }

    /** Returns a urls::string_view

        @note It is the programmer's responsibility
        to ensure that the resulting string view
        does not outlive the string.

        @return A string_view representing the
        entire contents of the string.
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    operator string_view() const noexcept
    {
        return string_view(str_, len_);
    }

#ifndef BOOST_URL_NO_CXX17_HDR_STRING_VIEW
    /** Returns a std::string_view

        @note It is the programmer's responsibility
        to ensure that the resulting string view
        does not outlive the string.

        @return A string_view representing the
        entire contents of the string.
    */
    BOOST_URL_CONST_STRING_CONSTEXPR
    operator std::string_view() const noexcept
    {
        return std::string_view(str_, len_);
    }
#endif

    /** Returns a basic_string

        @return A string representing the
        entire contents of the const_string.
    */
    template <class A>
    BOOST_URL_CONST_STRING_CONSTEXPR
    operator std::basic_string<char, std::char_traits<char>, A>() const
    {
        return std::basic_string<
            char, std::char_traits<char>, A>(str_, len_);
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A const_string to compare

        @return true if the corresponding comparison holds
      */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator==( const_string const& lhs,
                const_string const& rhs ) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A const_string to compare

        @return true if the corresponding comparison holds
      */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator!=( const_string const& lhs,
                const_string const& rhs ) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A const_string to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator<( const_string const& lhs,
               const_string const& rhs ) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A const_string to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator<=( const_string const& lhs,
                const_string const& rhs ) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A const_string to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator>( const_string const& lhs,
               const_string const& rhs ) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A const_string to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator>=( const_string const& lhs,
                const_string const& rhs ) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
      */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator==( const_string const& lhs,
                string_view rhs ) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
      */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator!=( const_string const& lhs,
                string_view rhs ) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator<( const_string const& lhs,
               string_view rhs ) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator<=( const_string const& lhs,
                string_view rhs ) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator>( const_string const& lhs,
               string_view rhs ) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator>=( const_string const& lhs,
                string_view rhs ) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
      */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator==( string_view lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) == 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
      */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator!=( string_view lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) != 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator<( string_view lhs,
               const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) > 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator<=( string_view lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) >= 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator>( string_view lhs,
               const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) < 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A string_view to compare

        @return true if the corresponding comparison holds
     */
    friend BOOST_URL_CONST_STRING_CONSTEXPR
    bool
    operator>=( string_view lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) <= 0;
    }

#if !defined(BOOST_URL_NO_CXX17_HDR_STRING_VIEW)
    // "sufficient number of additional overloads"
    // against std::string_view as in urls::string_view

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
      */
    friend
    bool
    operator==( const_string const& lhs,
                std::string_view rhs ) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
      */
    friend
    bool
    operator!=( const_string const& lhs,
                std::string_view rhs ) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator<( const_string const& lhs,
               std::string_view rhs ) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator<=( const_string const& lhs,
                std::string_view rhs ) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator>( const_string const& lhs,
               std::string_view rhs ) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator>=( const_string const& lhs,
                std::string_view rhs ) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
      */
    friend
    bool
    operator==( std::string_view lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) == 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
      */
    friend
    bool
    operator!=( std::string_view lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) != 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator<( std::string_view lhs,
               const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) > 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator<=( std::string_view lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) >= 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator>( std::string_view lhs,
               const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) < 0;
    }

    /** Compares two strings

        @param lhs A const_string to compare
        @param rhs A std::string_view to compare

        @return true if the corresponding comparison holds
     */
    friend
    bool
    operator>=( std::string_view lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) <= 0;
    }
#endif

    // "sufficient number of additional overloads" against types
    // convertible to both urls::string_view and std::string_view

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
      */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator==( const_string const& lhs,
                StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) == 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
      */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator!=( const_string const& lhs,
                StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) != 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator<( const_string const& lhs,
               StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) < 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator<=( const_string const& lhs,
                StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) <= 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator>( const_string const& lhs,
               StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) > 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator>=( const_string const& lhs,
                StringViewable const& rhs ) noexcept
    {
        return lhs.compare(string_view(rhs)) >= 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
      */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator==( StringViewable const& lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) == 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
      */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator!=( StringViewable const& lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) != 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator<( StringViewable const& lhs,
               const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) > 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator<=( StringViewable const& lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) >= 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator>( StringViewable const& lhs,
               const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) < 0;
    }

    /** Compares two strings

        @note This function only participates in overload resolution
        if we can construct a string_view from the types being
        compared.

        @tparam StringViewable A type from which we can construct a string_view

        @param lhs A const_string to compare
        @param rhs A StringViewable to compare

        @return true if the corresponding comparison holds
     */
    template <class StringViewable>
    friend
    BOOST_URL_CONST_STRING_CONSTEXPR
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        std::is_constructible<string_view, StringViewable>::value, bool
    >::type
#else
    bool
#endif
    operator>=( StringViewable const& lhs,
                const_string const& rhs ) noexcept
    {
        return rhs.compare(lhs) <= 0;
    }

    friend inline
    std::ostream&
    operator<<(std::ostream& os,
               const_string const& cs) {
      return os << cs.operator string_view();
    }
};

//------------------------------------------------

/** A producer for constant strings used a specified allocator.

    Objects of this type are used to produce
    constant strings which all use the specified
    allocator.
*/
class const_string::factory
{
    friend class const_string;

    class base;

    base* p_;

    template<class T>
    class impl;

public:
    /** Destructor
    */
    BOOST_URL_DECL
    ~factory();

    /** Constructor

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    factory(
        factory const& other) noexcept;

    /** Assignment

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    factory&
    operator=(
        factory const& other) noexcept;

    /** Constructor

        Default constructed factories use
        the standard allocator
        `std::allocator<char>`.

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    factory() noexcept;

    /** Constructor

        All factories constructed with this
        function will use the standard allocator
        `std::allocator<char>`.
    */
    template<class T>
    explicit
    factory(std::allocator<T> const&)
        : factory()
    {
    }

    /** Constructor

        Creates a factory using the specified
        allocator.
    */
    template<class Allocator>
    explicit
    factory(Allocator const& alloc);

    /** Return a newly constructed string

        Constructs a string with the specified
        size. The function object `init` is
        used to initialize the contents of
        the string, and will be invoked with
        this equivalent signature:

        @code
        void (std::size_t size, char* dest );
        @endcode

        @param n The size of the resulting string

        @param init The function object to invoke.
    */
    template<class InitFn>
    const_string
    operator()(
        std::size_t n,
        InitFn const& init) const;

    /** Return a newly constructed copy of a string

        Constructs a string with the specified
        size. The function object `init` is
        used to initialize the contents of
        the string, and will be invoked with
        this equivalent signature:

        @code
        void (std::size_t size, char* dest );
        @endcode

        @param s The string to copy
    */
    BOOST_URL_DECL
    const_string
    operator()(string_view s) const;
};

} // urls
} // boost

#include <boost/url/impl/const_string.hpp>

#endif
