/*
	symlink.hh
	----------
*/

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Genie
{
	
	void symlink( const FSTree* it, const plus::string& target );
	
}

