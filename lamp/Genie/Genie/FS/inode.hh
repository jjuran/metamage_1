/*
	inode.hh
	--------
*/

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	ino_t inode( const FSTree* node );
	
}

