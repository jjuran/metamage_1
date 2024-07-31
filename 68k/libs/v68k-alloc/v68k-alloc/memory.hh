/*
	memory.hh
	---------
*/

#ifndef V68KALLOC_MEMORY_HH
#define V68KALLOC_MEMORY_HH

// v68k
#include "v68k/memory.hh"


namespace v68k  {
namespace alloc {

const uint32_t start = 0x00500000;  //  5 MiB
const uint32_t limit = 0x00E00000;  // 14 MiB

const int page_size_bits = 16;
const int page_size      = 1 << page_size_bits;  // 64K

const uint32_t n_alloc_bytes = limit - start;  // 9 MiB

const uint32_t n_alloc_pages = n_alloc_bytes / page_size;


int count_free_pages();

inline
uint32_t count_free_bytes()
{
	return count_free_pages() << page_size_bits;
}

uint32_t allocate_n_pages_for_existing_alloc_unchecked( uint32_t n, void* alloc );

inline uint32_t allocate_n_pages( void* alloc, uint32_t n )
{
	if ( n > n_alloc_pages )
	{
		return 0;
	}
	
	return allocate_n_pages_for_existing_alloc_unchecked( n, alloc );
}

inline uint32_t allocate( void* alloc, uint32_t size )
{
	if ( size & (page_size - 1) )
	{
		return 0;  // Not a page size multiple
	}
	
	const uint32_t n = size >> page_size_bits;
	
	return allocate_n_pages( alloc, n );
}

uint32_t allocate( uint32_t size );

void* deallocate_existing( uint32_t addr );

void deallocate( uint32_t addr );

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access );

}  // namespace alloc
}  // namespace v68k


#endif
