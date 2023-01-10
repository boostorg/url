//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DETAIL_ROUTER_HPP
#define BOOST_URL_DETAIL_ROUTER_HPP

#include <boost/url/pct_string_view.hpp>
#include <boost/url/grammar/delim_rule.hpp>
#include <boost/url/grammar/optional_rule.hpp>
#include <boost/url/grammar/range_rule.hpp>
#include <boost/url/grammar/tuple_rule.hpp>
#include <string>

namespace boost {
namespace urls {
namespace detail {

// A path segment template
class segment_template
{
    enum class modifier : unsigned char
    {
        none,
        // {id?}
        optional,
        // {id*}
        star,
        // {id+}
        plus
    };

    std::string str_;
    bool is_literal_ = true;
    modifier modifier_ = modifier::none;

    friend struct segment_template_rule_t;
public:
    segment_template() = default;

    BOOST_URL_DECL
    bool
    match(pct_string_view seg) const;

    string_view
    string() const
    {
        return str_;
    }

    BOOST_URL_DECL
    string_view
    id() const;

    bool
    empty() const
    {
        return str_.empty();
    }

    bool
    is_literal() const
    {
        return is_literal_;
    }

    bool
    has_modifier() const
    {
        return !is_literal() &&
               modifier_ != modifier::none;
    }

    bool
    is_optional() const
    {
        return modifier_ == modifier::optional;
    }

    bool
    is_star() const
    {
        return modifier_ == modifier::star;
    }

    bool
    is_plus() const
    {
        return modifier_ == modifier::plus;
    }

    friend
    bool operator==(
        segment_template const& a,
        segment_template const& b)
    {
        if (a.is_literal_ != b.is_literal_)
            return false;
        if (a.is_literal_)
            return a.str_ == b.str_;
        return a.modifier_ == b.modifier_;
    }

    // segments have precedence:
    //     - literal
    //     - unique
    //     - optional
    //     - plus
    //     - star
    friend
    bool operator<(
        segment_template const& a,
        segment_template const& b)
    {
        if (b.is_literal())
            return false;
        if (a.is_literal())
            return !b.is_literal();
        return a.modifier_ < b.modifier_;
    }
};

// A segment template is either a literal string
// or a replacement field (as in a format_string).
// Fields cannot contain format specs and might
// have one of the following modifiers:
// - ?: optional segment
// - *: zero or more segments
// - +: one or more segments
struct segment_template_rule_t
{
    using value_type = segment_template;

    BOOST_URL_DECL
    urls::result<value_type>
    parse(
        char const*& it,
        char const* end
    ) const noexcept;
};

constexpr auto segment_template_rule = segment_template_rule_t{};

constexpr auto path_template_rule =
    grammar::tuple_rule(
        grammar::squelch(
            grammar::optional_rule(
                grammar::delim_rule('/'))),
        grammar::range_rule(
            segment_template_rule,
            grammar::tuple_rule(
                grammar::squelch(grammar::delim_rule('/')),
                segment_template_rule)));

// a small vector for child nodes...
// we shouldn't expect many children per node, and
// we don't want to allocate for that. But we also
// cannot cap the max number of child nodes because
// especially the root nodes can potentially an
// exponentially higher number of child nodes.
class child_idx_vector
{
    static constexpr std::size_t N = 5;
    std::size_t static_child_idx[N]{};
    std::size_t* child_idx{nullptr};
    std::size_t size_{0};
    std::size_t cap_{0};

public:
    ~child_idx_vector()
    {
        delete[] child_idx;
    }

    bool
    empty() const
    {
        return size_ == 0;
    }

    std::size_t
    size() const
    {
        return size_;
    }

    std::size_t*
    begin()
    {
        if (size_ > N)
        {
            return child_idx;
        }
        return static_child_idx;
    }

    std::size_t*
    end()
    {
        return begin() + size_;
    }

    std::size_t const*
    begin() const
    {
        if (size_ > N)
        {
            return child_idx;
        }
        return static_child_idx;
    }

    std::size_t const*
    end() const
    {
        return begin() + size_;
    }

    void
    erase(std::size_t* it)
    {
        BOOST_ASSERT(it - begin() >= 0);
        std::memmove(it - 1, it, end() - it);
        --size_;
    }

    void
    push_back(std::size_t v)
    {
        if (size_ == N && !child_idx)
        {
            child_idx = new std::size_t[N*2];
            cap_ = N*2;
            std::memcpy(child_idx, static_child_idx, N);
        }
        else if (child_idx && size_ == cap_)
        {
            auto* tmp = new std::size_t[cap_*2];
            std::memcpy(tmp, child_idx, cap_);
            delete[] child_idx;
            cap_ = cap_*2;
        }
        begin()[size_++] = v;
    }
};

class router_base
{
    void* impl_{nullptr};

public:
    // A type-erased router resource
    struct any_resource
    {
        virtual ~any_resource() = default;
        virtual void const* get() const noexcept = 0;
    };

protected:
    BOOST_URL_DECL
    router_base();

    BOOST_URL_DECL
    virtual ~router_base();

    BOOST_URL_DECL
    void
    insert_impl(
        string_view s,
        any_resource const* v);

    BOOST_URL_DECL
    any_resource const*
    find_impl(
        segments_encoded_view path,
        string_view*& matches,
        string_view*& names) const noexcept;
};

} // detail
} // urls
} // boost

#endif
