/*
	extent.cc
	---------
*/

#include "plus/extent.hh"

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
		ASSERT( long( capacity ) >= 0 );
		
		capacity += sizeof (extent_header);
		
		extent_header* header = (extent_header*) ::operator new( capacity );
		
		header->refcount = 1;
		header->capacity = capacity;
		
		char* buffer = reinterpret_cast< char* >( header + 1 );
		
		return buffer;
	}
	
	static inline void extent_free( extent_header* header )
	{
		::operator delete( header );
	}
	
	static extent_header* header_from_buffer( const char* buffer )
	{
		// This casts away const, but it's only the characters that are
		// const, not the header.
		
		extent_header* header = (extent_header*) buffer - 1;
		
		return header;
	}
	
	void extent_add_ref( const char* buffer )
	{
		// This casts away const, but it's only the characters that are
		// const, not the header.
		
		extent_header* header = header_from_buffer( buffer );
		
		++header->refcount;
		
		ASSERT( long( header->refcount ) >= 0 );
	}
	
	void extent_release( const char* buffer )
	{
		extent_header* header = header_from_buffer( buffer );
		
		ASSERT( long( header->refcount ) > 0 );
		
		if ( --header->refcount == 0 )
		{
			extent_free( header );
		}
	}
	
	unsigned long extent_refcount( const char* buffer )
	{
		const extent_header* header = header_from_buffer( buffer );
		
		ASSERT( long( header->refcount ) > 0 );
		
		return header->refcount;
	}
	
}
