/*
	make_alias.cc
	-------------
*/

#include "relix/api/make_alias.hh"

// mac-config
#include "mac_config/aliases.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-app-utils
#include "mac_app/create_alias.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// MacVFS
#include "MacVFS/util/FSSpec_from_node.hh"


namespace relix
{
	
	enum
	{
		gestaltAliasMgrAttr = 'alis',
		
		unimpErr = -4,
	};
	
	const bool aliases_present =
		CONFIG_ALIASES  &&
			(CONFIG_ALIASES_GRANTED  ||
				mac::sys::gestalt( gestaltAliasMgrAttr ) );
	
	void make_alias( const vfs::node& target, const vfs::node& alias )
	{
		using mac::types::FSSpec;
		
		if ( ! aliases_present )
		{
			Mac::ThrowOSStatus( unimpErr );
			return;
		}
		
		const FSSpec targetSpec = vfs::FSSpec_from_node( target );
		const FSSpec aliasSpec  = vfs::FSSpec_from_node( alias  );
		
		Mac::ThrowOSStatus( mac::app::create_alias( aliasSpec, targetSpec ) );
	}
	
}
