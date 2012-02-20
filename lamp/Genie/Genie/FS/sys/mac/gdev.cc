/*
	Genie/FS/sys/mac/gdev.cc
	------------------------
*/

#include "Genie/FS/sys/mac/gdev.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// iota
#include "iota/strings.hh"

// gear
#include "gear/hexidecimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// MacFeatures
#include "MacFeatures/ColorQuickdraw.hh"

// Genie
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/sys/mac/gdev/list.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static plus::string gdev_main_readlink( const FSTree* node )
	{
		const GDHandle gdH = ::GetMainDevice();
		
		ASSERT( gdH != NULL );
		
		plus::var_string result = "list/12345678";
		
		const size_t hex_offset = STRLEN( "list/" );
		
		gear::encode_32_bit_hex( (unsigned) gdH, &result[ hex_offset ] );
		
		return result;
	}
	
	static const link_method_set gdev_main_link_methods =
	{
		&gdev_main_readlink
	};
	
	static const node_method_set gdev_main_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&gdev_main_link_methods
	};
	
	static FSTreePtr gdev_main_factory( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    const void*          args )
	{
		if ( !MacFeatures::Has_ColorQuickdraw() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return new FSTree( parent, name, S_IFLNK | 0777, &gdev_main_methods );
	}
	
	const FSTree_Premapped::Mapping sys_mac_gdev_Mappings[] =
	{
		{ "main", &gdev_main_factory },
		
		{ "list", &New_FSTree_sys_mac_gdev_list },
		
		{ NULL, NULL }
		
	};
	
}

