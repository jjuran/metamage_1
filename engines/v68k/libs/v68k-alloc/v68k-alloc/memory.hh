/*
	memory.hh
	---------
*/

#ifndef V68KALLOCMEMORY_HH
#define V68KALLOCMEMORY_HH

// v68k
#include "v68k/memory.hh"


namespace v68k  {
namespace alloc {

const uint32_t start = 0x00800000;  //  8 MiB
const uint32_t limit = 0x00F00000;  // 15 MiB

const int page_size_bits = 16;
const int page_size      = 1 << page_size_bits;  // 64K

const uint32_t n_alloc_bytes = limit - start;  // 7 MiB

const uint32_t n_alloc_pages = n_alloc_bytes / page_size;


uint32_t allocate_n_pages_for_existing_alloc_unchecked( uint32_t n, void* alloc );

inline uint32_t allocate( void* alloc, uint32_t size )
{
	if ( size & (page_size - 1) )
	{
		return 0;  // Not a page size multiple
	}
	
	const uint32_t n = size >> page_size_bits;
	
	if ( n > n_alloc_pages )
	{
		return 0;
	}
	
	return allocate_n_pages_for_existing_alloc_unchecked( n, alloc );
}

uint32_t allocate( uint32_t size );

void deallocate( uint32_t addr );

class memory : public v68k::memory
{
	public:
		uint8_t* translate( uint32_t               addr,
		                    uint32_t               length,
		                    v68k::function_code_t  fc,
		                    v68k::memory_access_t  access ) const;
};

}  // namespace alloc
}  // namespace v68k


#endif

