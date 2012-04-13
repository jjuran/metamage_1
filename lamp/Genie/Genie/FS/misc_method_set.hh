/*
	misc_method_set.hh
	------------------
*/

#ifndef GENIE_FS_MISCMETHODSET_HH
#define GENIE_FS_MISCMETHODSET_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	typedef node_ptr (*parent_method)( const node* );
	
	typedef ino_t (*inode_method)( const node* );
	
	
	struct misc_method_set
	{
		parent_method  parent;
		inode_method   parent_inode;
		inode_method   inode;
	};
	
}

namespace Genie
{
	
	using vfs::parent_method;
	using vfs::inode_method;
	
	using vfs::misc_method_set;
	
}

#endif

