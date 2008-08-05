/*	==============================
 *	FSTree_sys_mac_machine_name.hh
 *	==============================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACMACHINENAME_HH
#define GENIE_FILESYSTEM_FSTREESYSMACMACHINENAME_HH

// Standard C++
#include <string>


namespace Genie
{
	
	class sys_mac_machine_name_Query
	{
		public:
			std::string operator()() const;
	};
	
}

#endif

