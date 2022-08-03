//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_DETAIL_TYPE_TRAITS_HPP
#define BOOST_URL_GRAMMAR_DETAIL_TYPE_TRAITS_HPP

#include <iterator>

namespace boost {
namespace urls {
namespace grammar {
namespace detail {

// input iter exemplar
struct input_it_ex {
    // InputIterator

    // 1. Iterator

    // 1.1 Type defs
    using value_type = char;
    using difference_type = std::ptrdiff_t;
    using reference = char&;
    using pointer = char*;
    using iterator_category = std::input_iterator_tag;

    // 1.2 CopyConstructible
    input_it_ex(input_it_ex const&);

    // 1.3 CopyAssignable
    input_it_ex&
    operator=(input_it_ex const&);

    // 1.4 Destructible
    ~input_it_ex();

    // 1.5 Swappable
    friend
    void swap(
        input_it_ex&,
        input_it_ex&);

    // 1.6 Expressions
    reference
    operator*() const;

    input_it_ex&
    operator++();

    input_it_ex
    operator++(int);

    // 1.7 Default constructible (GCC11/MSVC14.2)
    input_it_ex();

    // 2. EqualityComparable
    friend
    bool operator==(
        input_it_ex const&,
        input_it_ex const&);

    friend
    bool operator!=(
        input_it_ex const&,
        input_it_ex const&);

    // 3. Expressions
    pointer
    operator->();
};

} // detail
} // grammar
} // urls
} // boost

#endif
