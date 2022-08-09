//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_RECYCLED_HPP
#define BOOST_URL_GRAMMAR_RECYCLED_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/detail/recycled.hpp>
#include <cstddef>
#include <type_traits>
#include <stddef.h> // ::max_align_t

namespace boost {
namespace urls {
namespace grammar {

/** Provides an aligned storage buffer aligned for T
*/
#ifdef BOOST_URL_DOCS
template<class T>
struct aligned_storage
{
    /** Return a pointer to the aligned storage area
    */
    void* addr() noexcept;

    /** Return a pointer to the aligned storage area
    */
    void const* addr() const noexcept;
};
#else
template<class T>
using aligned_storage =
    detail::aligned_storage_impl<
        detail::nearest_pow2(sizeof(T), 64),
            (alignof(::max_align_t) > alignof(T)) ?
                alignof(::max_align_t) : alignof(T)>;
#endif

//------------------------------------------------

/** A thread-safe collection of instances of T

    Instances of this type may be used to control
    where recycled instances of T come from when
    used with @ref recycled_ptr.

    @par Example
    @code
    static recycled< std::string > bin;

    recycled_ptr< std::string > ps( bin );

    // Put the string into a known state
    ps->clear();
    @endcode

    @see
        @ref recycled_ptr.
*/
template<class T>
class recycled
{
public:
    /** Destructor

        All recycled instances of T are destroyed.
        Undefined behavior results if there are
        any @ref recycled_ptr which reference this
        bin.
    */
    ~recycled();

    /** Constructor
    */
    constexpr recycled() = default;

private:
    template<class>
    friend class recycled_ptr;

    struct U
    {
        T t;
        U* next = nullptr;
    };

    struct report;

    U* try_acquire();
    void release(U* u) noexcept;

    U* head_ = nullptr;
    std::mutex m_;
};

//------------------------------------------------

/** A pointer to an instance of T

    This is a smart pointer container
    which acquires an instance of `T` upon
    construction. The instance is guaranteed
    to be in a valid, but unknown state.

    @par Example
    @code
    static recycled< std::string > bin;

    recycled_ptr< std::string > ps( bin );

    // Put the string into a known state
    ps->clear();
    @endcode

    @tparam T the type of object to acquire,
    which must be <em>DefaultConstructible</em>.
*/
template<class T>
class recycled_ptr
{
    // T must be default constructible!
    static_assert(
        std::is_default_constructible<T>::value,
        "T must be DefaultConstructible");

    friend class recycled<T>;

    using B = recycled<T>;
    using U = typename B::U;

    B* bin_ = nullptr;
    U* p_ = nullptr;

public:
    /** Destructor

        If a pointee exists, it is
        returned to the recycle bin.
    */
    ~recycled_ptr();

    /** Constructor

        Upon construction, the pointer
        will acquire exclusive access to
        an instance of `T` which is either
        recycled from the specified bin,
        or newly allocated. The instance
        will be guaranteed to be in an
        unknown but valid state.

        @par Example
        @code
        static recycled< std::string > bin;

        recycled_ptr< std::string > ps( bin );

        // Put the string into a known state
        ps->clear();
        @endcode

        @param bin The bin to use for recycling.

        @see
            @ref recycled.
    */
    explicit
    recycled_ptr(recycled<T>& bin);

    /** Constructor

        Upon construction, the pointer
        will acquire exclusive access to
        an instance of `T` which is either
        recycled from an implementation-defined
        global instance of `recycled<T>`,
        or newly allocated. The instance
        will be guaranteed to be in an
        unknown but valid state.

        @par Example
        @code
        recycled_ptr< std::string > ps;

        // Put the string into a known state
        ps->clear();
        @endcode

        @see
            @ref recycled.
    */
    recycled_ptr();

    /** Constructor

        After the move, the only valid operation
        on the moved-from object is destruction.
    */
    recycled_ptr(
        recycled_ptr&&) noexcept;

    /** Assignment (deleted)
    */
    recycled_ptr& operator=(
        recycled_ptr&&) = delete;

    /** Return true if this does not own an object

        This can only happen after this
        becomes moved-from.
    */
    bool
    empty() const noexcept
    {
        return p_ == nullptr;
    }

    /** Return the pointed-to object
    */
    T* get() const noexcept
    {
        return &p_->t;
    }

    /** Return the pointed-to object
    */
    T* operator->() const noexcept
    {
        return get();
    }

    /** Return the pointed-to object
    */
    T& operator*() const noexcept
    {
        return *get();
    }
};

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/recycled.hpp>

#endif
