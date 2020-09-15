/*	==============
 *	ResFile_Dir.hh
 *	==============
 */

#ifndef GENIE_FS_RESFILEDIR_HH
#define GENIE_FS_RESFILEDIR_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"

// Files.h
struct FSSpec;


namespace Genie
{
	
	vfs::node_ptr Get_ResFileDir_FSTree( const vfs::node*     parent,
	                                     const plus::string&  name,
	                                     const FSSpec&        file );
	
}

#endif
