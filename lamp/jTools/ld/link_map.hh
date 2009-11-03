/*
	link_map.hh
	-----------
	
	Copyright 2009, Joshua Juran
*/

#ifndef LD_LINKMAP_HH
#define LD_LINKMAP_HH

// Standard C
#include <stdint.h>


namespace tool
{
	
	uint32_t get_code_offset( const char* name );
	
	void record_symbolics( const char* output_path, unsigned path_size );
	
}

#endif

