/*
	sys/mac/time/dls+gmt-delta.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_TIME_DLSGMTDELTA_HH
#define GENIE_FS_SYS_MAC_TIME_DLSGMTDELTA_HH

// Genie
#include "Genie/FS/property.hh"


namespace Genie
{
	
	struct sys_mac_time_dlsgmtdelta : readonly_property
	{
		static const int fixed_size = 4;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary );
	};
	
	struct sys_mac_time_dls : readonly_property
	{
		static const int fixed_size = 1;
		
		static void get( plus::var_string& out, const vfs::node* that, bool binary );
	};
	
	struct sys_mac_time_gmtdelta : readonly_property
	{
		static const int fixed_size = 4;
		
		static void get( plus::var_string& out, const vfs::node* that, bool binary );
	};
	
}

#endif

