/*
	Genie/FS/sys/app/menu/list/ID.hh
	---------------------------------
*/

#include "Genie/FS/sys/app/menu/list/ID.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif

// gear
#include "gear/parse_decimal.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Str.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/sys/app/menu/list/ID/items.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static MenuRef GetKeyFromParent( const vfs::node* parent )
	{
		return GetMenuRef( gear::parse_decimal( parent->name().c_str() ) );
	}
	
	static
	MenuRef get_menu( const vfs::node* that )
	{
		MenuRef menu = GetKeyFromParent( that );
		
		if ( menu == NULL )
		{
			p7::throw_errno( EIO );
		}
		
		return menu;
	}
	
	static
	void title_get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		MenuRef menu = get_menu( that );
		
		Str255 title = { 0 };
		
		::GetMenuTitle( menu, title );
		
		result = title;
		
		if ( name[ 0 ] != '.' )
		{
			result = plus::utf8_from_mac( result );
		}
	}
	
	static
	void set_menu_title( MenuRef menu, const char* begin, const char* end )
	{
		N::Str255 title( begin, end - begin );
		
		SetMenuTitle( menu, title );
	}
	
	static
	void title_set( const vfs::node* that, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		MenuRef menu = get_menu( that );
		
		if ( name[ 0 ] == '.' )
		{
			set_menu_title( menu, begin, end );
		}
		else
		{
			plus::string mac_text = plus::mac_from_utf8( begin, end - begin );
			
			set_menu_title( menu, mac_text.begin(), mac_text.end() );
		}
	}
	
	static const vfs::property_params sys_app_menu_list_ID_title_params =
	{
		vfs::no_fixed_size,
		(vfs::property_get_hook) &title_get,
		(vfs::property_set_hook) &title_set,
	};
	
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_app_menu_list_ID_Mappings[] =
	{
		{ ".mac-title", PROPERTY( sys_app_menu_list_ID_title ) },
		{ "title",      PROPERTY( sys_app_menu_list_ID_title ) },
		
		{ "items", &New_FSTree_sys_app_menu_list_ID_items },
		
		{ NULL, NULL }
	};
	
}
