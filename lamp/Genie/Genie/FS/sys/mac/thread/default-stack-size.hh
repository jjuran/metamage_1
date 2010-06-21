/*
	Genie/FS/sys/mac/thread/default-stack-size.hh
	---------------------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_THREAD_DEFAULTSTACKSIZE_HH
#define GENIE_FS_SYS_MAC_THREAD_DEFAULTSTACKSIZE_HH

// Genie
#include "Genie/FS/property.hh"


namespace Genie
{
	
	struct sys_mac_thread_defaultstacksize : readonly_property
	{
		static const std::size_t fixed_size = 4;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
}

#endif

