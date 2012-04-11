/*
	open.hh
	-------
*/

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	class FSTree;
	
	IOPtr open( const FSTree* node, int flags, mode_t mode );
	
}

