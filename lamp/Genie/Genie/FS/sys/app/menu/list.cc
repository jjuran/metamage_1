/*
	Genie/FS/sys/app/menu/list.cc
	-----------------------------
*/

#include "Genie/FS/sys/app/menu/list.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/node/types/basic_directory.hh"

// Genie
#include "Genie/FS/sys/app/menu/list/ID.hh"


#define MenuList  (*(Handle*) 0x0A1C)


namespace Genie
{
	
	static vfs::node_ptr menu_lookup( const vfs::node* parent, const plus::string& name )
	{
		MenuRef menu = GetMenuRef( gear::parse_decimal( name.c_str() ) );
		
		if ( menu == NULL )
		{
			throw poseven::errno_t( ENOENT );
		}
		
		return fixed_dir( parent, name, sys_app_menu_list_ID_Mappings );
	}
	
	static void menulist_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
	#if !TARGET_API_MAC_CARBON
		
		Handle h = MenuList;
		
		MenuBarHeader header = **(MenuBarHeader**) h;
		
		const unsigned n_menus = header.lastMenu / 6 - 1;
		
		for ( int i = 1;  i <= n_menus;  ++i )
		{
			MenuBarMenu* menus = (MenuBarMenu*) *h;
			
			MenuBarMenu menu = menus[i];
			
			MenuID id = GetMenuID( menu.menu );
			
			cache.push_back( vfs::dir_entry( id, gear::inscribe_decimal( id ) ) );
		}
		
	#endif
	}
	
	vfs::node_ptr New_FSTree_sys_app_menu_list( const vfs::node*     parent,
	                                            const plus::string&  name,
	                                            const void*          args )
	{
		return vfs::new_basic_directory( parent, name, menu_lookup, menulist_iterate );
	}
	
}
