/*
	chmod.hh
	--------
*/

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	void chmod( const FSTree* it, mode_t mode );
	
}

