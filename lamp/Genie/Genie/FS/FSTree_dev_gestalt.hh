/*	=====================
 *	FSTree_dev_gestalt.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREEDEVGESTALT_HH
#define GENIE_FILESYSTEM_FSTREEDEVGESTALT_HH

// POSIX
#include "sys/stat.h"

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	struct dev_gestalt
	{
		static const mode_t perm = S_IRUSR;
		
		static vfs::filehandle_ptr open( const vfs::node* that, int flags, mode_t mode );
	};
	
}

#endif

