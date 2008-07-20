/*	========================
 *	FSTree_sys_mac_window.hh
 *	========================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_SYS_MAC_WINDOW_HH
#define GENIE_FILESYSTEM_FSTREE_SYS_MAC_WINDOW_HH

// Nitrogen
#include "Nitrogen/MacWindows.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	struct WindowRef_KeyName_Traits : public Pointer_KeyName_Traits< Nitrogen::WindowRef >  {};
	
	
	class FSTree_sys_mac_window_REF;
	
	
	struct sys_mac_window_Details : public WindowRef_KeyName_Traits
	{
		typedef Nitrogen::WindowList_Container Sequence;
		
		typedef FSTree_sys_mac_window_REF ChildNode;
		
		static const Sequence& ItemSequence()  { return Nitrogen::WindowList(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_window_Details > FSTree_sys_mac_window;
	
}

#endif

