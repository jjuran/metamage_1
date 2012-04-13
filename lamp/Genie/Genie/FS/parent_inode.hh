/*
	parent_inode.hh
	---------------
*/

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	ino_t parent_inode( const FSTree* node );
	
}

