/*	==================
 *	FSTree_RsrcFile.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_RSRCFILE_HH
#define GENIE_FILESYSTEM_FSTREE_RSRCFILE_HH

// Genie
#include "Genie/FS/FSTree.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	FSTreePtr GetRsrcForkFSTree( const FSSpec& file, bool onServer );
	
}

#endif

