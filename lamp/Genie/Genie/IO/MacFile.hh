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

// vfs
#include "vfs/filehandle_ptr.hh"


namespace Genie
{
	
	vfs::filehandle_ptr
	//
	New_DataForkHandle( nucleus::owned< Mac::FSFileRefNum >&  refNum,
	                    int                                   flags );
	
	vfs::filehandle_ptr
	//
	New_RsrcForkHandle( nucleus::owned< Mac::FSFileRefNum >&  refNum,
	                    int                                   flags );
	
}

#endif

