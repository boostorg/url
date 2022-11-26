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
        grammar::parse(path, detail::dynamic_path_rule).value();
    auto it = segs.begin();
    auto end = segs.end();

    // Iterate existing nodes
    node* cur = &nodes_.front();
    while (it != end)
    {
        if ((*it).string() == "." ||
            (*it).string() == "..")
            urls::detail::throw_invalid_argument();
        auto cit = std::find_if(
            cur->child_idx.begin(),
            cur->child_idx.end(),
            [this, &it](std::size_t ci) -> bool
            {
                return nodes_[ci].seg == *it;
            });
        if (cit == cur->child_idx.end())
            break;
        cur = &nodes_[*cit];
        ++it;
    }

    // Insert new nodes
    while (it != end)
    {
        if ((*it).string() == "." ||
            (*it).string() == "..")
            urls::detail::throw_invalid_argument();
        node child;
        child.seg = *it;
        std::size_t cur_id = cur - nodes_.data();
        child.parent_idx = cur_id;
        nodes_.push_back(std::move(child));
        cur = &nodes_[cur_id];
        cur->child_idx.push_back(nodes_.size() - 1);
        cur = &nodes_.back();
        ++it;
    }
    cur->resource = resource;
}

template <class T>
auto
router<T>::match(string_view request)
    -> result<match_results>
{
    // Parse request as regular path
    auto r = parse_path(request);
    if (!r)
    {
        BOOST_URL_RETURN_EC(r.error());
    }

    // Iterate nodes
    node const* cur = &nodes_.front();
    segments_encoded_view segs = *r;
    auto it = segs.begin();
    auto end = segs.end();
    int level = 0;
    while (it != end)
    {
        if (**it == ".")
        {
            ++it;
            continue;
        }
        if (**it == "..")
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
        auto cit = std::find_if(
            cur->child_idx.begin(),
            cur->child_idx.end(),
            [this, &it](std::size_t i) {
                return nodes_[i].seg.match(*it);
            });
        if (cit == cur->child_idx.end())
            ++level;
        else
            cur = &nodes_[*cit];
        ++it;
    }
    if (!cur->resource ||
        level != 0)
    {
        BOOST_URL_RETURN_EC(
            grammar::error::mismatch);
    }
    return match_results(*cur);
}

} // experimental
} // urls
} // boost

