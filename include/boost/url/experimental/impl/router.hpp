//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {
namespace experimental {

template <class T>
void
router<T>::route(string_view path, T const& resource)
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
    cur->resource = resource;
}

template <class T>
auto
router<T>::try_match(
    segments_encoded_view::const_iterator it,
    segments_encoded_view::const_iterator end,
    node const* cur,
    int level)
    -> node const*
{
    while (it != end)
    {
        pct_string_view s = *it;
        if (*s == ".")
        {
            ++it;
            continue;
        }
        if (*s == "..")
        {
            ++it;
            if (level > 0 ||
                cur == &nodes_.front())
                --level;
            else
                cur = &nodes_[cur->parent_idx];
            continue;
        }
        if (level < 0)
        {
            ++level;
            ++it;
            continue;
        }
        node const* r = nullptr;
        for (auto i: cur->child_idx)
        {
            auto& c = nodes_[i];
            if (c.seg.match(s))
            {
                if (c.seg.is_literal() ||
                    !c.seg.has_modifier())
                {
                    r = try_match(
                        std::next(it), end, &c, level);
                }
                else if (c.seg.is_optional())
                {
                    r = try_match(
                        std::next(it), end, &c, level);
                    if (r)
                        break;
                    r = try_match(
                        it, end, &c, level);
                }
                else
                {
                    auto it1 = it;
                    if (c.seg.is_plus())
                        ++it1;
                    while (it1 != end)
                    {
                        r = try_match(
                            it1, end, &c, level);
                        if (r)
                            break;
                        ++it1;
                    }
                    r = try_match(
                        it1, end, &c, level);
                    if (r)
                        break;
                }
            }
        }
        if (r)
            return r;
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
        // we reached a node with no resource,
        // but it might still have only child
        // optional segments with resources
        return cur->find_optional_resource(nodes_);
    }
    return cur;
}

template <class T>
auto
router<T>::match(pct_string_view request)
    -> result<match_results>
{
    // Parse request as regular path
    auto r = parse_path(request);
    if (!r)
    {
        BOOST_URL_RETURN_EC(r.error());
    }
    segments_encoded_view segs = *r;

    // Iterate nodes
    node const* cur = try_match(
        segs.begin(), segs.end(),
        &nodes_.front(), 0);
    if (cur)
        return match_results(*cur);
    BOOST_URL_RETURN_EC(
        grammar::error::mismatch);
}

} // experimental
} // urls
} // boost

