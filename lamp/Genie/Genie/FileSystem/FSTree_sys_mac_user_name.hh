/*	===========================
 *	FSTree_sys_mac_user_name.hh
 *	===========================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACUSERNAME_HH
#define GENIE_FILESYSTEM_FSTREESYSMACUSERNAME_HH

// Standard C++
#include <string>


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_user_name
	{
		static std::string Read( const FSTree* );
	};
	
}

#endif

