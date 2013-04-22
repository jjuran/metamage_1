/*
	screen.hh
	---------
*/

#ifndef SCREEN_HH
#define SCREEN_HH

// v68k
#include "v68k/memory.hh"


const uint32_t screen_addr = 0x0001A700;

const uint32_t screen_size = 21888;  // 512x342x1 / 8


int set_screen_backing_store_file( const char* path );

class screen_memory : public v68k::memory
{
	public:
		uint8_t* translate( uint32_t               addr,
		                    uint32_t               length,
		                    v68k::function_code_t  fc,
		                    v68k::memory_access_t  access ) const;
};


#endif
