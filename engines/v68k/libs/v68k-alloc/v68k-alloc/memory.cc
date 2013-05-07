/*
	memory.cc
	---------
*/

#include "v68k-alloc/memory.hh"

// Standard C
#include <stdlib.h>


#pragma exceptions off


namespace v68k  {
namespace alloc {

static void* alloc_pages[ n_alloc_pages + 1 ];


static int find_n_pages_at( int n, void* alloc, int i )
{
	// Returns 0 on success, or next index to try
	
	void* const ante = (char*) alloc - page_size;
	void* const post = (char*) alloc + page_size;
	
	int j = i + 1 + n;
	
	if ( alloc_pages[ j ] == post )
	{
		return j;
	}
	
	while ( --j > i )
	{
		if ( alloc_pages[ j ] != NULL )
		{
			return j;
		}
	}
	
	if ( alloc_pages[ i ] == ante )
	{
		return i + 1;
	}
	
	return 0;
}

static int find_n_pages( int n, void* alloc )
{
	// Returns index of first page on success, otherwise 0
	
	const int end = n_alloc_pages - n;
	
	int i = 0;
	
	while ( i < end )
	{
		int next = find_n_pages_at( n, alloc, i );
		
		if ( next == 0 )
		{
			return i + 1;
		}
		
		i = next;
	}
	
	return 0;
}

uint32_t allocate_n_pages_for_existing_alloc_unchecked( uint32_t n, void* alloc )
{
	if ( alloc == NULL )
	{
		return 0;  // NULL
	}
	
	int i = find_n_pages( n, alloc );
	
	if ( i == 0 )
	{
		return 0;
	}
	
	const uint32_t result = start + i * page_size;
	
	for ( const int end = i + n;  i != end;  ++i )
	{
		alloc_pages[ i ] = alloc;
		
		alloc = (char*) alloc + page_size;
	}
	
	return result;
}

uint32_t allocate( uint32_t size )
{
	if ( size > n_alloc_bytes )
	{
		return 0;  // NULL
	}
	
	const int n = (size + page_size - 1) / page_size;  // round up
	
	void* alloc = calloc( n, page_size );
	
	const uint32_t result = allocate_n_pages_for_existing_alloc_unchecked( n, alloc );
	
	if ( result == 0 )
	{
		free( alloc );
	}
	
	return result;
}

void deallocate( uint32_t addr )
{
	if ( start <= addr  &&  addr < limit )
	{
		int i = (addr - start) / page_size;
		
		void* alloc = alloc_pages[ i ];
		
		if ( alloc != NULL  &&  alloc != (void*) -1L )
		{
			free( alloc );
			
			do
			{
				alloc_pages[ i++ ] = NULL;
				
				alloc = (char*) alloc + page_size;
			}
			while ( alloc_pages[ i ] == alloc );
		}
	}
}


uint8_t* memory::translate( uint32_t               addr,
                            uint32_t               length,
                            v68k::function_code_t  fc,
                            v68k::memory_access_t  access ) const
{
	if ( access == mem_exec )
	{
		// alloc memory is not executable, for now
		
		return 0;  // NULL
	}
	
	if ( addr < start  ||  addr >= limit )
	{
		return 0;
	}
	
	addr -= start;
	
	const int index  = addr / page_size;
	const int offset = addr % page_size;
	
	void* page_alloc = alloc_pages[ index ];
	
	if ( page_alloc == NULL  ||  page_alloc == (void*) -1L )
	{
		// Address is not mapped, or reserved
		
		return 0;
	}
	
	if ( offset + length > page_size  &&  alloc_pages[ index + 1 ] != (char*) page_alloc + page_size )
	{
		// Access runs off end of block
		
		return 0;
	}
	
	return (uint8_t*) page_alloc + offset;
}

}  // namespace alloc
}  // namespace v68k

