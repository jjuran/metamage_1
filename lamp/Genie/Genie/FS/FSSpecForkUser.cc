/*	=================
 *	FSSpecForkUser.cc
 *	=================
 */

#include "Genie/FS/FSSpecForkUser.hh"

// POSIX
#include "fcntl.h"

// vfs
#include "vfs/filehandle.hh"


namespace Genie
{
	
	namespace n = nucleus;
	
	
	vfs::filehandle_ptr
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   int            flags,
	                   ForkOpener     openFork,
	                   HandleCreator  createHandle )
	{
		Mac::FSIOPermissions rdPerm = Mac::FSIOPermissions( flags + 1 - O_RDONLY  &  FREAD  );
		Mac::FSIOPermissions wrPerm = Mac::FSIOPermissions( flags + 1 - O_RDONLY  &  FWRITE );
		
		n::owned< Mac::FSFileRefNum > fileHandle = openFork( fileSpec, rdPerm | wrPerm );
		
		return createHandle( fileHandle, flags );
	}
	
}

