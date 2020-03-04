//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_CONFIG_HPP
#define BOOST_URL_DETAIL_CONFIG_HPP

#ifndef BOOST_URL_STANDALONE
# include <boost/config.hpp>
# include <boost/assert.hpp>
# include <boost/optional.hpp>
# include <boost/static_assert.hpp>
# include <boost/throw_exception.hpp>
# include <boost/system/error_code.hpp>
# include <boost/system/system_error.hpp>
# include <boost/utility/string_view.hpp>
#else
# include <cassert>
# include <optional>
# include <string_view>
# include <system_error>
#endif
#include <stdint.h>

// detect 32/64 bit
#if UINTPTR_MAX == UINT64_MAX
# define BOOST_URL_ARCH 64
#elif UINTPTR_MAX == UINT32_MAX
# define BOOST_URL_ARCH 32
#else
# error Unknown or unsupported architecture, please open an issue
#endif

// VFALCO Copied from <boost/config.hpp>
//        This is a derivative work.
#ifdef __has_cpp_attribute
// clang-6 accepts [[nodiscard]] with -std=c++14, but warns about it -pedantic
# if __has_cpp_attribute(nodiscard) && !(defined(__clang__) && (__cplusplus < 201703L))
#  define BOOST_URL_NODISCARD [[nodiscard]]
# else
#  define BOOST_URL_NODISCARD
# endif
#else
# define BOOST_URL_NODISCARD
#endif

#ifndef BOOST_URL_FORCEINLINE
# ifdef _MSC_VER
#  define BOOST_URL_FORCEINLINE __forceinline
# else
#  define BOOST_URL_FORCEINLINE inline
# endif
#endif

// BOOST_NORETURN ---------------------------------------------//
// Macro to use before a function declaration/definition to designate
// the function as not returning normally (i.e. with a return statement
// or by leaving the function scope, if the function return type is void).
#if !defined(BOOST_NORETURN)
#  if defined(_MSC_VER)
#    define BOOST_NORETURN __declspec(noreturn)
#  elif defined(__GNUC__)
#    define BOOST_NORETURN __attribute__ ((__noreturn__))
#  elif defined(__has_attribute) && defined(__SUNPRO_CC) && (__SUNPRO_CC > 0x5130)
#    if __has_attribute(noreturn)
#      define BOOST_NORETURN [[noreturn]]
#    endif
#  elif defined(__has_cpp_attribute) 
#    if __has_cpp_attribute(noreturn)
#      define BOOST_NORETURN [[noreturn]]
#    endif
#  endif
#endif

#ifndef BOOST_SYMBOL_VISIBLE
#define BOOST_SYMBOL_VISIBLE
#endif

#ifndef BOOST_ASSERT
#define BOOST_ASSERT assert
#endif

#ifndef BOOST_STATIC_ASSERT
#define BOOST_STATIC_ASSERT( ... ) static_assert(__VA_ARGS__, #__VA_ARGS__)
#endif

#ifndef BOOST_FALLTHROUGH
#define BOOST_FALLTHROUGH [[fallthrough]]
#endif

#ifndef BOOST_THROW_EXCEPTION
# ifndef BOOST_NO_EXCEPTIONS
#  define BOOST_THROW_EXCEPTION(x) throw(x)
# else
#  define BOOST_THROW_EXCEPTION(x) do{}while(0)
# endif
#endif

#ifndef BOOST_URL_NO_SSE2
# if (defined(_M_IX86) && _M_IX86_FP == 2) || \
      defined(_M_X64) || defined(__SSE2__)
#  define BOOST_URL_USE_SSE2
# endif
#endif

