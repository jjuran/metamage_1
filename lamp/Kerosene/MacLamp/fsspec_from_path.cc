/*	===================
 *	fsspec_from_path.cc
 *	===================
 */

// Standard C
#include "errno.h"

// Standard C++
#include <string>

// POSIX
#include "sys/stat.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/Pathnames.hh"

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/Str.h"

// Kerosene
#include "MacFilenameFromUnixFilename.hh"

// MacLamp
#include "fsspec_from_path.hh"


//
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace K = Kerosene;
	
	int fsspec_from_path( const char*  pathname,
	                      FSSpec*      outFSS )
	{
		try
		{
			struct ::stat stat_buffer;
			
			int status = stat( pathname, &stat_buffer );
			
			if ( status == -1 )
			{
				if ( errno != ENOENT )
				{
					return status;
				}
				else
				{
					stat_buffer.st_mode = 0;  // not a dir
				}
			}
			
			std::string parent_pathname_string;
			std::string basename_string;
			
			bool is_dir = S_ISDIR( stat_buffer.st_mode );
			
			if ( !is_dir )
			{
				parent_pathname_string = io::get_preceding_directory( pathname );
				basename_string        = io::get_filename_string    ( pathname );
				
				basename_string = K::MacFilenameFromUnixFilename( basename_string );
				
				status = stat( parent_pathname_string.c_str(), &stat_buffer );
				
				if ( status == -1 )
				{
					return status;
				}
			}
			
			N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -stat_buffer.st_dev );
			N::FSDirID        dirID   = N::FSDirID       (  stat_buffer.st_ino );
			
			if ( vRefNum == 0 )
			{
				errno = EXDEV;
				
				return -1;
			}
			
			N::Str255 filename = basename_string;
			
			FSSpec dummy_FSSpec;
			
			if ( outFSS == NULL )
			{
				outFSS = &dummy_FSSpec;
			}
			
			*outFSS = N::FSMakeFSSpec( vRefNum, dirID, filename );
			
			return 0;
		}
		catch ( const std::bad_alloc& )
		{
			errno = ENOMEM;
		}
		catch ( ... )
		{
			errno = ENOENT;
		}
		
		return -1;
	}
	
	FSSpec make_fsspec_from_path( const char* pathname )
	{
		FSSpec spec;	
		
		p7::throw_posix_result( fsspec_from_path( pathname, &spec ) );
		
		return spec;
	}
//

