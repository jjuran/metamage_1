/*
	sys/mac/time/dls+gmt-delta.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_TIME_DLSGMTDELTA_HH
#define GENIE_FS_SYS_MAC_TIME_DLSGMTDELTA_HH


namespace plus
{
	
	class var_string;
	
}

namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_time_dlsgmtdelta
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
	struct sys_mac_time_dls
	{
		static void get( plus::var_string& out, const FSTree* that, bool binary );
	};
	
	struct sys_mac_time_gmtdelta
	{
		static void get( plus::var_string& out, const FSTree* that, bool binary );
	};
	
}

#endif

