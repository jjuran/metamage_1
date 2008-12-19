/*	======================
 *	FSTree_sys_mac_keys.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACKEYS_HH
#define GENIE_FILESYSTEM_FSTREESYSMACKEYS_HH

// Standard C++
#include <string>


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_keys
	{
		static std::string Read( const FSTree* );
	};
	
}

#endif

