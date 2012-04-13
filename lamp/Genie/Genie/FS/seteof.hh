/*
	seteof.hh
	---------
*/

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	void seteof( const FSTree* node, off_t length );
	
}

