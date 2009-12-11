/*	=================
 *	FSSpecForkUser.cc
 *	=================
 */

#include "Genie/FS/FSSpecForkUser.hh"

// POSIX
#include "fcntl.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	boost::shared_ptr< IOHandle >
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   OpenFlags      flags,
	                   ForkOpener     openFork,
	                   HandleCreator  createHandle )
	{
		N::FSIOPermissions rdPerm = N::FSIOPermissions( flags + 1  &  FREAD  );
		N::FSIOPermissions wrPerm = N::FSIOPermissions( flags + 1  &  FWRITE );
		
		NN::Shared< N::FSFileRefNum > fileHandle = openFork( fileSpec, rdPerm | wrPerm );
		
		return createHandle( fileHandle, flags );
	}
	
}

