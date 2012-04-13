/*
	open.hh
	-------
*/

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	IOPtr open( const FSTree* it, int flags, mode_t mode );
	
}

