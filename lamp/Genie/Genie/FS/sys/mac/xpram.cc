/*
	Genie/FS/sys/mac/xpram.cc
	-------------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/xpram.hh"

// mac-sys-utils
#include "mac_sys/xpram.hh"


namespace Genie
{
	
	plus::string sys_mac_xpram::Read( const vfs::node* parent, const plus::string& name )
	{
		const UInt16 length = 256;
		const UInt16 offset = 0;
		
		plus::string xpram;
		
		char* p = xpram.reset( length );
		
		mac::sys::ReadXPRam( p, length, offset );
		
		return xpram;
	}
	
}

#endif
