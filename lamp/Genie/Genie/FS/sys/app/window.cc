/*
	Genie/FS/sys/app/window.cc
	--------------------------
*/

#include "Genie/FS/sys/app/window.hh"

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// iota
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/sys/app/window/list.hh"
#include "Genie/FS/ReadableSymLink.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class sys_app_window_front : public FSTree_ReadableSymLink
	{
		public:
			sys_app_window_front( const FSTreePtr&     parent,
			                      const plus::string&  name )
			:
				FSTree_ReadableSymLink( parent, name )
			{
			}
			
			bool Exists() const  { return ::FrontWindow() != NULL; }
			
			bool IsLink() const  { return Exists(); }
			
			plus::string ReadLink() const;
	};
	
	plus::string sys_app_window_front::ReadLink() const
	{
		const WindowRef windowPtr = ::FrontWindow();
		
		if ( windowPtr == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		plus::var_string result = "list/12345678";
		
		const size_t hex_offset = STRLEN( "list/" );
		
		iota::encode_32_bit_hex( (unsigned) windowPtr, &result[ hex_offset ] );
		
		return result;
	}
	
	
	const FSTree_Premapped::Mapping sys_app_window_Mappings[] =
	{
		{ "front", &Basic_Factory< sys_app_window_front > },
		
		{ "list", &New_FSTree_sys_app_window_list },
		
		{ NULL, NULL }
		
	};
	
}

