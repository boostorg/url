//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/basic_url.hpp>
#include <type_traits>

#include "test_suite.hpp"

namespace boost {
namespace urls {

template <bool always_equal>
struct alloc_base
{
};

template<>
struct alloc_base<true>
{
  using is_always_equal = std::true_type;
};
template<>
struct alloc_base<false>
{
  using is_always_equal = std::false_type;
};


template <typename T, bool always_equal>
struct statefull_allocator : public alloc_base<always_equal>
{
  using value_type = T;  
  using is_always_equal = typename alloc_base<always_equal>::is_always_equal;

  statefull_allocator() = default;
  template<typename U, bool paf>
  constexpr statefull_allocator(statefull_allocator<U, paf> const &) noexcept;


  T *allocate(std::size_t n)
  {
    auto p = static_cast<T*>(::operator new(sizeof(value_type) * n));
      return p;
  }

  void deallocate(T *p, size_t) noexcept
  {
     ::operator delete(p);
  }
};

template <typename T, bool b>
using is_true_traits = typename statefull_allocator<T, b>::is_always_equal;

template<typename A, bool always_equal>
bool operator==(statefull_allocator<A, always_equal> const &, statefull_allocator<A, always_equal> const &)
{ 
  auto const is_equal = is_true_traits<A, always_equal>();
  return is_equal; 
}

template<typename A, bool always_equal>
bool operator!=(statefull_allocator<A, always_equal> const &, statefull_allocator<A, always_equal> const &)
{ 
  auto const is_equal = is_true_traits<A, always_equal>();
  return !is_equal; 
}


template<typename Allocator>
class basic_url_test
{
public:
    test_suite::log_type log;
    using alloc_type = Allocator;

    using url = basic_url<Allocator>;

    void
    dump(url const& u)
    {
        log <<
            "href     : " << u.encoded_url() << "\n"
            "scheme   : " << u.scheme() << "\n"
            "user     : " << u.encoded_user() << "\n"
            "password : " << u.encoded_password() << "\n"
            "hostname : " << u.encoded_host() << "\n"
            "port     : " << u.port_part() << "\n" <<
            "path     : " << u.encoded_path() << "\n"
            "query    : " << u.query_part() << "\n"
            "fragment : " << u.fragment_part() << "\n"
            //"resource : " << u.encoded_resource() << "\n"
            ;
        log.flush();
    }

    void 
    testMoveConstruction()
    {
      url a("http://original.com");
      auto b = std::move(a);
      BOOST_TEST(b.encoded_url() == "http://original.com");
      BOOST_TEST(a.encoded_url() == "");
      b.set_host("newvalue.com");
      BOOST_TEST(b.encoded_url() == "http://newvalue.com");
      b.set_host("hello.com");
      BOOST_TEST(b.encoded_url() == "http://hello.com");
    }

    void 
    testCopyConstruction()
    {
      url a("http://original.com");
      auto b = a;
      BOOST_TEST(b.encoded_url() == "http://original.com");
      BOOST_TEST(a.encoded_url() == "http://original.com");
      b.set_host("newvalue.com");
      BOOST_TEST(b.encoded_url() == "http://newvalue.com");
      BOOST_TEST(a.encoded_url() == "http://original.com");
      b.set_host("hello.com");
      BOOST_TEST(b.encoded_url() == "http://hello.com");
    }

    void
    testMoveAssignement()
    {
      url a("http://original.com");
      url b("http://replaced.com");
      b = std::move(a);
      BOOST_TEST(b.encoded_url() == "http://original.com");
      BOOST_TEST(a.encoded_url() == "");
      b.set_host("newvalue.com");
      BOOST_TEST(b.encoded_url() == "http://newvalue.com");
      BOOST_TEST(a.encoded_url() == "");
      b.set_host("hello.com");
      BOOST_TEST(b.encoded_url() == "http://hello.com");
    }

    void
    testCopyAssignement()
    {
      url a("http://original.com");
      url b("http://replaced.com");
      b = a;
      BOOST_TEST(b.encoded_url() == "http://original.com");
      BOOST_TEST(a.encoded_url() == "http://original.com");
      b.set_host("newvalue.com");
      BOOST_TEST(b.encoded_url() == "http://newvalue.com");
      BOOST_TEST(a.encoded_url() == "http://original.com");
      b.set_host("hello.com");
      BOOST_TEST(b.encoded_url() == "http://hello.com");
    }

    void testSelfAssignement()
    {
      url a("http://original.com");
      a = a;
      BOOST_TEST(a.encoded_url() == "http://original.com");
      a.set_host("newvalue.com");
      BOOST_TEST(a.encoded_url() == "http://newvalue.com");
    }

    void testSwap()
    {
      {
        url a("http://original.com");
        url b("http://replaced.com");
        std::swap(a, b);
        BOOST_TEST(a.encoded_url() == "http://replaced.com");
        BOOST_TEST(b.encoded_url() == "http://original.com");
        b.set_host("newvalue.com");
        BOOST_TEST(a.encoded_url() == "http://replaced.com");
        BOOST_TEST(b.encoded_url() == "http://newvalue.com");
      }
      {
        url a("http://original.com");
        url b("http://replaced.com");
        std::swap(b, a);
        BOOST_TEST(a.encoded_url() == "http://replaced.com");
        BOOST_TEST(b.encoded_url() == "http://original.com");
        b.set_host("newvalue.com");
        BOOST_TEST(a.encoded_url() == "http://replaced.com");
        BOOST_TEST(b.encoded_url() == "http://newvalue.com");
      }
    }

    void
    run()
    {
	    testMoveConstruction();
        testCopyConstruction();
        testMoveAssignement();
        testCopyAssignement();
        testSelfAssignement();
        testSwap();
    }
};

using basic_url_test_std_allocator = basic_url_test<std::allocator<char>>;
using basic_url_test_statefull_allocator_not_equal = basic_url_test<statefull_allocator<char, false>>;

TEST_SUITE(basic_url_test_std_allocator, "boost.url.basic_url");
TEST_SUITE(basic_url_test_statefull_allocator_not_equal, "boost.url.basic_url");

} // urls
} // boost
