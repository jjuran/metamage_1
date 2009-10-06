/*	===================
 *	fsspec_from_path.cc
 *	===================
 */

// Standard C
#include "errno.h"

// POSIX
#include "sys/stat.h"

// poseven
#include "poseven/Errno.hh"

// MacLamp
#include "fsspec_from_path.hh"


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
		
		typedef UInt32 FSDirID;
		
		const FSVolumeRefNum vRefNum = -stat_buffer.st_dev;
		const FSDirID        parID   = stat_buffer.st_rdev;
		
		const unsigned name_length = stat_buffer.st_name[0];
		
		if ( vRefNum == 0  ||  name_length == 0 )
		{
			errno = EXDEV;
			
			return -1;
		}
		
		if ( name_length > 31 )
		{
			errno = ENAMETOOLONG;
			
			return -1;
		}
		
		if ( outFSS != NULL )
		{
			outFSS->vRefNum = vRefNum;
			outFSS->parID   = parID;
			
			std::memcpy( outFSS->name, stat_buffer.st_name, 1 + name_length );
		}
		
		return 0;
	}
	
	FSSpec make_fsspec_from_path( const char* pathname )
	{
		FSSpec spec;	
		
		p7::throw_posix_result( fsspec_from_path( pathname, &spec ) );
		
		return spec;
	}
//

