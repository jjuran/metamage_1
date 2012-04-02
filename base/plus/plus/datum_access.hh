/*
	datum_access.hh
	---------------
*/

#ifndef PLUS_DATUMACCESS_HH
#define PLUS_DATUMACCESS_HH

// plus
#include "plus/datum_storage.hh"


namespace plus
{
	
	inline signed char margin( const datum_storage& x )
	{
		return x.small[ datum_max_offset ];
	}
	
	inline bool is_small( const datum_storage& x )
	{
		return margin( x ) >= 0;
	}
	
	inline unsigned long small_size( const datum_storage& x )
	{
		return datum_max_offset - x.small[ datum_max_offset ];
	}
	
	inline unsigned long alloc_substr_offset( const datum_storage& x )
	{
		return x.alloc.capacity >= 0 ? 0
		                             : -x.alloc.capacity;
	}
	
	unsigned long size( const datum_storage& x );
	
	unsigned long capacity( const datum_storage& x );
	
	const char* begin( const datum_storage& x );
	
	inline char* begin( datum_storage& x )
	{
		typedef const datum_storage& const_reference;
		
		return (char*) begin( const_reference( x ) );
	}
	
}

#endif

