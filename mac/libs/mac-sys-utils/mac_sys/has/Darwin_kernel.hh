/*
	Darwin_kernel.hh
	----------------
*/

#ifndef MACSYS_HAS_DARWINKERNEL_HH
#define MACSYS_HAS_DARWINKERNEL_HH

// mac-sys-utils
#include "mac_sys/has/BlueBox.hh"
#include "mac_sys/has/native_Carbon.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_Darwin_kernel()
	{
		return has_native_Carbon()  ||  has_BlueBox();
	}
	
}
}

#endif
