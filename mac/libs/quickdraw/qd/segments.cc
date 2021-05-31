/*
	segments.cc
	-----------
*/

#include "qd/segments.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// Standard C++
#include <algorithm>

// more-libc
#include "more/string.h"

// quickdraw
#include "qd/region_detail.hh"


namespace quickdraw
{
	
	malloc_segments_box::malloc_segments_box( size_t capacity )  // bytes
	:
		segments_box( (short*) malloc( capacity ) )
	{
	}
	
	malloc_segments_box::~malloc_segments_box()
	{
		free( begin() );
	}
	
	void segments_box::erase( short* it )
	{
		std::copy( it + 1, end(), it );
		
		--its_size;
	}
	
	void segments_box::insert( short* it, short h )
	{
		short* old_end = end();
		
		++its_size;
		
		std::copy_backward( it, old_end, end() );
		
		*it = h;
	}
	
	void segments_box::push_back( short h )
	{
		its_data[ its_size++ ] = h;
	}
	
	void segments_box::toggle( short h )
	{
		typedef segments_box::iterator Iter;
		
		Iter the_end = end();
		
		Iter it = std::lower_bound( begin(), the_end, h );
		
		if ( it != the_end  &&  *it == h )
		{
			erase( it );
		}
		else
		{
			insert( it, h );
		}
	}
	
	void segments_box::swap( segments_box& that )
	{
		using std::swap;
		
		swap( its_data, that.its_data );
		swap( its_size, that.its_size );
	}
	
	
	void xor_segments( segments_box&        a,
	                   segments_box const&  b )
	{
		typedef segments_box::const_iterator Iter;
		
		Iter it = b.begin();
		
		while ( it != b.end() )
		{
			xor_segments( a, *it++ );
		}
	}
	
	static inline
	size_t byte_distance( const void* begin, const void* end )
	{
		return (const char*) end - (const char*) begin;
	}
	
	static inline
	void copy_segments( short*& r, const short* begin, const short* end )
	{
		const size_t n = byte_distance( begin, end );
		
		r = (short*) mempcpy( r, begin, n );
	}
	
	static inline
	void copy_segments( short*& r, const segments_box& segments )
	{
		copy_segments( r, &*segments.begin(), &*segments.end() );
	}
	
	void accumulate_row( short*& r, short v, const segments_box& segments )
	{
		if ( ! segments.empty() )
		{
			*r++ = v;
			
			copy_segments( r, segments );
			
			if ( r[ -1 ] != Region_end )
			{
				*r++ = Region_end;
			}
		}
	}
	
}
