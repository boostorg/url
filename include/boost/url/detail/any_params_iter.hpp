//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_ANY_PARAMS_ITER_HPP
#define BOOST_URL_DETAIL_ANY_PARAMS_ITER_HPP

#include <boost/url/error_types.hpp>
#include <boost/url/param.hpp>
#include <boost/url/pct_string_view.hpp>
#include <boost/static_assert.hpp>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {
namespace detail {

//------------------------------------------------

struct not_empty_t
{
};

// Causes empty strings to transform
// into one-element range of params.
constexpr not_empty_t not_empty{};

class not_empty_param
{
    bool value_ = false;

public:
    constexpr
    not_empty_param() noexcept = default;

    constexpr
    not_empty_param(
        not_empty_t) noexcept
        : value_(true)
    {
    }

    operator bool() const noexcept
    {
        return value_;
    }
};

//------------------------------------------------
//
// any_params_iter
//
//------------------------------------------------

/*  An iterator to a type-erased,
    possibly encoded sequence of
    query params_view.
*/  
class BOOST_SYMBOL_VISIBLE
    any_params_iter
{
    string_view* s_ = nullptr;
    bool empty_ = false;

protected:
    explicit
    any_params_iter(
        bool empty,
        string_view* s = nullptr) noexcept
        : s_(s)
        , empty_(empty)
    {
    }

    static
    string_view&
    clean(string_view& s) noexcept
    {
        // prevent null
        if(s.data() == nullptr)
            s = string_view("", 0);
        return s;
    }

public:
    BOOST_URL_DECL
    virtual
    ~any_params_iter() noexcept = 0;

    // Return the input string or nullptr
    string_view*
    input() const noexcept
    {
        return s_;
    }

    // True if the sequence is empty
    bool
    empty() const noexcept
    {
        return empty_;
    }

    // Rewind the iterator to the beginning
    virtual
    void
    rewind() noexcept = 0;

    // Measure and increment the current
    // element. Returns false on end of
    // range or error. Upon success, n is
    // increased by the encoded size.
    virtual
    bool
    measure(
        std::size_t& n,
        error_code& ec) noexcept = 0;

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
//
// query_iter
//
//------------------------------------------------

// A string of plain query params
struct BOOST_SYMBOL_VISIBLE
    query_iter
    : any_params_iter
{
    BOOST_URL_DECL
    explicit
    query_iter(
        string_view s,
        not_empty_param ne = {}) noexcept;

private:
    string_view s_;
    std::size_t n_ = 0;
    char const* p_ = nullptr;

    void rewind() noexcept override;
    bool measure(std::size_t&, error_code&) noexcept override;
    void copy(char*&, char const*) noexcept override;
    void increment() noexcept;
};

//------------------------------------------------
//
// params_iter
//
//------------------------------------------------

class params_iter_base
{
protected:
    // return encoded size
    BOOST_URL_DECL
    static
    void
    measure_impl(
        param_view const& v,
        std::size_t& n) noexcept;

    // encode to dest
    BOOST_URL_DECL
    static
    void
    copy_impl(
        char*& dest,
        char const* end,
        param_view const& v) noexcept;
};

// A range of plain query params_view
template<class FwdIt>
struct params_iter
    : any_params_iter
    , private params_iter_base
{
    BOOST_STATIC_ASSERT(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::reference,
            param_view>::value);

    params_iter(
        FwdIt first,
        FwdIt last) noexcept
        : any_params_iter(
            first == last)
        , it0_(first)
        , it_(first)
        , end_(last)
    {
    }

private:
    FwdIt it0_;
    FwdIt it_;
    FwdIt end_;

    void
    rewind() noexcept override
    {
        it_ = it0_;
    }

    bool
    measure(
        std::size_t& n,
        error_code&) noexcept override
    {
        if(it_ == end_)
            return false;
        measure_impl(*it_++, n);
        return true;
    }

    void
    copy(
        char*& dest,
        char const* end) noexcept override
    {
        copy_impl(dest, end, *it_++);
    }
};

//------------------------------------------------
//
// params_encoded_iter
//
//------------------------------------------------

// Validating and copying from
// a string of encoded params
class params_encoded_iter_base
{
protected:
    BOOST_URL_DECL
    static
    bool
    measure_impl(
        param_pct_view const& v,
        std::size_t& n,
        error_code& ec) noexcept;

    BOOST_URL_DECL
    static
    void
    copy_impl(
        char*& dest,
        char const* end,
        param_view const& v) noexcept;
};

// A range of encoded query params_view
template<class FwdIt>
struct params_encoded_iter
    : any_params_iter
    , private params_encoded_iter_base
{
    BOOST_STATIC_ASSERT(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::reference,
            param_pct_view>::value);

    BOOST_STATIC_ASSERT(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::reference,
            param_view>::value);

    params_encoded_iter(
        FwdIt first,
        FwdIt last) noexcept
        : any_params_iter(
            first == last)
        , it0_(first)
        , it_(first)
        , end_(last)
    {
    }

private:
    FwdIt it0_;
    FwdIt it_;
    FwdIt end_;

    void
    rewind() noexcept override
    {
        it_ = it0_;
    }

    bool
    measure(
        std::size_t& n,
        error_code& ec) noexcept override
    {
        if(it_ == end_)
            return false;
        return measure_impl(
            *it_++, n, ec);
    }

    void
    copy(
        char*& dest,
        char const* end
            ) noexcept override
    {
        copy_impl(
            dest, end, *it_++);
    }
};

//------------------------------------------------
//
// param_value_iter
//
//------------------------------------------------

// An iterator which outputs
// exactly one value
struct param_value_iter
    : any_params_iter
{
    param_value_iter(
        std::size_t nk,
        string_view value,
        bool has_value) noexcept
        : any_params_iter(
            false, &value_)
        , nk_(nk)
        , value_(value)
        , has_value_(has_value)
    {
    }

private:
    std::size_t nk_ = 0;
    string_view value_;
    bool has_value_ = false;
    bool at_end_ = false;

    void rewind() noexcept override;
    bool measure(std::size_t&, error_code&) noexcept override;
    void copy(char*&, char const*) noexcept override;
};

//------------------------------------------------
//
// param_encoded_value_iter
//
//------------------------------------------------

// An iterator which outputs
// exactly one encoded value
struct param_encoded_value_iter
    : any_params_iter
{
    param_encoded_value_iter(
        std::size_t nk,
        pct_string_view value,
        bool has_value) noexcept
        : any_params_iter(false,
            &detail::ref(value_))
        , nk_(nk)
        , value_(value)
        , has_value_(has_value)
    {
    }

private:
    std::size_t nk_ = 0;
    pct_string_view value_;
    bool has_value_ = false;
    bool at_end_ = false;

    void rewind() noexcept override;
    bool measure(std::size_t&, error_code&) noexcept override;
    void copy(char*&, char const*) noexcept override;
};

//------------------------------------------------

template<class FwdIt>
params_iter<FwdIt>
make_params_iter(
    FwdIt first, FwdIt last)
{
    return params_iter<
        FwdIt>(first, last);
}

template<class FwdIt>
params_encoded_iter<FwdIt>
make_params_encoded_iter(
    FwdIt first, FwdIt last)
{
    return params_encoded_iter<
        FwdIt>(first, last);
}

} // detail
} // urls
} // boost

#endif
