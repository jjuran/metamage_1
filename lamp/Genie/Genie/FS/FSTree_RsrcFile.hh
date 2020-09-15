/*	==================
 *	FSTree_RsrcFile.hh
 *	==================
 */

#ifndef GENIE_FS_FSTREERSRCFILE_HH
#define GENIE_FS_FSTREERSRCFILE_HH

// vfs
#include "vfs/node_ptr.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	vfs::node_ptr GetRsrcForkFSTree( const FSSpec& file );
	
}

#endif
