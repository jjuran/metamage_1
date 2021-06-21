/*
	FSSpec_from_stat.cc
	-------------------
*/

#include "mac_relix/FSSpec_from_stat.hh"

// POSIX
#include <sys/stat.h>

// Standard C
#include <errno.h>

// more-libc
#include "more/string.h"

// mac-types
#include "mac_types/FSSpec.hh"


namespace mac   {
namespace relix {
	
	int FSSpec_from_stat( const struct stat& stat_buffer, FSSpec& result )
	{
	#ifdef __RELIX__
		
		const unsigned name_length = stat_buffer.st_name[ 0 ];
		
		if ( stat_buffer.st_dev <= 0  ||  name_length == 0 )
		{
			errno = EXDEV;
			
			return -1;
		}
		
		if ( name_length > 31 )
		{
			errno = ENAMETOOLONG;
			
			return -1;
		}
		
		typedef mac::types::FSSpec FSSpec;
		
		FSSpec& fsspec = (FSSpec&) result;
		
		fsspec.vRefNum = -stat_buffer.st_dev;
		fsspec.parID   = stat_buffer.st_rdev;
		
		mempcpy( fsspec.name, stat_buffer.st_name, 1 + name_length );
		
	#endif
		
		return 0;
	}
	
}
}
