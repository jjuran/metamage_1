/*
	datum_access.cc
	---------------
*/

#include "vxo/datum_access.hh"


namespace vxo
{
	
	unsigned long size( const datum_storage& x )
	{
		/*
			For small strings, the margin (stored in the last byte) is the
			number of bytes by which the string could increase while still
			fitting in the internal buffer.  Length plus margin equals the
			maximum length for a small string (either 15 for 32-bit longs,
			or 31 for 64-bit).
		*/
		
		return is_small( x ) ? small_size( x )
		                     : x.alloc.length;
	}
	
	unsigned long capacity( const datum_storage& x )
	{
		return + is_small( x )        ? datum_max_offset
		       : x.alloc.capacity > 0 ? x.alloc.capacity
		       :                        x.alloc.length;
	}
	
	const char* begin( const datum_storage& x )
	{
		return is_small( x ) ? x.small
		                     : x.alloc.pointer + alloc_substr_offset( x );
	}
	
}
