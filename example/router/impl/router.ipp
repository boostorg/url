//
// Copyright (c) 2023 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#include <router.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/pct_string_view.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/optional.hpp>
#include <boost/url/parse_path.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>

namespace boost {
namespace urls {

class router_base::impl
{
    // Pool of nodes in the resource tree
    std::vector<router_base::node> nodes_;

public:
    impl()
    {
        // root node with no resource
        nodes_.push_back(node{});
    }

    ~impl()
    {
        for (auto &r: nodes_)
            delete r.resource;
    }

    // include a node for a resource
    void
    route_impl(
        string_view path,
        any_resource const* resource);

    // match a node
    router_base::node const*
    match_impl(string_view request) const;

private:
    // try to match from this root node
    node const*
    try_match(
        segments_encoded_view::const_iterator it,
        segments_encoded_view::const_iterator end,
        node const* root,
        int level) const;

    // check if a node has a resource when we
    // also consider optional paths through
    // the child nodes.
    static
    node const*
    find_optional_resource(
        const node* root,
        std::vector<node> const& ns);
};

router_base::node const*
router_base::impl::
find_optional_resource(
    const node* root,
    std::vector<node> const& ns)
{
    BOOST_ASSERT(root);
    if (root->resource)
        return root;
    if (root->child_idx.empty())
        return nullptr;
    for (auto i: root->child_idx)
    {
        auto& c = ns[i];
        if (!c.seg.is_optional() &&
            !c.seg.is_star())
            continue;
        // Child nodes are also
        // potentially optional.
        auto n =
            find_optional_resource(&c, ns);
        if (n)
            return n;
    }
    return nullptr;
}

void
router_base::impl::
route_impl(
    string_view path,
    router_base::any_resource const* resource)
{
    // Parse dynamic route segments
    if (path.starts_with("/"))
        path.remove_prefix(1);
    auto segs =
        grammar::parse(path, detail::path_template_rule).value();
    auto it = segs.begin();
    auto end = segs.end();

    // Iterate existing nodes
    node* cur = &nodes_.front();
    int level = 0;
    while (it != end)
    {
        string_view seg = (*it).string();
        if (seg == ".")
        {
            ++it;
            continue;
        }
        if (seg == "..")
        {
            // discount unmatched leaf or
            // keep track of levels behind root
            if (level > 0 ||
                cur == &nodes_.front())
            {
                --level;
                ++it;
                continue;
            }
            // move to parent deleting current
            // if it carries no resource
            std::size_t p_idx = cur->parent_idx;
            if (cur == &nodes_.back() &&
                !cur->resource &&
                cur->child_idx.empty())
            {
                node* p = &nodes_[p_idx];
                std::size_t cur_idx = cur - nodes_.data();
                p->child_idx.erase(
                    std::remove(
                        p->child_idx.begin(),
                        p->child_idx.end(),
                        cur_idx));
                nodes_.pop_back();
            }
            cur = &nodes_[p_idx];
            ++it;
            continue;
        }
        // discount unmatched root parent
        if (level < 0)
        {
            ++level;
            ++it;
            continue;
        }
        // look for child
        auto cit = std::find_if(
            cur->child_idx.begin(),
            cur->child_idx.end(),
            [this, &it](std::size_t ci) -> bool
            {
                return nodes_[ci].seg == *it;
            });
        if (cit != cur->child_idx.end())
        {
            // move to existing child
            cur = &nodes_[*cit];
        }
        else
        {
            // create child if it doesn't exist
            node child;
            child.seg = *it;
            std::size_t cur_id = cur - nodes_.data();
            child.parent_idx = cur_id;
            nodes_.push_back(std::move(child));
            nodes_[cur_id].child_idx.push_back(nodes_.size() - 1);
            cur = &nodes_.back();
        }
        ++it;
    }
    if (level != 0)
    {
        urls::detail::throw_invalid_argument();
    }
    if (cur->resource)
    {
        delete cur->resource;
    }
    cur->resource = resource;
}

router_base::node const*
router_base::impl::
try_match(
    segments_encoded_view::const_iterator it,
    segments_encoded_view::const_iterator end,
    node const* cur,
    int level) const
{
    while (it != end)
    {
        pct_string_view s = *it;
        if (*s == ".")
        {
            // ignore segment
            ++it;
            continue;
        }
        if (*s == "..")
        {

            // move back to the parent node
            ++it;
            if (level <= 0 &&
                cur != &nodes_.front())
                cur = &nodes_[cur->parent_idx];
            else
                // there's no parent, so we
                // discount that from the implicit
                // tree beyond terminals
                --level;
            continue;
        }

        // we are in the implicit tree above the
        // root, so discount that as a level
        if (level < 0)
        {
            ++level;
            ++it;
            continue;
        }

        // calculate the lower bound on the
        // possible number of branches to
        // determine if we need to branch.
        // We branch when we might have more than
        // one child matching node at this level.
        // If so, we need to potentially branch
        // to find which path leads to a valid
        // resource. Otherwise, we can just
        // consume the node and input without
        // any recursive function calls.
        bool branch = false;
        if (cur->child_idx.size() > 1)
        {
            int branches_lb = 0;
            for (auto i: cur->child_idx)
            {
                auto& c = nodes_[i];
                if (c.seg.is_literal() ||
                    !c.seg.has_modifier())
                {
                    // a literal path counts only
                    // if it matches
                    branches_lb += c.seg.match(s);
                }
                else
                {
                    // everything not literal counts
                    // as more than one path already
                    branches_lb = 2;
                }
                if (branches_lb > 1)
                {
                    // already know we need to
                    // branch
                    branch = true;
                    break;
                }
            }
        }

        // attempt to match each child node
        node const* r = nullptr;
        bool match_any = false;
        for (auto i: cur->child_idx)
        {
            auto& c = nodes_[i];
            if (c.seg.match(s))
            {
                if (c.seg.is_literal() ||
                    !c.seg.has_modifier())
                {
                    // just continue from the
                    // next segment
                    if (branch)
                    {
                        r = try_match(
                            std::next(it), end, &c, level);
                        if (r)
                            break;
                    }
                    else
                    {
                        cur = &c;
                        match_any = true;
                        break;
                    }
                }
                else if (c.seg.is_optional())
                {
                    // attempt to match by ignoring
                    // and not ignoring the segment.
                    // we first try the complete
                    // continuation consuming the
                    // input, which is the
                    // longest and most likely
                    // match
                    r = try_match(
                        std::next(it), end, &c, level);
                    if (r)
                        break;
                    // try complete continuation
                    // consuming no segment
                    r = try_match(
                        it, end, &c, level);
                    if (r)
                        break;
                }
                else
                {
                    // attempt to match many
                    // segments
                    auto first = it;
                    // if this is a plus seg, we
                    // already consumed the first
                    // segment
                    if (c.seg.is_plus())
                        ++first;
                    // {*} is usually the last
                    // match in a path.
                    // try complete continuation
                    // match for every subrange
                    // from {last, last} to
                    // {first, last}.
                    // We also try {last, last}
                    // first because it is the
                    // longest match.
                    auto start = end;
                    while (start != first)
                    {
                        r = try_match(
                            start, end, &c, level);
                        if (r)
                            break;
                        --start;
                    }
                    if (r)
                        break;
                    r = try_match(
                        start, end, &c, level);
                    if (r)
                        break;
                }
            }
        }
        // r represent we already found a terminal
        // node which is a match
        if (r)
            return r;
        // if we couldn't match anything, we go
        // one level up in the implicit tree
        // because the path might still have a
        // "..".
        if (!match_any)
            ++level;
        ++it;
    }
    if (level != 0)
    {
        // the path ended below or above an
        // existing node
        return nullptr;
    }
    if (!cur->resource)
    {
        // we consumed all the input and reached
        // a node with no resource, but it might
        // still have child optional segments
        // with resources we can reach without
        // consuming any input
        return find_optional_resource(cur, nodes_);
    }
    return cur;
}

router_base::node const*
router_base::impl::
match_impl(string_view request) const
{
    // Parse request as regular path
    auto r = parse_path(request);
    if (!r)
    {
        // cannot find match for invalid path
        return nullptr;
    }
    segments_encoded_view segs = *r;

    // Iterate nodes from the root
    return try_match(
        segs.begin(), segs.end(),
        &nodes_.front(), 0);
}

BOOST_URL_DECL
router_base::
router_base()
    : impl_(new impl{}) {}

void
router_base::
route_impl(
    string_view s,
    any_resource const* p)
{
    impl_->route_impl(s, p);
}

router_base::node const*
router_base::
match_impl(string_view s) const noexcept
{
    return impl_->match_impl(s);
}


} // urls
} // boost

