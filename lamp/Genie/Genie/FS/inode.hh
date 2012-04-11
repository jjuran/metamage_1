/*
	inode.hh
	--------
*/

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class FSTree;
	
	ino_t inode( const FSTree* node );
	
}

