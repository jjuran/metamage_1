/*
	parent_inode.hh
	---------------
*/

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class FSTree;
	
	ino_t parent_inode( const FSTree* node );
	
}

