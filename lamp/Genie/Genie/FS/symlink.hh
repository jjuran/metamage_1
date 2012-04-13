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
	
	void symlink( const FSTree* node, const plus::string& target );
	
}

