/*
	misc_method_set.hh
	------------------
*/

#ifndef VFS_METHODS_MISCMETHODSET_HH
#define VFS_METHODS_MISCMETHODSET_HH

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

#endif

