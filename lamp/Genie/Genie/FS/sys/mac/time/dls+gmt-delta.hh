/*
	sys/mac/time/dls+gmt-delta.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_TIME_DLSGMTDELTA_HH
#define GENIE_FS_SYS_MAC_TIME_DLSGMTDELTA_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_time_dlsgmtdelta
	{
		static plus::string Read( const FSTree* that, bool binary );
	};
	
}

#endif

