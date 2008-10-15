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
	
	class sys_mac_keys_Query
	{
		public:
			sys_mac_keys_Query()  {}
			
			std::string operator()() const;
	};
	
}

#endif

