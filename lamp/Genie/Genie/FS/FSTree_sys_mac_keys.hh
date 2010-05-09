/*	======================
 *	FSTree_sys_mac_keys.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACKEYS_HH
#define GENIE_FILESYSTEM_FSTREESYSMACKEYS_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_keys
	{
		static plus::string Read( const FSTree* );
	};
	
}

#endif

