/*	======================
 *	sys_app_window_list.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_SYS_APP_WINDOW_LIST_HH
#define GENIE_FILESYSTEM_SYS_APP_WINDOW_LIST_HH

// Nitrogen
#include "Nitrogen/MacWindows.h"

// Genie
#include "Genie/FS/FSTree_Directory.hh"


namespace Genie
{
	
	struct WindowRef_KeyName_Traits : public Pointer_KeyName_Traits< Nitrogen::WindowRef >  {};
	
	
	struct sys_app_window_list_Details : public WindowRef_KeyName_Traits
	{
		typedef Nitrogen::WindowList_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::WindowList(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_app_window_list_Details > sys_app_window_list;
	
}

#endif

