//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/core/allocator_access.hpp>
#include <boost/core/allocator_traits.hpp>
#include <boost/core/empty_value.hpp>
#include <boost/type_traits/make_void.hpp>
#include <boost/url/any_allocator.hpp>
#include <boost/url/static_pool.hpp>
#include <boost/utility/result_of.hpp>

#include "test_suite.hpp"

#include <list>
#include <vector>

namespace boost {
namespace urls {

template <typename T>
T
get_value()
{
    return T{};
}

template <>
char
get_value<char>()
{
    return '_';
}

template <>
int
get_value<int>()
{
    return 42;
}

template <>
long
get_value<long>()
{
    return 42;
}

template <bool B, class T = void>
using enable_if_t = typename std::enable_if<
                                  B, T>::type;

template <typename T>
using pool_t = static_pool<sizeof(T) * 500>;

template <typename T, bool stateful>
using resource_t = typename std::conditional<
        stateful,
        pool_t<T>,
        empty_init_t>::type;

template <typename T>
using pool_allocator_t = static_pool_allocator<T>;

class any_allocator_test
{
public:
#define HAS_MEMBER_SFINAE(MEMBER)            \
    template <class T, class = void>         \
    struct has_##MEMBER : std::false_type    \
    {                                        \
    };                                       \
                                             \
    template <class T>                       \
    struct has_##MEMBER<                     \
        T,                                   \
        void_t<typename T::MEMBER>>          \
        : std::true_type                     \
    {                                        \
    };                                       \
                                             \
    template <class T>                       \
    static constexpr bool has_##MEMBER##_v() \
    {                                        \
        return has_##MEMBER<T>::value;       \
    }

    HAS_MEMBER_SFINAE(value_type)
    HAS_MEMBER_SFINAE(pointer)
    HAS_MEMBER_SFINAE(const_pointer)
    HAS_MEMBER_SFINAE(void_pointer)
    HAS_MEMBER_SFINAE(const_void_pointer)
    HAS_MEMBER_SFINAE(size_type)
    HAS_MEMBER_SFINAE(difference_type)
    HAS_MEMBER_SFINAE(
        propagate_on_container_copy_assignment)
    HAS_MEMBER_SFINAE(
        propagate_on_container_move_assignment)
    HAS_MEMBER_SFINAE(propagate_on_container_swap)
    HAS_MEMBER_SFINAE(is_always_equal)
    HAS_MEMBER_SFINAE(equal_operator)
    HAS_MEMBER_SFINAE(reference)
    HAS_MEMBER_SFINAE(const_reference)
#undef HAS_MEMBER_SFINAE

    template <class T, class U, class = void>
    struct has_rebind : std::false_type
    {
    };

    template <class T, class U>
    struct has_rebind<
        T,
        U,
        void_t<typename T::template rebind<U>>>
        : std::true_type
    {
    };

    template <class T, class = void>
    struct is_dereferenciable : std::false_type
    {
    };

    template <class T>
    struct is_dereferenciable<
        T,
        boost::void_t<decltype(*(
            boost::declval<T>()))>>
        : std::true_type
    {
    };

    template <typename T>
    static enable_if_t<
        has_equal_operator<T>::value,
        bool>
    is_equal_if_equal_exists(const T& a,
                             const T& b)
    {
        return a == b;
    }

