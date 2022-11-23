//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
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
#include <boost/url/error.hpp>
#include <boost/url/parse_path.hpp>
#include <boost/url/pct_string_view.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/rfc/uri_rule.hpp>
#include <boost/url/rfc/pchars.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>
#include <boost/optional.hpp>
#include <vector>

namespace boost {
namespace urls {
namespace experimental {

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
    struct match_any_t {};

    // A dynamic path segment
    class dynamic_segment
    {
        using validate_fn = std::function<bool(string_view)>;
        variant2::variant<std::string, match_any_t, validate_fn> seg_;
    public:
        dynamic_segment() = default;

        dynamic_segment(dynamic_segment const& s) = default;

        dynamic_segment(string_view s)
            : seg_(s) {}

        dynamic_segment(validate_fn s)
            : seg_(s) {}

        dynamic_segment(match_any_t s)
            : seg_(s) {}

        dynamic_segment&
        operator=(dynamic_segment const& s) = default;

        dynamic_segment&
        operator=(string_view s)
        {
            seg_ = s;
            return *this;
        }

        dynamic_segment&
        operator=(validate_fn s)
        {
            seg_ = s;
            return *this;
        }

        dynamic_segment&
        operator=(match_any_t s)
        {
            seg_ = s;
            return *this;
        }

        bool
        empty() const
        {
            return
                seg_.index() == 0 &&
                variant2::get<0>(seg_).empty();
        }

        bool
        match(string_view v)
        {
            if (variant2::holds_alternative<std::string>(seg_))
                return variant2::get<std::string>(seg_) == v;
            else if (variant2::holds_alternative<match_any_t>(seg_))
                return true;
            else
                return variant2::get<validate_fn>(seg_)(v);
        }

        friend
        bool operator==(
            dynamic_segment const& a,
            dynamic_segment const& b)
        {
            if (a.seg_.index() != b.seg_.index())
                return false;
            if (variant2::holds_alternative<std::string>(a.seg_))
                return
                    variant2::get<std::string>(a.seg_) ==
                    variant2::get<std::string>(b.seg_);
            return variant2::holds_alternative<match_any_t>(a.seg_);
        }
    };

    // A node in the resource tree
    struct resource_seg
    {
        dynamic_segment seg;
        optional<T> resource{boost::none};
        std::size_t parent_idx{std::size_t(-1)};
        std::vector<std::size_t> child_idx;
    };

    // Pool of other nodes in the resource tree
    std::vector<resource_seg> nodes_;

    // AFREITAS: Qt uses a map of rules for
    // dynamic segments. This seems more
    // appropriate for a C++ API because there's
    // nothing in other frameworks that look good
    // in C++. Being able to constrain the
    // dynamic segments is fundamental.
    // std::map<int, std::string> rules_;

public:
    class match_results
    {
        // This class should also allow us to get
        // positional and named dynamic segments
        // as well as the rule T.
        // AFREITAS: Is this going to be lazy?
        resource_seg const* leaf_;
    public:
        explicit
        match_results(resource_seg const& leaf_)
            : leaf_(&leaf_)
        {}

        T const& operator*() const
        {
            return *leaf_->resource;
        }
    };

    router()
    {
        nodes_.push_back(resource_seg{});
    }

    void
    route(pct_string_view path, T const& resource)
    {
        auto it = path.begin();
        auto end = path.end();
        auto next = [&]() -> result<dynamic_segment>
        {
            bool dyn = false;
            if (it == end)
                return {};
            if (*it == '/')
                ++it;
            // We support the {id} pattern for now
            if (*it == '{')
            {
                ++it;
                dyn = true;
            }
            auto r =
                detail::segment_rule.parse(it, end);
            if (!r)
            {
                BOOST_URL_RETURN_EC(
                    grammar::error::invalid);
            }
            if (dyn)
            {
                if (it == end ||
                    *it != '}')
                {
                    BOOST_URL_RETURN_EC(
                        grammar::error::mismatch);
                }
                ++it;
                return {match_any_t{}};
            }
            return dynamic_segment{*r};
        };

        resource_seg* cur = &nodes_.front();

        // iterate existing nodes
        dynamic_segment seg = next().value();
        while (!seg.empty())
        {
            auto cit = std::find_if(
                cur->child_idx.begin(),
                cur->child_idx.end(),
                [this, &seg](std::size_t ci) -> bool
                { return nodes_[ci].seg == seg; });
            if (cit != cur->child_idx.end())
            {
                cur = &nodes_[*cit];
                seg = next().value();
                continue;
            }
            break;
        }

        // insert new nodes
        while (!seg.empty())
        {
            resource_seg child;
            child.seg = std::move(seg);
            std::size_t cur_id = cur - nodes_.data();
            child.parent_idx = cur_id;
            nodes_.push_back(std::move(child));
            cur = &nodes_[cur_id];
            cur->child_idx.push_back(nodes_.size() - 1);
            cur = &nodes_.back();
            seg = next().value();
        }
        cur->resource = resource;
    }

    // match route and return corresponding result
    result<match_results>
    match(string_view request)
    {
        auto r = parse_path(request);
        if (!r)
        {
            BOOST_URL_RETURN_EC(r.error());
        }
        resource_seg const* cur = &nodes_.front();
        segments_encoded_view segs = *r;
        auto sit = segs.begin();
        auto send = segs.end();
        while (sit != send)
        {
            auto cit = std::find_if(
                cur->child_idx.begin(),
                cur->child_idx.end(),
                [this, &sit](std::size_t i) {
                    return nodes_[i].seg.match(*sit);
                });
            if (cit == cur->child_idx.end())
            {
                BOOST_URL_RETURN_EC(
                    grammar::error::mismatch);
            }
            cur = &nodes_[*cit];
            ++sit;
        }
        if (!cur->resource)
        {
            BOOST_URL_RETURN_EC(
                grammar::error::mismatch);
        }
        return match_results(*cur);
    }

    // AFREITAS: this function would return a
    // request string with the params replaced,
    // but this usually doesn't make much sense
    // unless we supported named routes
    //    std::string
    //    resolve(
    //        pct_string_view route_name,
    //        std::initializer_list<string_view> params)
    //    {
    //
    //        return {};
    //    }

    /*
     * AFREITAS: Implement some other
     * std::map-like functions?
     */

private:


};

} // experimental
} // urls
} // boost

#endif



