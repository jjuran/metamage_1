/*
	sys/mac/time/dls+gmt-delta.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_TIME_DLSGMTDELTA_HH
#define GENIE_FS_SYS_MAC_TIME_DLSGMTDELTA_HH

// vfs
#include "vfs/property.hh"


namespace Genie
{
	
	struct sys_mac_time_dlsgmtdelta : vfs::readonly_property
	{
		static const int fixed_size = 4;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary );
	};
	
	struct sys_mac_time_dls : vfs::readonly_property
	{
		static const int fixed_size = 1;
		
		static void get( plus::var_string& out, const vfs::node* that, bool binary );
	};
	
	struct sys_mac_time_gmtdelta : vfs::readonly_property
	{
		static const int fixed_size = 4;
		
		static void get( plus::var_string& out, const vfs::node* that, bool binary );
	};
	
}

#endif
