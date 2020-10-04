/*
	extent.cc
	---------
*/

#include "plus/extent.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// debug
#include "debug/assert.hh"

// plus
#include "plus/ref_count.hh"


namespace plus
{
	
	struct extent_header
	{
		typedef reference_count_t refcount_t;
		
		refcount_t    refcount;
		unsigned long capacity;
		destructor    dtor;
		destructor    dealloc;
	};
	
	/*
		These can be changed to use malloc() and free() when we're ready to
		take advantage of realloc().
	*/
	
	char* extent_alloc_nothrow( unsigned long capacity )
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
		
		extent_header* header = (extent_header*) malloc( extent_size );
		
		if ( header == NULL )
		{
			return NULL;
		}
		
		header->refcount = 1;
		header->capacity = capacity;
		header->dtor     = NULL;
		header->dealloc  = &free;
		
		char* buffer = reinterpret_cast< char* >( header + 1 );
		
		return buffer;
	}
	
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
		header->dtor     = NULL;
		header->dealloc  = &::operator delete;
		
		char* buffer = reinterpret_cast< char* >( header + 1 );
		
		return buffer;
	}
	
	char* extent_alloc_nothrow( unsigned long capacity, destructor dtor )
	{
		char* extent = extent_alloc_nothrow( capacity );
		
		memset( extent, '\0', capacity );
		
		extent_set_destructor( extent, dtor );
		
		return extent;
	}
	
	char* extent_alloc( unsigned long capacity, destructor dtor )
	{
		char* extent = extent_alloc( capacity );
		
		memset( extent, '\0', capacity );
		
		extent_set_destructor( extent, dtor );
		
		return extent;
	}
	
	static inline extent_header* header_from_buffer( const char* buffer )
	{
		// This casts away const, but it's only the characters that are
		// const, not the header.
		
		extent_header* header = (extent_header*) buffer - 1;
		
		ASSERT( long( header->refcount ) >= 0 );
		
		return header;
	}
	
	static char* extent_duplicate( const char* buffer )
	{
		extent_header* header = header_from_buffer( buffer );
		
		char* duplicate = extent_alloc( header->capacity );
		
		// TODO:  We'll often need a copy constructor as well.
		extent_set_destructor( duplicate, header->dtor );
		
		memcpy( duplicate, buffer, header->capacity );
		
		return duplicate;
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
			if ( destructor dtor = header->dtor )
			{
				dtor( (void*) buffer );
			}
			
			header->dealloc( header );
		}
	}
	
	static
	void extent_clear( void* buffer )
	{
		extent_header* header = header_from_buffer( (char*) buffer );
		
		memset( buffer, '\0', header->capacity );
	}
	
	char* extent_unshare( char* buffer )
	{
		extent_header* header = header_from_buffer( buffer );
		
		if ( header->refcount != 1 )
		{
			buffer = extent_duplicate( buffer );
			
			--header->refcount;
		}
		
		return buffer;
	}
	
	void extent_set_destructor( const char* buffer, destructor dtor )
	{
		extent_header* header = header_from_buffer( buffer );
		
		ASSERT( dtor == NULL  ||  header->dtor == NULL );
		
		header->dtor = dtor;
	}
	
	bool extent_set_selfdestruct( const char* buffer )
	{
		extent_header* header = header_from_buffer( buffer );
		
		if ( header->dtor != NULL )
		{
			return false;
		}
		
		header->dtor = &extent_clear;
		
		return true;
	}
	
	unsigned long extent_refcount( const char* buffer )
	{
		const extent_header* header = header_from_buffer( buffer );
		
		return header->refcount;
	}
	
	unsigned long extent_area( const char* buffer )
	{
		const extent_header* header = header_from_buffer( buffer );
		
		return sizeof (extent_header) + header->capacity;
	}
	
}