    template <typename T>
    static enable_if_t<
        !has_equal_operator<T>::value,
        bool>
    is_equal_if_equal_exists(const T&, const T&)
    {
        return true;
    }

#define TEST_MEMBER(T, M) \
    BOOST_STATIC_ASSERT(has_##M##_v<T>())

    template <typename T>
    static void
    testTypes()
    {
        using alloc_t  = any_allocator<T>;
        using traits_t = allocator_traits<alloc_t>;

        // Mandatory for allocator
        TEST_MEMBER(alloc_t, value_type);
        BOOST_STATIC_ASSERT(
            has_rebind<alloc_t, int>::value);
        BOOST_STATIC_ASSERT(
            has_rebind<alloc_t,
                       unsigned long>::value);
        BOOST_STATIC_ASSERT(
            has_rebind<alloc_t, long[40]>::value);

        // Mandatory for allocator in GCC<=5 && Clang<=3.5
        TEST_MEMBER(alloc_t, size_type);
        TEST_MEMBER(alloc_t, difference_type);
        TEST_MEMBER(alloc_t, reference);
        TEST_MEMBER(alloc_t, const_reference);
        TEST_MEMBER(alloc_t, pointer);
        TEST_MEMBER(alloc_t, const_pointer);

        // Optional for allocator
        // Should be reachable by traits
        TEST_MEMBER(traits_t, pointer);
        TEST_MEMBER(traits_t, const_pointer);
        TEST_MEMBER(traits_t, void_pointer);
        TEST_MEMBER(traits_t, const_void_pointer);
        TEST_MEMBER(traits_t, size_type);
        TEST_MEMBER(traits_t, difference_type);
        TEST_MEMBER(
            traits_t,
            propagate_on_container_copy_assignment);
        TEST_MEMBER(
            traits_t,
            propagate_on_container_move_assignment);
        TEST_MEMBER(traits_t,
                    propagate_on_container_swap);
        TEST_MEMBER(traits_t, is_always_equal);
    }

    template <typename T>
    static void
    testPointer()
    {
        // Types
        using alloc_t  = any_allocator<T>;
        using traits_t = allocator_traits<alloc_t>;
        using value_type = typename traits_t::
            value_type;
        using pointer = typename traits_t::pointer;
        using const_pointer = typename traits_t::
            const_pointer;

        BOOST_STATIC_ASSERT(
            std::is_same<
                typename result_of<decltype (
                    &alloc_t::allocate)(
                    alloc_t*,
                    std::size_t)>::type,
                pointer>::value);

        BOOST_STATIC_ASSERT(
            is_dereferenciable<pointer>::value);

        BOOST_STATIC_ASSERT(
            std::is_same<
                decltype(*std::declval<pointer>()),
                typename traits_t::value_type&>::
                value);

        BOOST_STATIC_ASSERT(
            std::is_same<
                decltype(*std::declval<
                         const_pointer>()),
                const typename traits_t::
                    value_type&>::value);

        BOOST_STATIC_ASSERT(
            std::is_same<
                decltype(std::pointer_traits<
                         pointer>::
                             pointer_to(
                                 declval<
                                     value_type&>())),
                pointer>::value);

        BOOST_STATIC_ASSERT(
            std::is_nothrow_copy_constructible<
                alloc_t>::value);
    }

    template <typename T>
    static
    std::allocator<T>
    get_allocator(empty_init_t&)
    {
        return std::allocator<T>{};
    }

    template <typename T>
    static
    pool_allocator_t<T>
    get_allocator(pool_t<T>& memory_resource)
    {
        return pool_allocator_t<T>(memory_resource);
    }

    template <typename T, bool stateful>
    static void
    testRelationships()
    {
        using alloc_t = any_allocator<T>;

        BOOST_STATIC_ASSERT(
            std::is_same<
                typename alloc_t::is_always_equal,
                std::false_type>::value);

        resource_t<T, stateful> mem1;
        auto base1 = get_allocator<T>(mem1);
        alloc_t a1(base1);

        resource_t<T, stateful> mem2;
        auto base2 = get_allocator<T>(
            mem2);
        alloc_t a2(base2);

        BOOST_TEST(
            (a1 == a2) == (base1 == base2));
        BOOST_TEST(
            (a1 != a2) == (base1 != base2));

        // A a1(a): Copy-constructs a1 such that
        // a1 == a
        alloc_t a3(a1);
        BOOST_TEST(a3 == a1);

        // A a1 = a: Copy-constructs a1 such that
        // a1 == a
        alloc_t a4 = a1;
        BOOST_TEST(a4 == a1);

        // A a(b): Constructs a such that B(a)==b
        // and A(b)==a
        resource_t<T, stateful> mem3;
        auto base3 = get_allocator<T>(
            mem3);
        any_allocator<int> b1(base3);
        alloc_t            a5(b1);
        BOOST_TEST(any_allocator<int>(a5) == b1);
        BOOST_TEST(alloc_t(b1) == a5);

        // A a1(std::move(a)): Constructs a1 such
        // that it equals the prior value of a
        resource_t<T, stateful> mem4;
        auto base4 = get_allocator<T>(
            mem4);
        alloc_t a6(base4);
        alloc_t prior_a6(a6);
        alloc_t a7(std::move(a6));
        BOOST_TEST(a7 == prior_a6);

        // A a1 = std::move(a): Constructs a1 such
        // that it equals the prior value of a
        resource_t<T, stateful> mem5;
        auto base5 = get_allocator<T>(
            mem5);
        alloc_t a8(base5);
        alloc_t prior_a8(a8);
        alloc_t a9 = std::move(a8);
        BOOST_TEST(a9 == prior_a8);

        // A a(std::move(b)): Constructs a such
        // that it equals the prior value of A(b)
        resource_t<T, stateful> mem6;
        auto base6 = get_allocator<T>(
            mem6);
        any_allocator<int> b2(base6);
        alloc_t            prior_ab(b2);
        alloc_t            a10(std::move(b2));
        BOOST_TEST(a10 == prior_ab);
    }

    template <typename BaseAlloc>
    static void
    testLifetime(BaseAlloc& base)
    {
        using T = allocator_value_type<BaseAlloc>;
        using alloc_t  = any_allocator<T>;
        using traits_t = allocator_traits<alloc_t>;
        using value_type = allocator_value_type_t<
            alloc_t>;
        using pointer = allocator_pointer_t<
            alloc_t>;
        using size_type = allocator_size_type_t<
            alloc_t>;

        // a.allocate(n) - A::pointer - Allocates
        // storage suitable for an array object of
        // type T[n] and creates the array, but
        // does not construct array elements.
        BOOST_STATIC_ASSERT(
            std::is_same<
                typename result_of<decltype (
                    &alloc_t::allocate)(
                    alloc_t*,
                    std::size_t)>::type,
                pointer>::value);

        alloc_t a(base);
        T*      p1 = a.allocate(1);
        *p1        = get_value<T>();

        T* p2      = a.allocate(3);
        p2[0]      = get_value<T>();
        p2[1]      = get_value<T>();
        p2[2]      = get_value<T>();

        // a.allocate(n, cvp) - A::pointer - Same
        // as a.allocate(n), but may use cvp
        // (nullptr or a pointer obtained from
        // a.allocate()) in unspecified manner to
        // aid locality.
        T* p3      = traits_t::allocate(a, 1, p2);
        *p3        = get_value<T>();

        T* p4      = traits_t::allocate(a, 3, p2);
        p4[0]      = get_value<T>();
        p4[1]      = get_value<T>();
        p4[2]      = get_value<T>();

        // Deallocates storage pointed to p, which
        // must be a value returned by a previous
        // call to allocate or allocate_at_least
        // (since C++23) that has not been
        // invalidated by an intervening call to
        // deallocate. n must match the value
        // previously passed to allocate or be
        // between the request and returned number
        // of elements via allocate_at_least (may
        // be equal to either bound) (since C++23).
        BOOST_STATIC_ASSERT(
            std::is_same<
                typename result_of<decltype (
                    &alloc_t::deallocate)(
                    alloc_t*,
                    std::size_t)>::type,
                void>::value);
        a.deallocate(p4, 3);
        a.deallocate(p3, 1);
        a.deallocate(p2, 3);
        a.deallocate(p1, 1);

        // a.max_size() - A::size_type - The
        // largest value that can be passed to
        // A::allocate()
        BOOST_STATIC_ASSERT(
            std::is_same<
                typename result_of<decltype (
                    &static_cast<size_type (&)(
                        const alloc_t&)>(
                        traits_t::max_size))(
                    alloc_t&)>::type,
                size_type>::value);

        BOOST_TEST(
            (std::numeric_limits<size_type>::max)()
                / sizeof(value_type)
            == traits_t::max_size(a));

        // a.construct(xp, args) (optional) -
        // Constructs an object of type X in
        // previously-allocated storage at the
        // address pointed to by xp, using args as
        // the constructor arguments.
        p1 = traits_t::allocate(a, 1);
        traits_t::construct(a, p1, get_value<T>());
        BOOST_TEST(is_equal_if_equal_exists(
            *p1,
            get_value<T>()));

        // Destructs an object of type X pointed
        // to by xp, but does not deallocate any
        // storage.
        traits_t::destroy(a, p1);
        traits_t::deallocate(a, p1, 1);
    }

    template <typename BaseAlloc>
    static void
    testContainerOps(BaseAlloc& base)
    {
        using T = allocator_value_type<BaseAlloc>;
        using alloc_t  = any_allocator<T>;
        using traits_t = allocator_traits<alloc_t>;

        // Provides an instance of A to be used by
        // the container that is copy-constructed
        // from the one that uses `a` currently.
        // Usually returns either a copy of a or a
        // default-constructed A.
        BOOST_STATIC_ASSERT(
            std::is_same<
                typename result_of<decltype (
                    &static_cast<alloc_t (&)(
                        const alloc_t&)>(
                        traits_t::
                            select_on_container_copy_construction))(
                    alloc_t&)>::type,
                alloc_t>::value);
        alloc_t a1(base);
        alloc_t a2 = traits_t::
            select_on_container_copy_construction(
                a1);
        BOOST_TEST(a1 == a2);

        // std::true_type or derived from it if
        // the allocator of type A needs to be
        // copied when the container that uses it
        // is copy-assigned. Like,
        // polymorphic_allocator, any_allocator
        // does not propagate.
        BOOST_STATIC_ASSERT(
            std::is_same<
                typename std::allocator_traits<
                    alloc_t>::
                    propagate_on_container_copy_assignment,
                std::false_type>::value);

        // std::true_type or derived from it if
        // the allocator of type A needs to be
        // moved when the container that uses it
        // is move-assigned.
        BOOST_STATIC_ASSERT(
            std::is_same<
                typename std::allocator_traits<
                    alloc_t>::
                    propagate_on_container_move_assignment,
                std::false_type>::value);

        // std::true_type or derived from it if
        // the allocators of type A need to be
        // swapped when two containers that use
        // them are swapped.
        BOOST_STATIC_ASSERT(
            std::is_same<
                typename std::allocator_traits<
                    alloc_t>::
                    propagate_on_container_swap,
                std::false_type>::value);
    }

#ifndef BOOST_URL_NO_GCC_4_2_WORKAROUND
    template <typename T>
    static void
    testDefaultConstruct()
    {
        using alloc_t = any_allocator<T>;

        // Allocators are not required to be
        // default constructible, but some
        // compilers require it
        BOOST_STATIC_ASSERT(
            std::is_nothrow_default_constructible<
                alloc_t>::value);
        alloc_t a1;
        alloc_t a2;
        BOOST_TEST(a1 == a2);
        BOOST_TEST_NOT(a1 != a2);

        // A a1(a): Copy-constructs a1 such that
        // a1 == a
        alloc_t a3(a1);
        BOOST_TEST(a3 == a1);

        // A a1 = a: Copy-constructs a1 such that
        // a1 == a
        alloc_t a4 = a1;
        BOOST_TEST(a4 == a1);

        // A a(b): Constructs a such that B(a)==b
        // and A(b)==a
        any_allocator<int> b1;
        alloc_t            a5(b1);
        BOOST_TEST(any_allocator<int>(a5) == b1);
        BOOST_TEST(alloc_t(b1) == a5);

        // A a1(std::move(a)): Constructs a1 such
        // that it equals the prior value of a
        alloc_t a6;
        alloc_t prior_a6(a6);
        alloc_t a7(std::move(a6));
        BOOST_TEST(a7 == prior_a6);

        // A a1 = std::move(a): Constructs a1 such
        // that it equals the prior value of a
        alloc_t a8;
        alloc_t prior_a8(a8);
        alloc_t a9 = std::move(a8);
        BOOST_TEST(a9 == prior_a8);

        // A a(std::move(b)): Constructs a such
        // that it equals the prior value of A(b)
        any_allocator<int> b2;
        alloc_t            prior_ab(b2);
        alloc_t            a10(std::move(b2));
        BOOST_TEST(a10 == prior_ab);
    }
#else
    template <typename T, bool stateful>
    static void
    testDefaultConstruct()
    {
    }
#endif

    template <typename BaseAlloc>
    static void
    testVector(BaseAlloc& base)
    {
        using T = allocator_value_type<BaseAlloc>;
        using alloc_t = any_allocator<T>;
        alloc_t                 any_alloc(base);
        std::vector<T, alloc_t> v(any_alloc);
        v.reserve(v.capacity() + 1);
        v.reserve(v.capacity() + 1);
        v.shrink_to_fit();
        v.reserve(v.capacity() + 1);
    }

    template <typename BaseAlloc>
    static void
    testList(BaseAlloc& base)
    {
        using T = allocator_value_type<BaseAlloc>;
        using alloc_t = any_allocator<T>;
        alloc_t               any_alloc(base);
        std::list<T, alloc_t> v(any_alloc);
        v.push_back(get_value<T>());
        v.push_back(get_value<T>());
        v.push_back(get_value<T>());
        v.pop_back();
        v.pop_back();
        v.pop_back();
    }

    template <typename T>
    static void
    testRequirements()
    {
        testTypes<T>();
        testPointer<T>();
        testRelationships<T, false>();
        testRelationships<T, true>();
        {
            pool_t<T> mem;
            pool_allocator_t<T> base(mem);
            testLifetime(base);

            pool_allocator_t<T> alloc(mem);
            testLifetime(alloc);
        }
        {
            pool_t<T> mem;
            pool_allocator_t<T> base(mem);
            testContainerOps(base);

            std::allocator<T> alloc;
            testContainerOps(alloc);
        }
        testDefaultConstruct<T>();
        {
            pool_t<T> mem;
            pool_allocator_t<T> base(mem);
            testVector(base);

            std::allocator<T> alloc;
            testVector(alloc);
        }
        {
            pool_t<T> mem;
            pool_allocator_t<T> base(mem);
            testList(base);

            std::allocator<T> alloc;
            testList(alloc);
        }
    }

    template <std::size_t N>
    static enable_if_t<alignof(max_align_t) <= N,
                       void>
    testTryAlignedNamedRequirements()
    {
        testRequirements<max_align_t>();
    }

    template <std::size_t N>
        static enable_if_t
        < N<alignof(max_align_t), void>
        testTryAlignedNamedRequirements()
    {
        testRequirements<
            type_with_alignment<2>::type>();
        testTryAlignedNamedRequirements<N * 2>();
    }

    static void
    testAligned()
    {
        testTryAlignedNamedRequirements<1>();
    }

    struct S
    {
        long        a{ 1 };
        std::size_t b{ 1 };
        long        c{ 1 };
        int         d{ 1 };
        char        e{ 'a' };
    };

    static void
    testBasicString()
    {
        pool_t<char>           pool;
        pool_allocator_t<char> alloc(pool);
        any_allocator<char>    any_alloc(alloc);
        std::basic_string<char,
                          std::char_traits<char>,
                          any_allocator<char>>
            str;
        str.reserve(str.capacity() + 1);
        str.reserve(str.capacity() + 1);
        str.shrink_to_fit();
        str.reserve(str.capacity() + 1);
    }

    void
    run()
    {
        testBasicString();
        testRequirements<char>();
        testRequirements<int>();
        testRequirements<long>();
        testRequirements<max_align_t>();
        testAligned();
        testRequirements<S>();
    }
};

TEST_SUITE(any_allocator_test,
           "boost.url.any_allocator");

} // namespace urls
} // namespace boost
