//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_ANY_SEGMENTS_ITER_HPP
#define BOOST_URL_DETAIL_ANY_SEGMENTS_ITER_HPP

#include <boost/url/pct_string_view.hpp>
#include <boost/static_assert.hpp>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {
namespace detail {

class BOOST_SYMBOL_VISIBLE
    any_segments_iter
{
    string_view* s_ = nullptr;

protected:
    explicit
    any_segments_iter(
        string_view* s = nullptr) noexcept
        : s_(s)
    {
    }

public:
    string_view front;

    // Return the input string or nullptr
    string_view*
    input() const noexcept
    {
        return s_;
    }

    // Rewind the iterator to the beginning
    virtual
    void
    rewind() noexcept = 0;

    // Measure and increment the current
    // element. n is increased by the
    // encoded size. Returns false on
    // end of range. 
    virtual bool measure(
        std::size_t& n) noexcept = 0;

    // Copy and increment the current
    // element. encoding is performed
    // if needed.
    virtual
    void
    copy(
        char*& dest,
        char const* end) noexcept = 0;
};

//------------------------------------------------

// iterates segments in an
// plain path string
struct BOOST_SYMBOL_VISIBLE
    path_iter :
    public any_segments_iter
{
    explicit
    path_iter(
        string_view s) noexcept;

private:
    string_view s_;
    std::size_t n_ = 0;
    char const* p_ = nullptr;
    char const* p0_;
    char const* end_;

    static string_view clean(string_view s) noexcept;
    void increment() noexcept;
    void rewind() noexcept override;
    bool measure(std::size_t&) noexcept override;
    void copy(char*&, char const*) noexcept override;
};

//------------------------------------------------

// iterates segments in an
// encoded path string
struct BOOST_SYMBOL_VISIBLE
    path_encoded_iter
    : public any_segments_iter
{
    explicit
    path_encoded_iter(
        pct_string_view s) noexcept;

private:
    string_view s_;
    std::size_t n_ = 0;
    char const* p_ = nullptr;
    char const* p0_;
    char const* end_;

    static string_view clean(pct_string_view s) noexcept;
    void increment() noexcept;
    void rewind() noexcept override;
    bool measure(std::size_t&) noexcept override;
    void copy(char*&, char const*) noexcept override;
};

//------------------------------------------------
//
// segments_iter
//
//------------------------------------------------

class segments_iter_base
{
protected:
    BOOST_URL_DECL static void measure_impl(
        string_view, std::size_t&) noexcept;
    BOOST_URL_DECL static void copy_impl(
        string_view, char*&, char const*) noexcept;
};

// iterates segments in a
// plain segment range
template<class FwdIt>
struct segments_iter
    : any_segments_iter
    , segments_iter_base
{
    BOOST_STATIC_ASSERT(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::reference,
            string_view>::value);

    segments_iter(
        FwdIt first,
        FwdIt last) noexcept
        : it_(first)
        , it0_(first)
        , end_(last)
    {
        if (first != last)
            front = *first;
    }

private:
    FwdIt it_;
    FwdIt it0_;
    FwdIt end_;

    void
    rewind() noexcept override
    {
        it_ = it0_;
    }

    bool
    measure(
        std::size_t& n) noexcept override
    {
        if(it_ == end_)
            return false;
        measure_impl(*it_, n);
        ++it_;
        return true;
    }

    void
    copy(
        char*& dest,
        char const* end
            ) noexcept override
    {
        copy_impl(*it_, dest, end);
        ++it_;
    }
};

//------------------------------------------------
//
// segments_encoded_iter
//
//------------------------------------------------

// Validating and copying from
// a string of encoded segments
class segments_encoded_iter_base
{
protected:
    BOOST_URL_DECL static bool measure_impl(
        pct_string_view, std::size_t&) noexcept;
    BOOST_URL_DECL static void copy_impl(
        string_view, char*&, char const*) noexcept;
};

// iterates segments in an
// encoded segment range
template<class FwdIt>
struct segments_encoded_iter
    : public any_segments_iter
    , public segments_encoded_iter_base
{
    BOOST_STATIC_ASSERT(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::reference,
            pct_string_view>::value);

    BOOST_STATIC_ASSERT(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::reference,
            string_view>::value);

    segments_encoded_iter(
        FwdIt first,
        FwdIt last) noexcept
        : it_(first)
        , it0_(first)
        , end_(last)
    {
        if (it_ != end_)
            front = *first;
    }

private:
    FwdIt it_;
    FwdIt it0_;
    FwdIt end_;

    void
    rewind() noexcept override
    {
        it_ = it0_;
    }

    bool
    measure(
        std::size_t& n) noexcept override
    {
        if(it_ == end_)
            return false;
        if(! measure_impl(*it_, n))
            return false;
        ++it_;
        return true;
    }

    void
    copy(
        char*& dest,
        char const* end
            ) noexcept override
    {
        copy_impl(*it_, dest, end);
        ++it_;
    }
};

//------------------------------------------------

template<class FwdIt>
segments_iter<FwdIt>
make_segments_iter(
    FwdIt first, FwdIt last)
{
    return segments_iter<
        FwdIt>(first, last);
}

template<class FwdIt>
segments_encoded_iter<FwdIt>
make_segments_encoded_iter(
    FwdIt first, FwdIt last)
{
    return segments_encoded_iter<
        FwdIt>(first, last);
}

} // detail
} // urls
} // boost

#endif
