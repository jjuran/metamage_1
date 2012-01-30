/*
	Genie/FS/sys/app/menu/list/ID/items.cc
	--------------------------------------
*/

#include "Genie/FS/sys/app/menu/list/ID/items.hh"

// Mac OS
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
#include "Genie/FS/sys/app/menu/list/ID/items/INDEX.hh"


namespace Genie
{
	
	static bool menu_has_item_index( MenuRef menu, const plus::string& index )
	{
		if ( menu != NULL  &&  index.size() < 5 )
		{
			if ( const unsigned i = gear::parse_unsigned_decimal( index.c_str() ) )
			{
				const UInt16 n_items = ::CountMenuItems( menu );
				
				return i <= n_items;
			}
		}
		
		return false;
	}
	
	static FSTreePtr menu_items_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		MenuRef menu = GetMenuRef( gear::parse_decimal( parent->owner()->name().c_str() ) );
		
		if ( !menu_has_item_index( menu, name ) )
		{
			throw poseven::errno_t( ENOENT );
		}
		
		return Premapped_Factory( parent, name, sys_app_menu_list_ID_items_INDEX_Mappings );
	}
	
	static void menu_items_iterate( const FSTreePtr& parent, FSTreeCache& cache )
	{
		MenuRef menu = GetMenuRef( gear::parse_decimal( parent->owner()->name().c_str() ) );
		
		if ( menu == NULL )
		{
			throw poseven::errno_t( ENOENT );
		}
		
		UInt16 n_items = ::CountMenuItems( menu );
		
		for ( int i = 1;  i <= n_items;  ++i )
		{
			cache.push_back( FSNode( i, gear::inscribe_decimal( i ) ) );
		}
	}
	
	FSTreePtr New_FSTree_sys_app_menu_list_ID_items( const FSTreePtr&     parent,
	                                                 const plus::string&  name,
	                                                 const void*          args )
	{
		return new_basic_directory( parent, name, menu_items_lookup, menu_items_iterate );
	}
	
}

