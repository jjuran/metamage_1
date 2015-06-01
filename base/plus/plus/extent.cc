/*
	extent.cc
	---------
*/

#include "plus/extent.hh"

#include <stdlib.h>

// debug
#include "debug/assert.hh"


namespace plus
{
	
	struct extent_header
	{
		unsigned long refcount;
		unsigned long capacity;
	};
	
	/*
		These can be changed to use malloc() and free() when we're ready to
		take advantage of realloc().
	*/
	
	char* extent_alloc( unsigned long capacity )
	{
		unsigned long extent_size = sizeof (extent_header) + capacity;
		
		ASSERT( extent_size > capacity );
		
		if ( extent_size < capacity )
		{
			/*
				Overflow occurred.  The capacity is too large, and the
				address space can't fit the entire extent including the header.
				This is a programming error -- if you need gigabytes of RAM,
				this is the wrong facility to use.
			*/
			
			abort();
		}
		
		extent_header* header = (extent_header*) ::operator new( extent_size );
		
		header->refcount = 1;
		header->capacity = capacity;
		
		char* buffer = reinterpret_cast< char* >( header + 1 );
		
		return buffer;
	}
	
	static inline void extent_free( extent_header* header )
	{
		::operator delete( header );
	}
	
	static inline extent_header* header_from_buffer( const char* buffer )
	{
		// This casts away const, but it's only the characters that are
		// const, not the header.
		
		extent_header* header = (extent_header*) buffer - 1;
		
		ASSERT( long( header->refcount ) > 0 );
		
		return header;
	}
	
	void extent_add_ref( const char* buffer )
	{
		extent_header* header = header_from_buffer( buffer );
		
		++header->refcount;
	}
	
	void extent_release( const char* buffer )
	{
		extent_header* header = header_from_buffer( buffer );
		
		if ( --header->refcount == 0 )
		{
			extent_free( header );
		}
	}
	
	unsigned long extent_refcount( const char* buffer )
	{
		const extent_header* header = header_from_buffer( buffer );
		
		return header->refcount;
	}
	
}
