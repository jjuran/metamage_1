/*	===========================
 *	FSTree_sys_mac_user_name.hh
 *	===========================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACUSERNAME_HH
#define GENIE_FILESYSTEM_FSTREESYSMACUSERNAME_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_user_name
	{
		static plus::string Read( const FSTree* that, bool binary );
	};
	
}

#endif

