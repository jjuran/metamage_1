/*
	Genie/FS/sys/app/menu/list.cc
	-----------------------------
*/

#include "Genie/FS/sys/app/menu/list.hh"

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MENUS__
#include <Menus.h>
#endif

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/sys/app/menu/list/ID.hh"


namespace Genie
{
	
	static FSTreePtr menu_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		MenuRef menu = GetMenuRef( gear::parse_decimal( name.c_str() ) );
		
		if ( menu == NULL )
		{
			throw poseven::errno_t( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_app_menu_list_ID_Mappings );
	}
	
	static void menulist_iterate( const FSTreePtr& parent, FSTreeCache& cache )
	{
	#if !TARGET_API_MAC_CARBON
		
		Handle h = LMGetMenuList();
		
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
	
	FSTreePtr New_FSTree_sys_app_menu_list( const FSTreePtr&     parent,
	                                        const plus::string&  name,
	                                        const void*          args )
	{
		return new_basic_directory( parent, name, menu_lookup, menulist_iterate );
	}
	
}

