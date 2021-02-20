//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/detail/indexed_string.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class parts_str_test
{
	void
	test_indexed_string_view()
	{
	}

	void
	test_indexed_string()
	{
		{
			detail::alloc_storage<std::allocator<char>> a( { } );
			detail::indexed_string<1> ps(a);
			BOOST_TEST(ps.length_all() == 0);
			BOOST_TEST(ps.get_all().empty());
			BOOST_TEST(ps.length(0) == 0);
			BOOST_TEST(ps.get(0).empty());
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0), 0, 4)).copy("test");
			BOOST_TEST(ps.length_all() == 4);
			BOOST_TEST(ps.get_all() == "test");
			BOOST_TEST(ps.length(0) == 4);
			BOOST_TEST(ps.get(0) == "test");
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0) + 1, 2, 0));
			BOOST_TEST(ps.length_all() == 2);
			BOOST_TEST(ps.get_all() == "tt");
			BOOST_TEST(ps.length(0) == 2);
			BOOST_TEST(ps.get(0) == "tt");
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0), 1, 3)).copy("tes");
			BOOST_TEST(ps.length_all() == 4);
			BOOST_TEST(ps.get_all() == "test");
			BOOST_TEST(ps.length(0) == 4);
			BOOST_TEST(ps.get(0) == "test");
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0) + 4, 0, 3)).copy("ing");
			BOOST_TEST(ps.length_all() == 7);
			BOOST_TEST(ps.get_all() == "testing");
			BOOST_TEST(ps.length(0) == 7);
			BOOST_TEST(ps.get(0) == "testing");
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0), 0, 5)).copy("more ");
			BOOST_TEST(ps.length_all() == 12);
			BOOST_TEST(ps.get_all() == "more testing");
			BOOST_TEST(ps.length(0) == 12);
			BOOST_TEST(ps.get(0) == "more testing");
			ps.clear(0);
			BOOST_TEST(ps.length_all() == 0);
			BOOST_TEST(ps.get_all().empty());
			BOOST_TEST(ps.length(0) == 0);
			BOOST_TEST(ps.get(0).empty());
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0), 0, 17)).copy("even more testing");
			BOOST_TEST(ps.length_all() == 17);
			BOOST_TEST(ps.get_all() == "even more testing");
			BOOST_TEST(ps.length(0) == 17);
			BOOST_TEST(ps.get(0) == "even more testing");
			ps.clear_all();
			BOOST_TEST(ps.length_all() == 0);
			BOOST_TEST(ps.get_all().empty());
			BOOST_TEST(ps.length(0) == 0);
			BOOST_TEST(ps.get(0).empty());
		}

		{
			detail::alloc_storage<std::allocator<char>> a( { } );
			detail::indexed_string<3> ps(a);
			BOOST_TEST(ps.length_all() == 0);
			BOOST_TEST(ps.get_all().empty());
			BOOST_TEST(ps.length(0) == 0);
			BOOST_TEST(ps.get(0).empty());
			BOOST_TEST(ps.length(1) == 0);
			BOOST_TEST(ps.get(1).empty());
			BOOST_TEST(ps.ptr(1) == 0);
			BOOST_TEST(ps.length(2) == 0);
			BOOST_TEST(ps.get(2).empty());
			BOOST_TEST(ps.ptr(2) == 0);
			ps.change_part(ps.reserve_change_part(1, ps.ptr(1), 0, 4)).copy("test");
			BOOST_TEST(ps.length_all() == 4);
			BOOST_TEST(ps.get_all() == "test");
			BOOST_TEST(ps.length(0) == 0);
			BOOST_TEST(ps.get(0) == "");
			BOOST_TEST(ps.length(1) == 4);
			BOOST_TEST(ps.get(1) == "test");
			BOOST_TEST(ps.length(2) == 0);
			BOOST_TEST(ps.get(2).empty());
			ps.change_part(ps.reserve_change_part(2, ps.ptr(2), 0, 3)).copy("ing");
			BOOST_TEST(ps.length_all() == 7);
			BOOST_TEST(ps.get_all() == "testing");
			BOOST_TEST(ps.length(0) == 0);
			BOOST_TEST(ps.get(0).empty());
			BOOST_TEST(ps.length(1) == 4);
			BOOST_TEST(ps.get(1) == "test");
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0), 0, 5)).copy("more ");
			BOOST_TEST(ps.length_all() == 12);
			BOOST_TEST(ps.get_all() == "more testing");
			BOOST_TEST(ps.length(0) == 5);
			BOOST_TEST(ps.get(0) == "more ");
			BOOST_TEST(ps.length(1) == 4);
			BOOST_TEST(ps.get(1) == "test");
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.change_part(ps.reserve_change_part(1, ps.ptr(1), 4, 5)).copy("debug");
			BOOST_TEST(ps.length_all() == 13);
			BOOST_TEST(ps.get_all() == "more debuging");
			BOOST_TEST(ps.length(0) == 5);
			BOOST_TEST(ps.get(0) == "more ");
			BOOST_TEST(ps.length(1) == 5);
			BOOST_TEST(ps.get(1) == "debug");
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.change_part(ps.reserve_change_part(1, ps.ptr(2) - 1, 0, 1)).copy("g");
			BOOST_TEST(ps.length_all() == 14);
			BOOST_TEST(ps.get_all() == "more debugging");
			BOOST_TEST(ps.length(0) == 5);
			BOOST_TEST(ps.get(0) == "more ");
			BOOST_TEST(ps.length(1) == 6);
			BOOST_TEST(ps.get(1) == "debugg");
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0), 0, 3)).copy("no ");
			BOOST_TEST(ps.length_all() == 17);
			BOOST_TEST(ps.get_all() == "no more debugging");
			BOOST_TEST(ps.length(0) == 8);
			BOOST_TEST(ps.get(0) == "no more ");
			BOOST_TEST(ps.length(1) == 6);
			BOOST_TEST(ps.get(1) == "debugg");
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0) + 3, 5, 0));
			BOOST_TEST(ps.length_all() == 12);
			BOOST_TEST(ps.get_all() == "no debugging");
			BOOST_TEST(ps.length(0) == 3);
			BOOST_TEST(ps.get(0) == "no ");
			BOOST_TEST(ps.length(1) == 6);
			BOOST_TEST(ps.get(1) == "debugg");
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.clear(1);
			BOOST_TEST(ps.length_all() == 6);
			BOOST_TEST(ps.get_all() == "no ing");
			BOOST_TEST(ps.length(0) == 3);
			BOOST_TEST(ps.get(0) == "no ");
			BOOST_TEST(ps.length(1) == 0);
			BOOST_TEST(ps.get(1).empty());
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.change_part(ps.reserve_change_part(1, ps.ptr(1), 0, 14)).copy("testing, less ");
			BOOST_TEST(ps.length_all() == 20);
			BOOST_TEST(ps.get_all() == "no testing, less ing");
			BOOST_TEST(ps.length(0) == 3);
			BOOST_TEST(ps.get(0) == "no ");
			BOOST_TEST(ps.length(1) == 14);
			BOOST_TEST(ps.get(1) == "testing, less ");
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.change_part(ps.reserve_change_part(0, ps.ptr(0), 2, 4)).copy("more");
			BOOST_TEST(ps.length_all() == 22);
			BOOST_TEST(ps.get_all() == "more testing, less ing");
			BOOST_TEST(ps.length(0) == 5);
			BOOST_TEST(ps.get(0) == "more ");
			BOOST_TEST(ps.length(1) == 14);
			BOOST_TEST(ps.get(1) == "testing, less ");
			BOOST_TEST(ps.length(2) == 3);
			BOOST_TEST(ps.get(2) == "ing");
			ps.change_part(ps.reserve_change_part(2, ps.ptr(2), 0, 6)).copy("debugg");
			BOOST_TEST(ps.length_all() == 28);
			BOOST_TEST(ps.get_all() == "more testing, less debugging");
			BOOST_TEST(ps.length(0) == 5);
			BOOST_TEST(ps.get(0) == "more ");
			BOOST_TEST(ps.length(1) == 14);
			BOOST_TEST(ps.get(1) == "testing, less ");
			BOOST_TEST(ps.length(2) == 9);
			BOOST_TEST(ps.get(2) == "debugging");
		}

		{
			detail::alloc_storage<std::allocator<char>> a( { } );
			detail::indexed_string<3> ps1(a);
			ps1.change_part(ps1.reserve_change_part(0, ps1.ptr(0), 0, 5)).copy("Part0");
			ps1.change_part(ps1.reserve_change_part(1, ps1.ptr(1), 0, 4)).copy("Prt1");
			ps1.change_part(ps1.reserve_change_part(2, ps1.ptr(2), 0, 2)).copy("P2");
			BOOST_TEST(std::strcmp(ps1.c_str(), "Part0Prt1P2") == 0);
			{
				detail::indexed_string<3> ps2(a);
				ps2.copy(ps1, 0, 3);
				BOOST_TEST(std::strcmp(ps1.c_str(), ps2.c_str()) == 0);
			}
			{
				detail::indexed_string<3> ps2(a);
				ps2.change_part(ps2.reserve_change_part(0, ps2.ptr(0), 0, 2)).copy("P0");
				ps2.change_part(ps2.reserve_change_part(1, ps2.ptr(1), 0, 2)).copy("P1");
				ps2.change_part(ps2.reserve_change_part(2, ps2.ptr(2), 0, 2)).copy("P2");
				BOOST_TEST(std::strcmp(ps2.c_str(), "P0P1P2") == 0);
				ps2.copy(ps1, 0, 3);
				BOOST_TEST(std::strcmp(ps1.c_str(), ps2.c_str()) == 0);
				BOOST_TEST(ps2.get(0) == "Part0");
				BOOST_TEST(ps2.get(1) == "Prt1");
				BOOST_TEST(ps2.get(2) == "P2");
			}
			{
				detail::indexed_string<3> ps2(a);
				ps2.change_part(ps2.reserve_change_part(0, ps2.ptr(0), 0, 2)).copy("P0");
				ps2.change_part(ps2.reserve_change_part(1, ps2.ptr(1), 0, 2)).copy("P1");
				ps2.change_part(ps2.reserve_change_part(2, ps2.ptr(2), 0, 2)).copy("P2");
				BOOST_TEST(std::strcmp(ps2.c_str(), "P0P1P2") == 0);
				ps2.copy(ps1, 0, 2);
				BOOST_TEST(std::strcmp(ps1.c_str(), ps2.c_str()) == 0);
				BOOST_TEST(ps2.get(0) == "Part0");
				BOOST_TEST(ps2.get(1) == "Prt1");
				BOOST_TEST(ps2.get(2) == "P2");
			}
			{
				detail::indexed_string<3> ps2(a);
				ps2.change_part(ps2.reserve_change_part(0, ps2.ptr(0), 0, 2)).copy("P0");
				ps2.change_part(ps2.reserve_change_part(1, ps2.ptr(1), 0, 2)).copy("P1");
				ps2.change_part(ps2.reserve_change_part(2, ps2.ptr(2), 0, 2)).copy("P2");
				BOOST_TEST(std::strcmp(ps2.c_str(), "P0P1P2") == 0);
				ps2.copy(ps1, 0, 1);
				BOOST_TEST(std::strcmp(ps2.c_str(), "Part0P1P2") == 0);
				BOOST_TEST(ps2.get(0) == "Part0");
				BOOST_TEST(ps2.get(1) == "P1");
				BOOST_TEST(ps2.get(2) == "P2");
			}
			{
				detail::indexed_string<3> ps2(a);
				ps2.change_part(ps2.reserve_change_part(0, ps2.ptr(0), 0, 2)).copy("P0");
				ps2.change_part(ps2.reserve_change_part(1, ps2.ptr(1), 0, 2)).copy("P1");
				ps2.change_part(ps2.reserve_change_part(2, ps2.ptr(2), 0, 2)).copy("P2");
				BOOST_TEST(std::strcmp(ps2.c_str(), "P0P1P2") == 0);
				ps2.copy(ps1, 1, 2);
				BOOST_TEST(std::strcmp(ps2.c_str(), "P0Prt1P2") == 0);
				BOOST_TEST(ps2.get(0) == "P0");
				BOOST_TEST(ps2.get(1) == "Prt1");
				BOOST_TEST(ps2.get(2) == "P2");
			}
			{
				detail::indexed_string<3> ps2(a);
				ps2.change_part(ps2.reserve_change_part(0, ps2.ptr(0), 0, 2)).copy("P0");
				ps2.change_part(ps2.reserve_change_part(1, ps2.ptr(1), 0, 2)).copy("P1");
				ps2.change_part(ps2.reserve_change_part(2, ps2.ptr(2), 0, 5)).copy("Part2");
				BOOST_TEST(std::strcmp(ps2.c_str(), "P0P1Part2") == 0);
				ps2.copy(ps1, 2, 3);
				BOOST_TEST(std::strcmp(ps2.c_str(), "P0P1P2") == 0);
				BOOST_TEST(ps2.get(0) == "P0");
				BOOST_TEST(ps2.get(1) == "P1");
				BOOST_TEST(ps2.get(2) == "P2");
			}
			{
				detail::indexed_string<3> ps2(a);
				ps2.copy_all(ps1);
				BOOST_TEST(ps2.get_all() == "Part0Prt1P2");
				BOOST_TEST(ps2.get(0) == "Part0");
				BOOST_TEST(ps2.get(1) == "Prt1");
				BOOST_TEST(ps2.get(2) == "P2");
				detail::indexed_string<3> ps3(a);
				ps2.copy(ps3, 0, 1);
				BOOST_TEST(ps2.get_all() == "Prt1P2");
				BOOST_TEST(ps2.get(0) == "");
				BOOST_TEST(ps2.get(1) == "Prt1");
				BOOST_TEST(ps2.get(2) == "P2");
				ps2.copy(ps3, 2, 3);
				BOOST_TEST(ps2.get_all() == "Prt1");
				BOOST_TEST(ps2.get(0) == "");
				BOOST_TEST(ps2.get(1) == "Prt1");
				BOOST_TEST(ps2.get(2) == "");
				ps2.copy(ps3, 1, 2);
				BOOST_TEST(ps2.empty());
				BOOST_TEST(ps2.get_all() == "");
				BOOST_TEST(ps2.get(0) == "");
				BOOST_TEST(ps2.get(1) == "");
				BOOST_TEST(ps2.get(2) == "");
			}
			{
				detail::indexed_string<3> ps2(a);
				ps2.copy_all(ps1);
				{
					BOOST_TEST(ps2.get_all() == "Part0Prt1P2");
					BOOST_TEST(ps2.get(0) == "Part0");
					BOOST_TEST(ps2.get(1) == "Prt1");
					BOOST_TEST(ps2.get(2) == "P2");
					ps2.clear(0, 1);
					BOOST_TEST(ps2.get_all() == "Prt1P2");
					BOOST_TEST(ps2.get(0) == "");
					BOOST_TEST(ps2.get(1) == "Prt1");
					BOOST_TEST(ps2.get(2) == "P2");
				}
				ps2.copy_all(ps1);
				{
					BOOST_TEST(ps2.get_all() == "Part0Prt1P2");
					BOOST_TEST(ps2.get(0) == "Part0");
					BOOST_TEST(ps2.get(1) == "Prt1");
					BOOST_TEST(ps2.get(2) == "P2");
					ps2.clear(1, 3);
					BOOST_TEST(ps2.get_all() == "Part0");
					BOOST_TEST(ps2.get(0) == "Part0");
					BOOST_TEST(ps2.get(1) == "");
					BOOST_TEST(ps2.get(2) == "");
				}
				ps2.copy_all(ps1);
				{
					BOOST_TEST(ps2.get_all() == "Part0Prt1P2");
					BOOST_TEST(ps2.get(0) == "Part0");
					BOOST_TEST(ps2.get(1) == "Prt1");
					BOOST_TEST(ps2.get(2) == "P2");
					ps2.clear(1, 2);
					BOOST_TEST(ps2.get_all() == "Part0P2");
					BOOST_TEST(ps2.get(0) == "Part0");
					BOOST_TEST(ps2.get(1) == "");
					BOOST_TEST(ps2.get(2) == "P2");
				}
				ps2.copy_all(ps1);
				{
					BOOST_TEST(ps2.get_all() == "Part0Prt1P2");
					BOOST_TEST(ps2.get(0) == "Part0");
					BOOST_TEST(ps2.get(1) == "Prt1");
					BOOST_TEST(ps2.get(2) == "P2");
					ps2.clear(1, 1);
					BOOST_TEST(ps2.get_all() == "Part0Prt1P2");
					BOOST_TEST(ps2.get(0) == "Part0");
					BOOST_TEST(ps2.get(1) == "Prt1");
					BOOST_TEST(ps2.get(2) == "P2");
				}
			}
		}
	}

public:

    void
    run()
    {
		test_indexed_string_view();
		test_indexed_string();
    }
};

TEST_SUITE(parts_str_test, "boost.url.indexed_string");

} // urls
} // boost
