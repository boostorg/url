//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_HPP
#define BOOST_URL_PARAMS_HPP

#include <boost/url/detail/config.hpp>

#include <boost/url/query_param.hpp>
#include <boost/url/const_string.hpp>
#include <boost/url/any_allocator.hpp>
#include <boost/url/detail/parts_base.hpp>

#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url;
class params_encoded;
#endif

/** A random-access handle to percent-decoded query parameters in a url.

   This handle represents the range of percent-decoded query parameters in a url.

   Its iterator dereferences to read-only query parameters in the underlying url.

 */
class params
    : private detail::parts_base
{
    friend class url;
    friend class params_encoded;

    /** A pointer to the url that contains the query parameters
     */
    url* u_ = nullptr;

    /** An allocator of read-only string values

        This is the allocator used to create read-only strings
        when iterators are dereferenced.
     */
    any_allocator<char> a_;

    /** Construct query params from a url

        This function constructs a query params object with the query
        parameters from the url `u`.

        @tparam Allocator Allocator type

        @param u The url with the query params

        @param a The allocator to encode the query params

     */
    template<class Allocator>
    params(
        url& u,
        Allocator const& a);

public:
    /** A random-access iterator referencing parameters in a url query.

        This iterator lazily constructs instances of @ref params::reference, which
        contain @ref string_value decoded representations of the current <key, value> pair
        in the underlying @ref url.

        The @ref params underlying allocator is used to construct the instances
        of @ref params::reference.

    */
#ifdef BOOST_URL_DOCS
    using iterator = __see_below__;
#else
    class iterator;
#endif

    /** A random-access iterator referencing parameters in a url query.

        @note Both @ref params::iterator and @ref params::const_iterator are
        read only.

    */
    using const_iterator = iterator;

    /** A read-only structure representing a decoded query parameter.

        Objects of this type are returned when accessing key-value pairs of the
        @ref params container.

        The instances of @ref string_value stored in a reference are read-only.

    */
    class reference
    {
        friend class params;
        friend class iterator;

        BOOST_URL_DECL
        reference(
            char const* s,
            std::size_t nk,
            std::size_t nv,
            const any_allocator<char>& a);

    public:
        /** The query key.

            This string, which may be empty,
            holds the query key with percent-decoding
            applied.
        */
        const_string key;

        /** The query value.

            This string, which may be empty,
            holds the query value with percent-decoding
            applied. The field @ref has_value indicates
            if the query value is defined.
        */
        const_string value;

        /** True if the query parameter has a value.

            This field is true if the query parameter
            has a value, even if that value is empty.
            An empty value is distinguished from
            having no value.
        */
        bool has_value;
    };

    /** A read-only structure representing a decoded query parameter.

        @note Both @ref params::reference and @ref params::const_reference are
        read only.

    */
    using const_reference = reference;

    /** A view of a query parameter key-value pair

        Unlike @ref params::reference, an instance of @ref params::value_type
        allows for making a copy of a query parameter where ownership is retained in the copy.

    */
    using value_type = query_param_view;

    /** An unsigned integer type to represent sizes
    */
    using size_type = std::size_t;

    /** A signed integer type to represent differences
    */
    using difference_type = std::ptrdiff_t;

    /** <!-- @name Members -->
        <!-- @{ -->
     */

    /** Assignment from initializer list

        Assigns @ref params from a list of @ref params::value_type.

        Each instance of @ref params::value_type is a view of a query <key, value> pair.

        @return Pointer to this instance of @ref params

        @param init Initializer list with query parameters

     */
    inline
    params&
    operator=(std::initializer_list<value_type> init);

    /** Assignment from initializer list

        Assign @ref params from a list of @ref params::value_type.

        Each instance of @ref params::value_type is a view of a query <key, value> pair.

        @param init Initializer list with query parameters

     */
    inline
    void
    assign(std::initializer_list<
        value_type> init);

    /** Assignment from iterators

        Assign @ref params from a range of @ref params::value_type.

        Each instance of @ref params::value_type is a view of a query <key, value> pair.

        @tparam FwdIt Forward Iterator Type

        @param first An iterator to the beginning of the query parameter range

        @param last An iterator to the end of the query parameter range

     */
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last);

