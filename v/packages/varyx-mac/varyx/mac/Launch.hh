/*
	Launch.hh
	---------
*/

#ifndef VARYX_MAC_LAUNCH_HH
#define VARYX_MAC_LAUNCH_HH

// vlib
#include "vlib/value.hh"


struct FSSpec;


namespace vlib
{
	
	struct proc_info;
	
}

namespace varyx
{
namespace mac
{
	
	extern const vlib::proc_info proc_LaunchApplication;
	
}
}

#endif
