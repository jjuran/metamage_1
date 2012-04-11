/*
	symlink.hh
	----------
*/

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class FSTree;
	
	void symlink( const FSTree* node, const plus::string& target );
	
}

