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
	
	class sys_mac_user_name_Query
	{
		public:
			sys_mac_user_name_Query()  {}
			
			std::string operator()() const;
	};
	
}

#endif