private:
    /** Assignment from forward iterators

        Assign @ref params from a range of @ref params::value_type.

        Each instance of @ref params::value_type is a view of a query <key, value> pair.

        @tparam FwdIt Forward Iterator Type

        @param first An iterator to the beginning of the query parameter range

        @param last An iterator to the end of the query parameter range

     */
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last,
        std::forward_iterator_tag);

public:
#ifndef BOOST_URL_DOCS
    /** Assignment from input iterators is deleted

        @note <!-- Doxygen cannot render ` = delete` -->
     */
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last,
        std::input_iterator_tag) = delete;
#endif

    /** <!-- @} --> */

    /** <!-- @name Element Access -->
        <!-- @{ -->
     */

    /** Access element at specified position with bounds checking

        @return Reference to query parameter at specified position

        @param pos Position

     */
    inline
    const_reference
    at(std::size_t pos) const;

    /** Access element with specified key with bounds checking

        @return Reference to mapped value of a query parameter

        @param key Element key

     */
    BOOST_URL_DECL
    const_string
    at(string_view key) const;

    /** Access element at specified position without bounds checking

        @return Reference to query parameter at specified position

        @param pos Element position

     */
    BOOST_URL_DECL
    const_reference
    operator[](
        std::size_t pos) const;

    /** Access first query parameter in the container

        @return Reference to first query parameter in the container

     */
    inline
    const_reference
    front() const;

    /** Access last query parameter in the container

        @return Reference to last query parameter in the container

     */
    inline
    const_reference
    back() const;

    /** <!-- @} --> */

    /** <!-- @name Iterators -->
        <!-- @{ -->
     */

    /** Returns an iterator to the beginning of the container

        @return Iterator to beginning of the container

     */
    inline
    const_iterator
    begin() const noexcept;

    /** Returns an iterator to the end of the container

        @return Iterator to the end of the container

     */
    inline
    iterator
    end() const noexcept;

    /** <!-- @} --> */

    /** <!-- @name Capacity -->
        <!-- @{ -->
     */

    /** Checks whether the container is empty

        @return True if container is empty

     */
    inline
    bool
    empty() const noexcept;

    inline
    std::size_t
    size() const noexcept;

    /** <!-- @} --> */

    /** <!-- @name Modifiers -->
        <!-- @{ -->
     */

    /** Clears the contents of the container

        Clears the contents of the container as if calling
        `erase(begin(), end())`.

     */
    inline
    void
    clear() noexcept;

    /** Insert element at the specified container position

        @return Iterator pointing to the new inserted element

        @param before Position at which the element should be inserted

        @param v Element to be inserted

      */
    inline
    iterator
    insert(
        iterator before,
        value_type const& v);

    /** Insert list of elements at container position

        @return Iterator pointing to the first inserted element

        @param before Position at which the elements should be inserted

        @param init Elements to be inserted

     */
    inline
    iterator
    insert(
        iterator before,
        std::initializer_list<
            value_type> init);

    /** Insert range of elements at container position

        @tparam FwdIt Iterator type

        @return Iterator pointing to the first inserted element

        @param before Position at which the elements should be inserted

        @param first An iterator to the beginning of the query parameter range

        @param last An iterator to the end of the query parameter range

     */
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last);

private:
    /** Insert forward range of elements at container position
     */
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::forward_iterator_tag);

public:
#ifndef BOOST_URL_DOCS
    /** Insert input range of elements at container position

        @note <!-- Doxygen cannot render ` = delete` -->
     */
    template<class FwdIt>
    iterator
    insert(
            iterator before,
            FwdIt first,
            FwdIt last,
            std::input_iterator_tag) = delete;
