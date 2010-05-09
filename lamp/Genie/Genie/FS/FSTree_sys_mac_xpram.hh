/*	=======================
 *	FSTree_sys_mac_xpram.hh
 *	=======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACXPRAM_HH
#define GENIE_FILESYSTEM_FSTREESYSMACXPRAM_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class sys_mac_xpram_Query
	{
		public:
			sys_mac_xpram_Query()  {}
			
			plus::string Get() const;
	};
	
	class FSTree;
	
	struct sys_mac_xpram
	{
		static plus::string Read( const FSTree* );
	};
	
}

#endif

