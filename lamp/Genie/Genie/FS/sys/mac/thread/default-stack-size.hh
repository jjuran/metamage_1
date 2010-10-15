/*
	Genie/FS/sys/mac/thread/default-stack-size.hh
	---------------------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_THREAD_DEFAULTSTACKSIZE_HH
#define GENIE_FS_SYS_MAC_THREAD_DEFAULTSTACKSIZE_HH

// Genie
#include "Genie/FS/FSTree_Directory.hh"


namespace plus
{
	
	class var_string;
	
}

namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_thread_defaultstacksize
	{
		static void Read( plus::var_string& result, const FSTree* that, bool binary );
	};
	
}

#endif

