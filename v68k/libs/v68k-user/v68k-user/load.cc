/*
	load.cc
	-------
*/

#include "v68k-user/load.hh"

// v68k
#include "v68k/endian.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k {
namespace user {

bool install_exception_handler( os_load_spec&    os,
                                int              index,
                                const uint16_t*  handler,
                                uint32_t         handler_size )
{
	const uint16_t* begin = handler;
	const uint16_t* end   = handler + handler_size / sizeof (uint16_t);
	
	if ( handler_size > os.mem_size - os.mem_used )
	{
		return false;
	}
	
	uint32_t* vectors = (uint32_t*) os.mem_base;  // reinterpret_cast
	
	vectors[ index ] = big_longword( os.mem_used );
	
	uint16_t* dest = (uint16_t*) (os.mem_base + os.mem_used);  // reinterpret_cast
	
	while ( begin < end )
	{
		*dest++ = big_word( *begin++ );
	}
	
	os.mem_used = (uint8_t*) dest - os.mem_base;  // reinterpret_cast
	
	return true;
}

}  // namespace user
}  // namespace v68k
