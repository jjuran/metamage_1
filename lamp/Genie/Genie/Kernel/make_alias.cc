/*
	make_alias.cc
	-------------
*/

#include "Genie/Kernel/make_alias.hh"

// MacVFS
#include "MacVFS/util/FSSpec_from_node.hh"

// Genie
#include "Genie/Utilities/CreateAlias.hh"


namespace Genie
{
	
	void make_alias( const vfs::node& target, const vfs::node& alias )
	{
		using mac::types::FSSpec;
		
		const FSSpec targetSpec = vfs::FSSpec_from_node( target );
		const FSSpec aliasSpec  = vfs::FSSpec_from_node( alias  );
		
		CreateAlias( aliasSpec, targetSpec );
	}
	
}

