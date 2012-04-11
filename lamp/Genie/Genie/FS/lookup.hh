/*
	lookup.hh
	---------
*/

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	class FSTree;
	
	FSTreePtr lookup( const FSTree*        node,
	                  const plus::string&  name,
	                  const FSTree*        surrogate = NULL );
	
}

