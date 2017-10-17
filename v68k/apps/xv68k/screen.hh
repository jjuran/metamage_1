/*
	screen.hh
	---------
*/

#ifndef SCREEN_HH
#define SCREEN_HH

// v68k
#include "v68k/memory.hh"


int set_screen_backing_store_file( const char* path, bool is_raster );

class screen_memory : public v68k::memory
{
	public:
		uint8_t* translate( uint32_t               addr,
		                    uint32_t               length,
		                    v68k::function_code_t  fc,
		                    v68k::memory_access_t  access ) const;
};


#endif
