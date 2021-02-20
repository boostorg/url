//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_PARTS_HPP
#define BOOST_URL_DETAIL_PARTS_HPP

#include <boost/url/host_type.hpp>
#include <boost/url/detail/char_type.hpp>
#include <boost/url/detail/indexed_string.hpp>
#include <boost/url/error.hpp>

namespace boost {
namespace urls {
namespace detail {

enum
{
    id_scheme = 0,  // trailing ':'
    id_user,        // leading "//"
    id_password,    // leading ':', trailing '@'
    id_host,
    id_port,        // leading ':'
    id_path,
    id_query,       // leading '?'
    id_frag,        // leading '#'
    id_end
};

struct parts_data
{
    std::size_t nseg = 0;
    std::size_t nparam = 0;
    host_type host = host_type::none;
};

using parts = part_index<id_end, parts_data>;
using parts_view = indexed_view<id_end, parts_data>;

class parts_string: public indexed_string<id_end, parts_data>
{
    using base = indexed_string<id_end, parts_data>;

public:

	BOOST_URL_NODISCARD
    int
    check_invariants() const noexcept
    {
        {
            auto const s = get(id_path);
            int n = 0;
            if( !s.empty() )
            {
                n = s.front() != '/';
                for( auto & c : s )
                    n += c == '/';
            }
            if( n != nseg )
                return __LINE__;
        }
        {
            auto const s = get(id_query);
            int n = 0;
            if( !s.empty() )
            {
                if( s.front() != '?' )
                    return __LINE__;
                ++n;
                for( auto & c : s )
                    n += c == '&';
            }
            if( n != nparam )
                return __LINE__;
        }
        if( (host==host_type::none) != get(id_host).empty() )
            return __LINE__;
        return 0;
    }

	explicit
    parts_string( storage& a ) noexcept:
		base(a)
	{
	}

    void
    clear_all() noexcept
    {
        base::clear_all();
        nseg = 0;
        nparam = 0;
        host = host_type::none;
    }

    void
    clear( int first, int last ) noexcept
    {
        base::clear( first, last );
        if( first <= id_path && last > id_path )
            nseg = 0;
        if( first <= id_query && last > id_query )
            nparam = 0;
        if( first <= id_host && last > id_host )
            host = host_type::none;
        BOOST_ASSERT(check_invariants() == 0);
    }

    void
    clear( int part ) noexcept
    {
        clear( part, part + 1 );
    }

    void
    copy( parts const & pt, string_view s, int first, int last )
    {
        base::copy( pt, s, first, last );
        if( first <= id_path && last > id_path )
            nseg = pt.nseg;
        if( first <= id_query && last > id_query )
            nparam = pt.nparam;
        if( first <= id_host && last > id_host )
            host = pt.host;
        BOOST_ASSERT(check_invariants() == 0);
    }

    void
    copy_all( parts const & pt, string_view s )
    {
        base::copy_all( pt, s );
        nseg = pt.nseg;
        nparam = pt.nparam;
        host = pt.host;
        BOOST_ASSERT(check_invariants() == 0);
    }
};

} // detail
} // urls
} // boost

#endif
