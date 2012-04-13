/*
	mkdir.hh
	--------
*/

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	void mkdir( const FSTree* node, mode_t mode );
	
}

