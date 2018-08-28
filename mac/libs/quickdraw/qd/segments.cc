/*
	segments.cc
	-----------
*/

#include "qd/segments.hh"

// Standard C
#include <stdlib.h>

// Standard C++
#include <algorithm>


namespace quickdraw
{
	
	segments_box::segments_box( size_t capacity )  // bytes
	{
		its_data = (short*) malloc( capacity );
		its_size = 0;
	}
	
	segments_box::~segments_box()
	{
		free( its_data );
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
	
}