#endif

     /** Replace an element at container position

        @return Iterator to position where element was inserted

        @param pos Position whose element should be replaced

        @param value New value for the position

     */
    inline
    iterator
    replace(
        iterator pos,
        value_type const& value);

     /** Replace a range of elements at a range of container positions

        @tparam FwdIt Iterator type

        @return Iterator to position where the first inserted element

        @param from Iterator to beginning of the range to be replaced

        @param to Iterator to end of the range to be replaced

        @param first An iterator to the beginning of range to be inserted

        @param last An iterator to the end of the range to be inserted

     */
    template<class FwdIt>
    iterator
    replace(
        iterator from,
        iterator to,
        FwdIt first,
        FwdIt last);

     /** Replace a list of elements at a range of container positions

        @return Iterator to position where the first inserted element

        @param from Iterator to beginning of the range to be replaced

        @param to Iterator to end of the range to be replaced

        @param init List of elements to replace the range

     */
    inline
    iterator
    replace(
        iterator from,
        iterator to,
        std::initializer_list<
            value_type> init);

     /** Remove a query parameter from the container

        @return Iterator to position where the element was removed

        @param pos Iterator to element which should be removed

     */
    BOOST_URL_DECL
    iterator
    remove_value(
        iterator pos);

     /** Replace a query parameter at the specified container position

        @return Iterator to position where the element was replaced

        @param pos Position whose element should be replaced

        @param value New value for the element

     */
    BOOST_URL_DECL
    iterator
    replace_value(
        iterator pos,
        string_view value);

    /** Constructs a value at container position

        @return Iterator to position where the element was constructed

        @param pos Position where the new element should be constructed

        @param key Key of the new element

        @param value Value of the new element

     */
    inline
    iterator
    emplace_at(
        iterator pos,
        string_view key,
        string_view value);

    /** Constructs a key-only value at container position

        This function constructs a value at the container position.

        The new value has a specified query key and no query mapped value.

        @return Iterator to position where the element was constructed

        @param pos Position where the new element should be constructed

        @param key The key of the new element

      */
    inline
    iterator
    emplace_at(
        iterator pos,
        string_view key);

    /** Constructs a key-only query parameter at container position

        @return Iterator to position where the element was constructed

        @param before Position where the new element should be constructed

        @param key Key of the query param

        @param value Value of the query param

      */
    inline
    iterator
    emplace_before(
        iterator before,
        string_view key,
        string_view value);

    /** Constructs a value at container position

        @return Iterator to position where the element was constructed

        @param before Position where the new element should be constructed

        @param key Key of the new query param

      */
    inline
    iterator
    emplace_before(
        iterator before,
        string_view key);

    /** Erases element from container at specified position

        @return Iterator to position where the element was erased

        @param pos Position whose element should be erased

      */
    inline
    iterator
    erase(iterator pos);

    /** Erases range of elements from container

        @return Iterator to position where the first element was erased

        @param first An iterator to the beginning of the query parameter range

        @param last An iterator to the end of the query parameter range

      */
    BOOST_URL_DECL
    iterator
    erase(
        iterator first,
        iterator last);

    /** Erases elements associated with a specified key from container

        @return Number of elements erased from the container

        @param key Key of the element to be erased

      */
    BOOST_URL_DECL
    std::size_t
    erase(string_view key) noexcept;

    /** Construct key-only element at the end of the container

        @return Position where the element was constructed

        @param key String-like element key

      */
    inline
    iterator
    emplace_back(
        string_view key);

    /** Construct element at the end of the container

        @return Position where the element was constructed

        @param key Element key

        @param value Element value

      */
    inline
    iterator
    emplace_back(
        string_view key,
        string_view value);

    /** Insert value at the end of the container

        @param value Value to be inserted

      */
    inline
    void
    push_back(
        value_type const& value);

    /** Remove element at the end of the container
      */
    inline
    void
    pop_back() noexcept;

    /** <!-- @} --> */

    /** <!-- @name Lookup -->
        <!-- @{ -->
     */

    /** Count number of elements with a specified key

        @return Number of elements with a specified key

        @param key Element key

      */
    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    /** Find element with a specified key

        @return Iterator pointing to element with a specified key

        @param key Element key

      */
    inline
    iterator
    find(string_view key) const noexcept;

    /** Find element with a specified key after the given position

        This function searches the range [`from`, `end`).

        `from==end` is valid.

        @return Iterator pointing to element with the specified key

        @param from First position to consider in search

        @param key Element key

      */
    BOOST_URL_DECL
    iterator
    find(
        iterator from,
        string_view key) const noexcept;

    /** Check if container contains an element with the specified key

        @return True if container contains an element with the specified key

        @param key Element key

      */
    inline
    bool
    contains(string_view key) const noexcept;

    /** <!-- @} --> */
};

} // urls
} // boost

// VFALCO This include is at the bottom of
// url.hpp because of a circular dependency
//#include <boost/url/impl/params.hpp>

#endif
