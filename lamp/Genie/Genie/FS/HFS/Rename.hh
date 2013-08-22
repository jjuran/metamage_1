/*
	Genie/FS/HFS/Rename.hh
	----------------------
*/

#ifndef GENIE_FS_HFS_RENAME_HH
#define GENIE_FS_HFS_RENAME_HH

// vfs
#include "vfs/node_fwd.hh"

//#include <Files.h>
struct FSSpec;


namespace Genie
{
	
	void Rename_HFS( const FSSpec& srcFileSpec, const vfs::node& destFile );
	
}

#endif

