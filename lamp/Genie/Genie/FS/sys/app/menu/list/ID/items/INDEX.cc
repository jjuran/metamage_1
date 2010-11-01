/*
	Genie/FS/sys/app/menu/list/ID/items/INDEX.hh
	--------------------------------------------
*/

#include "Genie/FS/sys/app/menu/list/ID/items/INDEX.hh"

// Standard C/C++
#include <cstddef>

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif

// iota
#include "iota/decimal.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/serialize.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Str.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/serialize_Str255.hh"
#include "Genie/FS/utf8_text_property.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
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
	
	
	static MenuRef get_MenuRef( const FSTree* that )
	{
		return GetMenuRef( iota::parse_decimal( that->ParentRef()->ParentRef()->Name().c_str() ) );
	}
	
	static UInt16 get_menu_item_index( const FSTree* that )
	{
		return iota::parse_decimal( that->Name().c_str() );
	}
	
	template < class Accessor >
	struct sys_app_menu_list_ID_items_INDEX_Property : readwrite_property
	{
		static const std::size_t fixed_size = Accessor::fixed_size;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			const MenuRef menu = get_MenuRef( that );
			
			const UInt16 index = get_menu_item_index( that );
			
			if ( menu == NULL  ||  index == 0 )
			{
				p7::throw_errno( EIO );
			}
			
			typedef typename Accessor::result_type result_type;
			
			const result_type data = Accessor::Get( menu, index );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const MenuRef menu = get_MenuRef( that );
			
			const UInt16 index = get_menu_item_index( that );
			
			if ( menu == NULL  ||  index == 0 )
			{
				p7::throw_errno( EIO );
			}
			
			Accessor::Set( menu, index, Accessor::reconstruct::apply( begin, end, binary ) );
		}
	};
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	#define PROPERTY_ACCESS( access )  PROPERTY( sys_app_menu_list_ID_items_INDEX_Property< access > )
	
	typedef sys_app_menu_list_ID_items_INDEX_Property< menu_item_text > sys_app_menu_list_ID_items_INDEX_text;
	
	const FSTree_Premapped::Mapping sys_app_menu_list_ID_items_INDEX_Mappings[] =
	{
		{ ".mac-text", PROPERTY( sys_app_menu_list_ID_items_INDEX_text ) },
		
		{ "text", PROPERTY( utf8_text_property< sys_app_menu_list_ID_items_INDEX_text > ) },
		
		{ "mark", PROPERTY_ACCESS( menu_item_mark ) },
		{ "key",  PROPERTY_ACCESS( menu_item_key  ) },
		
		{ NULL, NULL }
	};
	
}

