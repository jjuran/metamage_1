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
	
	class sys_mac_name_Query
	{
		public:
			sys_mac_name_Query()  {}
			
			std::string Get() const;
	};
	
}

#endif

