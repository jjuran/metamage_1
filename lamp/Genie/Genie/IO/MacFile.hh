/*	==========
 *	MacFile.hh
 *	==========
 */

#ifndef GENIE_IO_MACFILE_HH
#define GENIE_IO_MACFILE_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
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
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	typedef nucleus::owned< Mac::FSFileRefNum > (*ForkOpener)( const FSSpec&, Mac::FSIOPerm );
	
	typedef vfs::node_ptr (*FileGetter)( const FSSpec& );
	
	
	vfs::filehandle_ptr
	//
	new_HFS_fork_handle( nucleus::owned< Mac::FSFileRefNum >&  refNum,
	                     int                                   flags,
	                     FileGetter                            getFile );
	
	vfs::filehandle_ptr
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   int            flags,
	                   ForkOpener     openFork,
	                   FileGetter     getFile );
	
}

#endif

