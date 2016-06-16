/*
	make_alias.cc
	-------------
*/

#include "Genie/Kernel/make_alias.hh"

// mac-app-utils
#include "mac_app/create_alias.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// MacVFS
#include "MacVFS/util/FSSpec_from_node.hh"


namespace Genie
{
	
	void make_alias( const vfs::node& target, const vfs::node& alias )
	{
		using mac::types::FSSpec;
		
		const FSSpec targetSpec = vfs::FSSpec_from_node( target );
		const FSSpec aliasSpec  = vfs::FSSpec_from_node( alias  );
		
		Mac::ThrowOSStatus( mac::app::create_alias( aliasSpec, targetSpec ) );
	}
	
}
