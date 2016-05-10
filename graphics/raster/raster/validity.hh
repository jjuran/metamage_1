/*
	validity.hh
	-----------
*/

#ifndef RASTER_VALIDITY_HH
#define RASTER_VALIDITY_HH

// POSIX
#include <sys/types.h>


namespace raster
{
	
	struct raster_desc;
	
	off_t size_of_file_if_valid( int fd );
	
	bool is_valid_footer_size( size_t footer_size, off_t end );
	
	bool is_valid_metadata( off_t footer_offset, const raster_desc& desc );
	
}

#endif
