/*	======================
 *	sys_app_window_list.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_SYS_APP_WINDOW_LIST_HH
#define GENIE_FILESYSTEM_SYS_APP_WINDOW_LIST_HH

// plus
#include "plus/hexidecimal.hh"

// Nitrogen
#include "Nitrogen/MacWindows.h"

// Genie
#include "Genie/FS/FSTree_Directory.hh"


namespace Genie
{
	
	struct sys_app_window_list_Details
	{
		typedef Nitrogen::WindowRef Key;
		
		typedef Nitrogen::WindowList_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::WindowList(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static std::string NameFromKey( Key key )
		{
			return plus::encode_32_bit_hex( (unsigned) key );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			return (Key) plus::decode_32_bit_hex( name );
		}
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_app_window_list_Details > sys_app_window_list;
	
}

#endif

