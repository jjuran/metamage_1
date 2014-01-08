/*	=================
 *	FSSpecForkUser.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_FSSPECFORKUSER_HH
#define GENIE_FILESYSTEM_FSSPECFORKUSER_HH

// nucleus
#ifndef NUCLEUS_SHARED_HH
#include "nucleus/shared.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPERM_HH
#include "Mac/Files/Types/FSIOPerm.hh"
#endif

// vfs
#include "vfs/filehandle_ptr.hh"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	typedef nucleus::owned< Mac::FSFileRefNum > (*ForkOpener)( const FSSpec&, Mac::FSIOPerm );
	
	typedef vfs::filehandle_ptr (*HandleCreator)( const nucleus::shared< Mac::FSFileRefNum >&, int );
	
	vfs::filehandle_ptr
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   int            flags,
	                   ForkOpener     openFork,
	                   HandleCreator  createHandle );
	
}

#endif

