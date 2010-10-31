/*
	Genie/FS/sys/app/menu/list/ID.hh
	---------------------------------
*/

#include "Genie/FS/sys/app/menu/list/ID.hh"

// Standard C/C++
#include <cstddef>

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif

// iota
#include "iota/decimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Str.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/serialize_Str255.hh"
#include "Genie/FS/utf8_text_property.hh"
#include "Genie/FS/sys/app/menu/list/ID/items.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	struct menu_title : serialize_Str255_contents
	{
		static N::Str255 Get( MenuRef menu )
		{
			N::Str255 result;
			
			::GetMenuTitle( menu, result );
			
			return result;
		}
		
		static void Set( MenuRef menu, ConstStr255Param title )
		{
			::SetMenuTitle( menu, title );
		}
	};
	
	
	static MenuRef GetKeyFromParent( const FSTree* parent )
	{
		return GetMenuRef( iota::parse_decimal( parent->Name().c_str() ) );
	}
	
	template < class Accessor >
	struct sys_app_menu_list_ID_Property : readwrite_property
	{
		static const std::size_t fixed_size = Accessor::fixed_size;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			MenuRef menu = GetKeyFromParent( that );
			
			if ( menu == NULL )
			{
				p7::throw_errno( EIO );
			}
			
			typedef typename Accessor::result_type result_type;
			
			const result_type data = Accessor::Get( menu );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
		
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			MenuRef menu = GetKeyFromParent( that );
			
			if ( menu == NULL )
			{
				p7::throw_errno( EIO );
			}
			
			Accessor::Set( menu, Accessor::reconstruct::apply( begin, end, binary ) );
		}
	};
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	typedef sys_app_menu_list_ID_Property< menu_title > sys_app_menu_list_ID_title;
	
	const FSTree_Premapped::Mapping sys_app_menu_list_ID_Mappings[] =
	{
		{ ".mac-title", PROPERTY( sys_app_menu_list_ID_title ) },
		
		{ "title", PROPERTY( utf8_text_property< sys_app_menu_list_ID_title > ) },
		
		{ "items", &New_FSTree_sys_app_menu_list_ID_items },
		
		{ NULL, NULL }
	};
	
}

