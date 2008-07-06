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
		
		static std::string Name()  { return "window"; }
		
		static FSTreePtr Parent()  { return GetSingleton< FSTree_sys_mac >(); }
		
		static const Sequence& ItemSequence()  { return Nitrogen::WindowList(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
	};
	
	typedef FSTree_Sequence< sys_mac_window_Details > FSTree_sys_mac_window;
	
	
	class FSTree_sys_mac_window_REF : public FSTree_Virtual,
	                                  public WindowRef_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_sys_mac_window_REF( const Key& key ) : itsKey( key )  {}
			
			std::string Name() const  { return NameFromKey( itsKey ); }
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_sys_mac_window >(); }
	};
	
}

#endif

