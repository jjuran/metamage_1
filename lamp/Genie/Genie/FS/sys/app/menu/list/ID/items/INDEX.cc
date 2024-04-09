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


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	static
	void set_text( MenuRef menu, short index, const char* begin, const char* end )
	{
		N::Str255 title( begin, end - begin );
		
		::SetMenuItemText( menu, index, title );
	}
	
	struct menu_item_cmd : plus::serialize_hex< ::OSType >
	{
		static Ped::CommandCode Get( MenuRef menu, UInt32 choice )
		{
			return Ped::GetMenuItemCommandCode( choice );
		}
		
		static void Set( MenuRef menu, UInt32 choice, ::OSType code )
		{
			Ped::SetMenuItemCommandCode( choice,
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
		int32_t item;  // actually, a full menu choice
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
		
		long menu_choice = GetMenuID( menu ) << 16 | index;
		
		const menu_item_rec result = { menu, menu_choice };
		
		return result;
	}
	
	static
	void text_get( plus::var_string& result, const vfs::node* that, bool binary, const plus::string& name )
	{
		const menu_item_rec menu_item = get_menu_item( that );
		
		const MenuRef menu = menu_item.menu;
		const UInt16 index = menu_item.item;
		
		Str255 text = { 0 };
		
		::GetMenuItemText( menu, index, text );
		
		result = text;
		
		if ( name[ 0 ] != '.' )
		{
			result = plus::utf8_from_mac( result );
		}
	}
	
	static
	void text_set( const vfs::node* that, const char* begin, const char* end, bool binary, const plus::string& name )
	{
		const menu_item_rec menu_item = get_menu_item( that );
		
		const MenuRef menu = menu_item.menu;
		const UInt16 index = menu_item.item;
		
		if ( name[ 0 ] == '.' )
		{
			set_text( menu, index, begin, end );
		}
		else
		{
			plus::string mac_text = plus::mac_from_utf8( begin, end - begin );
			
			set_text( menu, index, mac_text.begin(), mac_text.end() );
		}
	}
	
	static const vfs::property_params sys_app_menu_list_ID_items_INDEX_text_params =
	{
		vfs::no_fixed_size,
		(vfs::property_get_hook) &text_get,
		(vfs::property_set_hook) &text_set,
	};
	
	#define DEFINE_GETTER( p )  \
	static void p##_get( plus::var_string& result, const vfs::node* that, bool binary )  \
	{  \
		const menu_item_rec menu_item = get_menu_item( that );  \
		const MenuRef menu = menu_item.menu;                    \
		const UInt32 index = menu_item.item;                    \
		const p::result_type data = p::Get( menu, index );      \
		p::deconstruct::apply( result, data, binary );          \
	}
	
	#define DEFINE_SETTER( p )  \
	static void p##_set( const vfs::node* that, const char* begin, const char* end, bool binary )  \
	{  \
		const menu_item_rec menu_item = get_menu_item( that );               \
		const MenuRef menu = menu_item.menu;                                 \
		const UInt32 index = menu_item.item;                                 \
		p::Set( menu, index, p::reconstruct::apply( begin, end, binary ) );  \
	}
	
	DEFINE_GETTER( menu_item_mark );
	DEFINE_GETTER( menu_item_key  );
	DEFINE_GETTER( menu_item_cmd  );
	
	DEFINE_SETTER( menu_item_mark );
	DEFINE_SETTER( menu_item_key  );
	DEFINE_SETTER( menu_item_cmd  );
	
	#define DEFINE_PARAMS( p )  \
	static const vfs::property_params p##_params = {p::fixed_size, &p##_get, &p##_set}
	
	DEFINE_PARAMS( menu_item_mark );
	DEFINE_PARAMS( menu_item_key  );
	DEFINE_PARAMS( menu_item_cmd  );
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	#define PROPERTY_ACCESS( access )  PROPERTY( access )
	
	const vfs::fixed_mapping sys_app_menu_list_ID_items_INDEX_Mappings[] =
	{
		{ ".mac-text", PROPERTY( sys_app_menu_list_ID_items_INDEX_text ) },
		{ "text",      PROPERTY( sys_app_menu_list_ID_items_INDEX_text ) },
		
		{ "mark", PROPERTY_ACCESS( menu_item_mark ) },
		{ "key",  PROPERTY_ACCESS( menu_item_key  ) },
		{ "cmd",  PROPERTY_ACCESS( menu_item_cmd  ) },
		
		{ NULL, NULL }
	};
	
}
