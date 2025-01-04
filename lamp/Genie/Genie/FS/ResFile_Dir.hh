/*	==============
 *	ResFile_Dir.hh
 *	==============
 */

#ifndef GENIE_FS_RESFILEDIR_HH
#define GENIE_FS_RESFILEDIR_HH

// vfs
#include "vfs/node_ptr.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	vfs::node_ptr Get_ResFileDir_FSTree( const vfs::node*  parent,
	                                     const FSSpec&     file );
	
}

#endif
