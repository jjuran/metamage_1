/*
	stat.hh
	-------
*/

// Genie
#include "Genie/FS/FSTree_fwd.hh"


// <sys/stat.h>
struct stat;


namespace Genie
{
	
	void stat( const FSTree* node, struct ::stat& sb );
	
}

