/*
	Genie/FS/sys/app/window.cc
	--------------------------
*/

#include "Genie/FS/sys/app/window.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// iota
#include "iota/strings.hh"

// gear
#include "gear/hexidecimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/nodes/symbolic_link.hh"

// Genie
#include "Genie/FS/sys/app/window/list.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static plus::string sys_app_window_front_ReadLink()
	{
		const WindowRef windowPtr = ::FrontWindow();
		
		if ( windowPtr == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		plus::var_string result = "list/12345678";
		
		const size_t hex_offset = STRLEN( "list/" );
		
		gear::encode_32_bit_hex( (unsigned) windowPtr, &result[ hex_offset ] );
		
		return result;
	}
	
	
	static FSTreePtr new_front( const FSTree*        parent,
	                            const plus::string&  name,
	                            const void*          args )
	{
		return vfs::new_symbolic_link( parent, name, sys_app_window_front_ReadLink() );
	}
	
	const vfs::fixed_mapping sys_app_window_Mappings[] =
	{
		{ "front", &new_front },
		
		{ "list", &New_FSTree_sys_app_window_list },
		
		{ NULL, NULL }
		
	};
	
}

