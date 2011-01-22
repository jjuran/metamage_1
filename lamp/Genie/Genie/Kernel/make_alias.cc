/*
	make_alias.cc
	-------------
*/

#include "Genie/Kernel/make_alias.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/Utilities/CreateAlias.hh"


namespace Genie
{
	
	void make_alias( const FSTreePtr& target, const FSTreePtr& alias )
	{
		const FSSpec targetSpec = GetFSSpecFromFSTree( target );
		const FSSpec aliasSpec  = GetFSSpecFromFSTree( alias  );
		
		CreateAlias( aliasSpec, targetSpec );
	}
	
}

