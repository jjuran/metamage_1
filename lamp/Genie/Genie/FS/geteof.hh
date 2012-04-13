/*
	geteof.hh
	---------
*/

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	off_t geteof( const FSTree* node );
	
}

