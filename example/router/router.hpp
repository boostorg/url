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
        std::vector<std::size_t> child_idx;
    };

    class match_results_base
    {
    protected:
        node const* leaf_;

        explicit
        match_results_base(node const* leaf)
            : leaf_(leaf)
        {}
    public:
        explicit
        operator bool() const;
        bool has_value() const;
        bool has_error() const;
        error_code error() const;
    };

    BOOST_URL_DECL
    router_base();

    BOOST_URL_DECL
    void
    route_impl(
        string_view s,
        any_resource const* p);

    BOOST_URL_DECL
    node const*
    match_impl(string_view s) const noexcept;
};


/** A URL router.

    This container matches static and dynamic
    URL requests to an object which represents
    how the it should be handled. These
    values are usually callback functions.

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
    class match_results
        : public match_results_base
    {
        friend router;

        match_results(node const* leaf)
            : match_results_base(leaf)
        {}

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
    match(pct_string_view request);
};

} // urls
} // boost

#include <impl/router.hpp>

#endif