#ifndef BOOST_URL_STANDALONE
# if defined(BOOST_URL_DOCS)
#  define BOOST_URL_DECL
# elif defined(BOOST_URL_HEADER_ONLY)
#  define BOOST_URL_DECL inline
# else
#  if (defined(BOOST_URL_DYN_LINK) || defined(BOOST_ALL_DYN_LINK)) && !defined(BOOST_URL_STATIC_LINK)
#   if defined(BOOST_URL_SOURCE)
#    define BOOST_URL_DECL  BOOST_SYMBOL_EXPORT
#    define BOOST_URL_BUILD_DLL
#   else
#    define BOOST_URL_DECL  BOOST_SYMBOL_IMPORT
#   endif
#  endif // shared lib
#  ifndef  BOOST_URL_DECL
#   define BOOST_URL_DECL
#  endif
#  if !defined(BOOST_URL_SOURCE) && !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_URL_NO_LIB)
#   define BOOST_LIB_NAME boost_uri
#   if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_URL_DYN_LINK)
#    define BOOST_DYN_LINK
#   endif
#   include <boost/config/auto_link.hpp>
#  endif
# endif
#else
# ifdef BOOST_URL_HEADER_ONLY
#  define BOOST_URL_DECL inline
# else
#  define BOOST_URL_DECL
# endif
#endif

// These macros are private, for tests, do not change
// them or else previously built libraries won't match.
#ifndef BOOST_URL_MAX_OBJECT_SIZE
# define BOOST_URL_NO_MAX_OBJECT_SIZE
# define BOOST_URL_MAX_OBJECT_SIZE  0x7ffffffe
#endif
#ifndef BOOST_URL_MAX_ARRAY_SIZE
# define BOOST_URL_NO_MAX_ARRAY_SIZE
# define BOOST_URL_MAX_ARRAY_SIZE   0x7ffffffe
#endif
#ifndef BOOST_URL_MAX_STRING_SIZE
# define BOOST_URL_NO_MAX_STRING_SIZE
# define BOOST_URL_MAX_STRING_SIZE  0x7ffffffe
#endif
#ifndef BOOST_URL_MAX_STACK_SIZE
# define BOOST_URL_NO_MAX_STACK_SIZE
# define BOOST_URL_MAX_STACK_SIZE  ((::size_t)(-1))
#endif
#ifndef BOOST_URL_PARSER_BUFFER_SIZE
# define BOOST_URL_NO_PARSER_BUFFER_SIZE
# define BOOST_URL_PARSER_BUFFER_SIZE 2048
#endif

namespace boost {
namespace urls {

#ifndef BOOST_URL_STANDALONE
using string_view = boost::string_view;
#else
using string_view = std::string_view;
#endif

namespace detail {

template<class...>
struct make_void
{
    using type =void;
};

template<class... Ts>
using void_t = typename
    make_void<Ts...>::type;

template<class T>
struct remove_const
{
    using type = T;
};

template<class T>
struct remove_const<T const>
{
    using type = T;
};

template<class T>
struct remove_reference
{
    using type = T;
};

template<class T>
struct remove_reference<T&>
{
    using type = T;
};

template<class T>
constexpr
typename remove_reference<T>::type&&
move(T&& t) noexcept
{
    return static_cast<typename
        remove_reference<T>::type&&>(t);
}

template<class T, class U>
inline
T
exchange(T& t, U u) noexcept
{
    T v = move(t);
    t = move(u);
    return v;
}

template<class T>
using is_string_viewish = typename std::enable_if<
    std::is_convertible<
        T const&, string_view>::value &&
    ! std::is_convertible<
        T const&, char const*>::value
            >::type;

/*  This is a derivative work, original copyright:

    Copyright Eric Niebler 2013-present

    Use, modification and distribution is subject to the
    Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)

    Project home: https://github.com/ericniebler/range-v3
*/
template<typename T>
struct static_const
{
    static constexpr T value {};
};
template<typename T>
constexpr T static_const<T>::value;

#define BOOST_URL_INLINE_VARIABLE(name, type) \
    namespace \
    { \
        constexpr auto& name = \
            ::boost::urls::detail::static_const<type>::value; \
    }

struct primary_template
{
};

template<class T>
using is_specialized =
    std::integral_constant<bool,
        ! std::is_base_of<primary_template, T>::value>;

template<class T>
using remove_cr =
    typename remove_const<
    typename remove_reference<T>::type>::type;

} // detail
} // urls
} // boost

#endif
