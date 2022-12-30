//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_EXPERIMENTAL_ROUTER_HPP
#define BOOST_URL_EXPERIMENTAL_ROUTER_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_string_view.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/optional.hpp>
#include <boost/url/parse_path.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>
#include <boost/url/detail/router.hpp>
#include <vector>

namespace boost {
namespace urls {

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
class router
{
    struct node
    {
        detail::segment_template seg{};
        optional<T> resource{boost::none};
        std::size_t parent_idx{std::size_t(-1)};
        std::vector<std::size_t> child_idx;

        node const*
        find_optional_resource(
            std::vector<node> const& ns) const
        {
            if (resource)
                return this;
            if (child_idx.empty())
                return nullptr;
            for (auto i: child_idx)
            {
                auto& c = ns[i];
                if (!c.seg.is_optional() &&
                    !c.seg.is_star())
                    continue;
                auto n =
                    c.find_optional_resource(ns);
                if (n)
                    return n;
            }
            return nullptr;
        }
    };

    // Pool of other nodes in the resource tree
    std::vector<node> nodes_;

public:
    class match_results
    {
        // This class should also allow us to get
        // positional and named dynamic segments
        // as well as the rule T.
        // AFREITAS: Is this going to be lazy?
        node const* leaf_;

        friend router;

        explicit
        match_results(node const& leaf_)
            : leaf_(&leaf_)
        {}
    public:
        T const& operator*() const
        {
            return *leaf_->resource;
        }
    };

    /// Constructor
    router()
    {
        nodes_.push_back(node{});
    }

    /** Route the specified URL path to a resource

        @param path A url path with dynamic segments
        @param resource A resource the path corresponds to

        @see
            https://fmt.dev/latest/syntax.html
     */
    void
    route(string_view path, T const& resource);

    /** Match URL path to corresponding resource

        @param request Request path
        @return The match results
     */
    result<match_results>
    match(pct_string_view request);

private:
    node const*
    try_match(
        segments_encoded_view::const_iterator it,
        segments_encoded_view::const_iterator end,
        node const* root,
        int level);
};

} // urls
} // boost

#include <boost/url/impl/router.hpp>

#endif

