/*
	validity.cc
	-----------
*/

#include "raster/validity.hh"

// POSIX
#include <sys/stat.h>

// Standard C
#include <errno.h>

// raster
#include "raster/raster.hh"


namespace raster
{
	
	off_t size_of_file_if_valid( int fd )
	{
		struct stat st;
		
		int nok = fstat( fd, &st );
		
		if ( nok )
		{
			return -1;
		}
		
		if ( ! S_ISREG( st.st_mode ) )
		{
			errno = EBADF;
			return -1;
		}
		
		if ( (st.st_size & 0x3) != 0 )
		{
			// File size must be a multiple of 4 bytes.
			errno = EINVAL;
			return -1;
		}
		
		return st.st_size;
	}
	
	bool is_valid_footer_size( size_t footer_size, off_t end )
	{
		if ( footer_size < sizeof (raster_metadata) + sizeof (uint32_t) )
		{
			return false;
		}
		
		if ( (footer_size & 0x3) != 0 )
		{
			// Footer size must be a multiple of 4 bytes.
			return false;
		}
		
		if ( footer_size > end )
		{
			return false;
		}
		
		return true;
	}
	
	bool is_valid_metadata( off_t footer_offset, const raster_desc& desc )
	{
		if ( desc.width == 0  ||  desc.height == 0 )
		{
			return false;
		}
		
		if ( desc.model >= Model_end_of_enumeration )
		{
			return false;
		}
		
		switch ( desc.weight )
		{
			default:
			case 0:
				return false;
			
			case 1:
			case 2:
			case 4:
			case 8:
			case 16:
			case 24:
			case 32:
				break;
		}
		
		const uint32_t max = 0xFFFF;
		
		if ( desc.width > max  ||  desc.height > max  ||  desc.stride > max )
		{
			// TODO:  Test width * height and height * stride for overflow.
			return false;
		}
		
		if ( desc.width * desc.weight > desc.stride * 8 )
		{
			// If stride is zero, it's caught here.
			return false;
		}
		
		if ( desc.frame > desc.extra )
		{
			return false;
		}
		
		const uint32_t image_size = desc.height * desc.stride;
		
		const unsigned frame_count = 1 + desc.extra;
		
		if ( desc.extra  &&  image_size > 0xFFFFFFFF / frame_count )
		{
			return false;
		}
		
		const uint32_t raster_size = image_size * frame_count;
		
		if ( raster_size > footer_offset )
		{
			return false;
		}
		
		return true;
	}
	
}
