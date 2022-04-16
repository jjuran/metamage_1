/*
	memory.cc
	---------
*/

#include "v68k-alloc/memory.hh"

// Standard C
#include <stdlib.h>

// log-of-war
#include "logofwar/report.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k  {
namespace alloc {

static void* alloc_pages[ n_alloc_pages + 1 ];


static int find_n_pages_at( int n, void* alloc, int i )
{
	// Returns 0 on success, or next index to try
	
	void* const ante = (char*) alloc - page_size;
	void* const post = (char*) alloc + page_size * n;
	
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

void* deallocate_existing( uint32_t addr )
{
	if ( start <= addr  &&  addr < limit )
	{
		int i = (addr - start) / page_size;
		
		void* alloc = alloc_pages[ i ];
		
		if ( i > 0 )
		{
			void* prev = alloc_pages[ i - 1 ];
			
			if ( (char*) alloc - (char*) prev == page_size )
			{
				ERROR = "dealloc: pointer is in middle of block -- skipping";
				
				return NULL;
			}
		}
		
		if ( alloc != NULL  &&  alloc != (void*) -1L )
		{
			void* next = alloc;
			
			do
			{
				alloc_pages[ i++ ] = NULL;
				
				next = (char*) next + page_size;
			}
			while ( alloc_pages[ i ] == next );
			
			return alloc;
		}
	}
	
	return NULL;
}

void deallocate( uint32_t addr )
{
	free( deallocate_existing( addr ) );
}

static
unsigned verify_n_pages( unsigned first, unsigned n )
{
	if ( first >= n_alloc_pages  ||  first + n > n_alloc_pages )
	{
		return 0;
	}
	
	void* page_alloc = alloc_pages[ first ];
	
	if ( page_alloc == NULL  ||  page_alloc == (void*) -1L )
	{
		// Address is not mapped, or reserved
		
		return 0;
	}
	
	void* next = page_alloc;
	
	for ( int i = 1;  i < n; ++i )
	{
		next = (char*) next + page_size;
		
		if ( alloc_pages[ first + i ] != next )
		{
			return 0;
		}
	}
	
	return n;
}


uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( access == mem_exec )
	{
		// alloc memory is not executable, for now
		
		/*
			FIXME:  alloc memory must be executable for load().  We need a
			way to specify whether a region should be executable or not.
		*/
		
		//return 0;  // NULL
	}
	
	if ( addr < start  ||  addr >= limit )
	{
		return 0;
	}
	
	addr -= start;
	
	const int index  = addr / page_size;
	const int offset = addr % page_size;
	
	const unsigned count = (offset + length - 1) / page_size + 1;
	
	if ( ! verify_n_pages( index, count ) )
	{
		// Access runs off end of block
		
		return 0;
	}
	
	void* page_alloc = alloc_pages[ index ];
	
	return (uint8_t*) page_alloc + offset;
}

}  // namespace alloc
}  // namespace v68k
