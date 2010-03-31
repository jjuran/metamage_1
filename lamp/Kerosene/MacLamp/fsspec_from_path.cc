/*	===================
 *	fsspec_from_path.cc
 *	===================
 */

// Standard C
#include "errno.h"

// POSIX
#include "sys/stat.h"

// poseven
#include "poseven/types/errno_t.hh"

// MacLamp
#include "fsspec_from_path.hh"
#include "FSSpec_from_stat.h"


//
	
	namespace p7 = poseven;
	
	int fsspec_from_path( const char*  pathname,
	                      FSSpec*      outFSS )
	{
		struct ::stat stat_buffer;
		
		int status = stat( pathname, &stat_buffer );
		
		if ( status == -1  &&  errno != ENOENT )
		{
			return -1;
		}
		
		return FSSpec_from_stat( &stat_buffer, outFSS );
	}
	
	FSSpec make_fsspec_from_path( const char* pathname )
	{
		FSSpec spec;	
		
		p7::throw_posix_result( fsspec_from_path( pathname, &spec ) );
		
		return spec;
	}
//

