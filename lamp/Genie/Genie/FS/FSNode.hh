/*
	FSNode.hh
	---------
*/

#ifndef GENIE_FS_FSNODE_HH
#define GENIE_FS_FSNODE_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/string.hh"


namespace Genie
{
	
	struct FSNode
	{
		ino_t         inode;
		plus::string  name;
		
		FSNode()
		{
		}
		
		FSNode( ino_t i, const plus::string& n ) : inode( i ), name( n )
		{
		}
		
		~FSNode();
	};
	
}

#endif

