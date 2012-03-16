/*
	misc_method_set.hh
	------------------
*/

#ifndef GENIE_FS_MISCMETHODSET_HH
#define GENIE_FS_MISCMETHODSET_HH

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	typedef FSTreePtr (*parent_method)( const FSTree* );
	
	typedef ino_t (*inode_method)( const FSTree* );
	
	
	struct misc_method_set
	{
		parent_method  parent;
		inode_method   parent_inode;
		inode_method   inode;
	};
	
}

#endif

