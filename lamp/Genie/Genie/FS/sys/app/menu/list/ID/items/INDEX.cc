/*
	Genie/FS/sys/app/menu/list/ID/items/INDEX.hh
	--------------------------------------------
*/

#include "Genie/FS/sys/app/menu/list/ID/items/INDEX.hh"

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
#include "plus/serialize.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Str.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"

// Pedestal
#include "Pedestal/MenuItemCommands.hh"

// Genie
#include "Genie/FS/serialize_Str255.hh"
#include "Genie/FS/utf8_text_property.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	struct menu_item_text : serialize_Str255_contents
	{
		static N::Str255 Get( MenuRef menu, UInt16 index )
		{
			N::Str255 result;
			
			::GetMenuItemText( menu, index, result );
			
			return result;
		}
		
		static void Set( MenuRef menu, UInt16 index, ConstStr255Param title )
		{
			::SetMenuItemText( menu, index, title );
		}
	};
	
	struct menu_item_cmd : plus::serialize_hex< ::OSType >
	{
		static Ped::CommandCode Get( MenuRef menu, UInt16 index )
		{
			return Ped::GetMenuItemCommandCode( Mac::MenuID( GetMenuID( menu ) ), index );
		}
		
		static void Set( MenuRef menu, UInt16 index, ::OSType code )
		{
			Ped::SetMenuItemCommandCode( Mac::MenuID( GetMenuID( menu ) ),
			                             index,
			                             Ped::CommandCode( code ) );
		}
	};
	
	struct menu_item_mark : plus::serialize_hex< char >
	{
		static char Get( MenuRef menu, UInt16 index )
		{
			CharParameter result;
			
			::GetItemMark( menu, index, &result );
			
			return result;
		}
		
		static void Set( MenuRef menu, UInt16 index, char mark )
		{
			::SetItemMark( menu, index, mark );
		}
	};
	
	struct menu_item_key : plus::serialize_char
	{
		static char Get( MenuRef menu, UInt16 index )
		{
			CharParameter result;
			
			::GetItemCmd( menu, index, &result );
			
			return result;
		}
		
		static void Set( MenuRef menu, UInt16 index, char cmd )
		{
			::SetItemCmd( menu, index, cmd );
		}
	};
	
	
	static MenuRef get_MenuRef( const vfs::node* that )
	{
		return GetMenuRef( gear::parse_decimal( that->owner()->owner()->name().c_str() ) );
	}
	
	static UInt16 get_menu_item_index( const vfs::node* that )
	{
		return gear::parse_decimal( that->name().c_str() );
	}
	
	struct menu_item_rec
	{
		MenuRef menu;
		int16_t item;
	};
	
	static
	menu_item_rec get_menu_item( const vfs::node* that )
	{
		const MenuRef menu = get_MenuRef( that );
		const UInt16 index = get_menu_item_index( that );
		
		if ( menu == NULL  ||  index == 0 )
		{
			p7::throw_errno( EIO );
		}
		
		const menu_item_rec result = { menu, index };
		
		return result;
	}
	
	template < class Accessor >
	struct sys_app_menu_list_ID_items_INDEX_Property : vfs::readwrite_property
	{
		static const int fixed_size = Accessor::fixed_size;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			const menu_item_rec menu_item = get_menu_item( that );

			const MenuRef menu = menu_item.menu;
			const UInt16 index = menu_item.item;
			
			typedef typename Accessor::result_type result_type;
			
			const result_type data = Accessor::Get( menu, index );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			const menu_item_rec menu_item = get_menu_item( that );

			const MenuRef menu = menu_item.menu;
			const UInt16 index = menu_item.item;
			
			Accessor::Set( menu, index, Accessor::reconstruct::apply( begin, end, binary ) );
		}
	};
	
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	#define PROPERTY_ACCESS( access )  PROPERTY( sys_app_menu_list_ID_items_INDEX_Property< access > )
	
	typedef sys_app_menu_list_ID_items_INDEX_Property< menu_item_text > sys_app_menu_list_ID_items_INDEX_text;
	
	const vfs::fixed_mapping sys_app_menu_list_ID_items_INDEX_Mappings[] =
	{
		{ ".mac-text", PROPERTY( sys_app_menu_list_ID_items_INDEX_text ) },
		
		{ "text", PROPERTY( utf8_text_property< sys_app_menu_list_ID_items_INDEX_text > ) },
		
		{ "mark", PROPERTY_ACCESS( menu_item_mark ) },
		{ "key",  PROPERTY_ACCESS( menu_item_key  ) },
		{ "cmd",  PROPERTY_ACCESS( menu_item_cmd  ) },
		
		{ NULL, NULL }
	};
	
}
