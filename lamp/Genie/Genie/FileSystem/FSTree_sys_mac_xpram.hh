/*	=======================
 *	FSTree_sys_mac_xpram.hh
 *	=======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACXPRAM_HH
#define GENIE_FILESYSTEM_FSTREESYSMACXPRAM_HH

// Standard C++
#include <string>


namespace Genie
{
	
	class sys_mac_xpram_Query
	{
		public:
			sys_mac_xpram_Query()  {}
			
			std::string operator()() const;
	};
	
}

#endif

