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
#include <stdexcept>

namespace boost {
namespace urls {

/// Base class for all routers
class router_base
{
    class impl;
    std::unique_ptr<impl> impl_;

protected:
    // A type-erased router resource
    struct any_resource
    {
        virtual ~any_resource() = default;
        virtual void const* get() const noexcept = 0;
    };

    // A node in the resource tree
    // Each segment in the resource tree might be
    // associated with
    struct node
    {
        // literal segment or replacement field
        detail::segment_template seg{};

        // A pointer to the resource
        any_resource const* resource{nullptr};

        // The complete match for the resource
        std::string path_template;

        // Index of the parent node in the
        // implementation pool of nodes
        std::size_t parent_idx{std::size_t(-1)};

        // Index of child nodes in the pool
        detail::child_idx_vector child_idx;
    };

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

    BOOST_URL_DECL
    router_base();

    BOOST_URL_DECL
    void
    route_impl(
        string_view s,
        any_resource const* p,
        std::size_t maxn);

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
template <class T, std::size_t N = 20>
class router : public router_base
{
public:
    /// A range type with the match results
    class match_results
        : public match_results_base
    {
        string_view matches_storage_[N];
        string_view ids_storage_[N];

        friend router;

        match_results() = default;

        match_results(
            node const* leaf,
            string_view matches[N],
            string_view ids[N],
            std::size_t n)
            : match_results_base(
                leaf, matches_storage_, ids_storage_, n)
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                matches_storage_[i] = matches[i];
                ids_storage_[i] = ids[i];
            }
        }

    public:
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
    match_results
    match(pct_string_view request) const noexcept;
};

} // urls
} // boost

#include <impl/router.hpp>

#endif

