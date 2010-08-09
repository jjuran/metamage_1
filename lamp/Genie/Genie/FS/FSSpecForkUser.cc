/*	=================
 *	FSSpecForkUser.cc
 *	=================
 */

#include "Genie/FS/FSSpecForkUser.hh"

// POSIX
#include "fcntl.h"


namespace Genie
{
	
	namespace n = nucleus;
	
	
	boost::shared_ptr< IOHandle >
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   OpenFlags      flags,
	                   ForkOpener     openFork,
	                   HandleCreator  createHandle )
	{
		Mac::FSIOPermissions rdPerm = Mac::FSIOPermissions( flags + 1 - O_RDONLY  &  FREAD  );
		Mac::FSIOPermissions wrPerm = Mac::FSIOPermissions( flags + 1 - O_RDONLY  &  FWRITE );
		
		n::shared< Mac::FSFileRefNum > fileHandle = openFork( fileSpec, rdPerm | wrPerm );
		
		return createHandle( fileHandle, flags );
	}
	
}

