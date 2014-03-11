/*	=================
 *	FSSpecForkUser.cc
 *	=================
 */

#include "Genie/FS/FSSpecForkUser.hh"

// POSIX
#include "fcntl.h"

// vfs
#include "vfs/filehandle.hh"

// Genie
#include "Genie/IO/MacFile.hh"


namespace Genie
{
	
	namespace n = nucleus;
	
	
	vfs::filehandle_ptr
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   int            flags,
	                   ForkOpener     openFork,
	                   FileGetter     getFile )
	{
		Mac::FSIOPermissions rdPerm = Mac::FSIOPermissions( flags + 1 - O_RDONLY  &  FREAD  );
		Mac::FSIOPermissions wrPerm = Mac::FSIOPermissions( flags + 1 - O_RDONLY  &  FWRITE );
		
		n::owned< Mac::FSFileRefNum > fileHandle = openFork( fileSpec, rdPerm | wrPerm );
		
		return new_HFS_fork_handle( fileHandle, flags, getFile );
	}
	
}

