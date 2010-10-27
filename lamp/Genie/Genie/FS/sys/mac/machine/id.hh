/*
	Genie/FS/sys/mac/machine/id.hh
	------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_MACHINE_ID_HH
#define GENIE_FS_SYS_MAC_MACHINE_ID_HH


namespace plus
{
	
	class var_string;
	
}

namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_machine_id
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
}

#endif

