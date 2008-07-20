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
#include "Genie/FileSystem/FSTree_sys_mac.hh"


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
	
	
	class FSTree_sys_mac_window_REF : public FSTree_Functional< WindowRef_KeyName_Traits::Key >
	{
		private:
			typedef WindowRef_KeyName_Traits::Key Key;
			
			typedef FSTree_Functional< Key > Base;
		
		public:
			FSTree_sys_mac_window_REF( const FSTreePtr&    parent,
			                           const std::string&  name,
			                           const Key&          key ) : Base( parent, name, key )
			{
			}
	};
	
}

#endif

