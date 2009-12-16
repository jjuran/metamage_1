/*	=================
 *	sys_app_window.cc
 *	=================
 */

#include "Genie/FS/sys_app_window.hh"

// Mac OS
#include <MacWindows.h>

// iota
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/sys_app_window_list.hh"
#include "Genie/FS/ReadableSymLink.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class sys_app_window_front : public FSTree_ReadableSymLink
	{
		public:
			sys_app_window_front( const FSTreePtr&    parent,
			                      const std::string&  name )
			:
				FSTree_ReadableSymLink( parent, name )
			{
			}
			
			bool Exists() const  { return ::FrontWindow() != NULL; }
			
			bool IsLink() const  { return Exists(); }
			
			std::string ReadLink() const;
	};
	
	std::string sys_app_window_front::ReadLink() const
	{
		const WindowRef windowPtr = ::FrontWindow();
		
		if ( windowPtr == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		std::string result = "list/12345678";
		
		const size_t hex_offset = STRLEN( "list/" );
		
		iota::encode_32_bit_hex( (unsigned) windowPtr, &result[ hex_offset ] );
		
		return result;
	}
	
	
	const FSTree_Premapped::Mapping sys_app_window_Mappings[] =
	{
		{ "front", &Basic_Factory< sys_app_window_front >, true },
		
		{ "list", &Basic_Factory< sys_app_window_list > },
		
		{ NULL, NULL }
		
	};
	
}

