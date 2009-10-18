/*	=================
 *	sys_app_window.cc
 *	=================
 */

#include "Genie/FS/sys_app_window.hh"

// Mac OS
#include <MacWindows.h>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/sys_app_window_list.hh"
#include "Genie/FS/ResolvePathname.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class sys_app_window_front : public FSTree
	{
		public:
			sys_app_window_front( const FSTreePtr&    parent,
			                      const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			bool Exists() const  { return ::FrontWindow() != NULL; }
			
			bool IsLink() const  { return Exists(); }
			
			std::string ReadLink() const;
			
			FSTreePtr ResolveLink() const;
	};
	
	std::string sys_app_window_front::ReadLink() const
	{
		const WindowRef windowPtr = ::FrontWindow();
		
		if ( windowPtr == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		std::string result = "list/";
		
		result += Pointer_KeyName_Traits< ::WindowRef >::NameFromKey( windowPtr );
		
		return result;
	}
	
	FSTreePtr sys_app_window_front::ResolveLink() const
	{
		return ResolveAbsolutePath( "/sys/app/window/" + ReadLink() );
	}
	
	
	const FSTree_Premapped::Mapping sys_app_window_Mappings[] =
	{
		{ "front", &Basic_Factory< sys_app_window_front >, true },
		
		{ "list", &Basic_Factory< sys_app_window_list > },
		
		{ NULL, NULL }
		
	};
	
}

