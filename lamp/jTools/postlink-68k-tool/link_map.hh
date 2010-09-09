/*
	link_map.hh
	-----------
*/

#ifndef LD_LINKMAP_HH
#define LD_LINKMAP_HH

// Standard C
#include <stdint.h>


namespace tool
{
	
	uint32_t get_code_offset( const char* name );
	
	void read_link_map( const char* link_map_path );
	
}

#endif

