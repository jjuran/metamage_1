/*	======================
 *	sys_app_window_list.cc
 *	======================
 */

#include "Genie/FS/sys_app_window_list.hh"

// Standard C++
#include <algorithm>

// plus
#include "plus/hexidecimal.hh"

// Nitrogen
#include "Nitrogen/MacWindows.h"

// Genie
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/sys_app_window_list_REF.hh"


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
	
	
	bool sys_app_window_list_Details::KeyIsValid( const Key& key )
	{
		if ( key == NULL )
		{
			return false;
		}
		
		Sequence sequence = ItemSequence();
		
		return std::find( sequence.begin(), sequence.end(), key ) != sequence.end();
	}
	
	FSTreePtr sys_app_window_list_Details::GetChildNode( const FSTreePtr&    parent,
		                                                 const std::string&  name,
		                                                 const Key&          key )
	{
		return Premapped_Factory< sys_app_window_list_REF_Mappings >( parent, name );
	}
	
	FSTreePtr New_FSTree_sys_app_window_list( const FSTreePtr& parent, const std::string& name )
	{
		return FSTreePtr( new sys_app_window_list( parent, name ) );
	}
	
}

