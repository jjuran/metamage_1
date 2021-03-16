/*
	Genie/FS/sys/mac/gdev.cc
	------------------------
*/

#include "Genie/FS/sys/mac/gdev.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// POSIX
#include <sys/stat.h>

// iota
#include "iota/strings.hh"

// mac-config
#include "mac_config/color-quickdraw.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"

// gear
#include "gear/hexadecimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// Genie
#include "Genie/FS/sys/mac/gdev/list.hh"


static inline
bool has_color_quickdraw()
{
	return CONFIG_COLOR_QUICKDRAW_GRANTED  ||  mac::sys::has_ColorQuickDraw();
}

namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static plus::string gdev_main_readlink( const vfs::node* that )
	{
		const GDHandle gdH = ::GetMainDevice();
		
		ASSERT( gdH != NULL );
		
		plus::var_string result = "list/12345678";
		
		const size_t hex_offset = STRLEN( "list/" );
		
		gear::encode_32_bit_hex( (unsigned) gdH, &result[ hex_offset ] );
		
		return result;
	}
	
	static const vfs::link_method_set gdev_main_link_methods =
	{
		&gdev_main_readlink
	};
	
	static const vfs::node_method_set gdev_main_methods =
	{
		NULL,
		NULL,
		&gdev_main_link_methods
	};
	
	static vfs::node_ptr gdev_main_factory( const vfs::node*     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		if ( ! (CONFIG_COLOR_QUICKDRAW  &&  has_color_quickdraw()) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new vfs::node( parent, name, S_IFLNK | 0777, &gdev_main_methods );
	}
	
	const vfs::fixed_mapping sys_mac_gdev_Mappings[] =
	{
		{ "main", &gdev_main_factory },
		
		{ "list", &New_FSTree_sys_mac_gdev_list },
		
		{ NULL, NULL }
		
	};
	
}
