//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_ROUTER_HPP
#define BOOST_URL_ROUTER_HPP

#include <boost/url/detail/config.hpp>
#include <detail/router.hpp>
#include <memory>
#include <vector>
#include <stdexcept>

namespace boost {
namespace urls {

/// Base class for all routers
class router_base
{
    class impl;
    std::unique_ptr<impl> impl_;

protected:
    // A node in the resource tree
    // Each segment in the resource tree might be
    // associated with
    struct node
    {
        static constexpr std::size_t npos{std::size_t(-1)};

        // literal segment or replacement field
        detail::segment_template seg{};

        // A pointer to the resource
        std::size_t resource_idx = std::size_t(-1);

        // The complete match for the resource
        std::string path_template;

        // Index of the parent node in the
        // implementation pool of nodes
        std::size_t parent_idx{npos};

        // Index of child nodes in the pool
        detail::child_idx_vector child_idx;
    };

public:
    class match_results_base
    {
    protected:
        node const* leaf_ = nullptr;
        string_view* matches_ = nullptr;
        string_view* ids_ = nullptr;
        std::size_t size_ = 0;

        match_results_base() = default;

        explicit
        match_results_base(
            node const* leaf,
            string_view* matches,
            string_view* ids,
            std::size_t N)
            : leaf_(leaf)
            , matches_(matches)
            , ids_(ids)
            , size_(N)
        {}
    public:
        using iterator = string_view*;
        using const_iterator = string_view const*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = string_view&;
        using const_reference = string_view const&;
        using pointer = string_view*;
        using const_pointer = string_view const*;

        // result-like functions
        explicit
        operator bool() const;

        bool
        has_value() const;

        bool
        has_error() const;

        error_code
        error() const;

        // vector-like functions
        const_reference
        at( size_type pos ) const;

        const_reference
        at( string_view id ) const;

        const_reference
        operator[]( size_type pos ) const;

        const_reference
        operator[]( string_view id ) const;

        const_iterator
        find( string_view id ) const;

        const_iterator
        begin() const;

        const_iterator
        end() const;

        bool
        empty() const noexcept;

        size_type
        size() const noexcept;
    };

protected:
    BOOST_URL_DECL
    router_base();

    BOOST_URL_DECL
    void
    route_impl(
        string_view s,
        std::size_t idx);

    BOOST_URL_DECL
    node const*
    match_impl(
        string_view s,
        string_view*& matches,
        string_view*& names) const noexcept;
};


/** A URL router.

    This container matches static and dynamic
    URL requests to an object which represents
    how the it should be handled. These
    values are usually callback functions.

    @tparam T type of resource associated with
    each path template

    @tparam N maximum number of replacement fields
    in a path template

    @par Exception Safety

    @li Functions marked `noexcept` provide the
    no-throw guarantee, otherwise:

    @li Functions which throw offer the strong
    exception safety guarantee.

    @see
        @ref parse_absolute_uri,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref resolve.
*/
template <class T>
class router : public router_base
{
public:
    /// A range type with the match results
    template <std::size_t N = 20>
    class match_results
        : public match_results_base
    {
        string_view matches_storage_[N];
        string_view ids_storage_[N];
        T const* data_{nullptr};

        friend router;

        match_results(
            T const* data,
            node const* leaf,
            string_view matches[N],
            string_view ids[N],
            std::size_t n)
            : match_results_base(
                leaf, matches_storage_, ids_storage_, n)
            , data_(data)
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                matches_storage_[i] = matches[i];
                ids_storage_[i] = ids[i];
            }
        }

    public:
        match_results() = default;

        T const&
        operator*() const;

        T const&
        value() const;
    };

    /// Constructor
    router() = default;

    /** Route the specified URL path to a resource

        @param path A url path with dynamic segments
        @param resource A resource the path corresponds to

        @see
            https://fmt.dev/latest/syntax.html
     */
    template <class U>
    void
    route(string_view path, U&& resource);

    /** Match URL path to corresponding resource

        @param request Request path
        @return The match results
     */
    template <std::size_t N>
    bool
    match(string_view request, match_results<N>& m) const noexcept;

    /** Match URL path and stores results into args

        @param request Request path
        @param args Storage for the matches

        @return True if we found a match
     */
    template <class ...Args>
    result<T>
    match_to(string_view request, Args&... args) const;

private:
    std::vector<T> data_;
};

} // urls
} // boost

#include <impl/router.hpp>

#endif

