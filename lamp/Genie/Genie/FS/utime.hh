/*
	utime.hh
	--------
*/

// Genie
#include "Genie/FS/FSTree_fwd.hh"


// time.h
struct timespec;


namespace Genie
{
	
	void utime( const FSTree* node, const struct timespec* times );
	
}

