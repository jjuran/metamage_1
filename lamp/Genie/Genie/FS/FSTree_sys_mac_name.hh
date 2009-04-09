/*	======================
 *	FSTree_sys_mac_name.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACNAME_HH
#define GENIE_FILESYSTEM_FSTREESYSMACNAME_HH

// Standard C++
#include <string>


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_name
	{
		static std::string Read( const FSTree* that, bool binary );
	};
	
}

#endif

