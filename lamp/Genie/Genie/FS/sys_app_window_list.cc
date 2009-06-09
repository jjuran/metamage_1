/*	======================
 *	sys_app_window_list.cc
 *	======================
 */

#include "Genie/FS/sys_app_window_list.hh"

// Standard C++
#include <algorithm>

// Genie
#include "Genie/FS/sys_app_window_list_REF.hh"


namespace Genie
{
	
	bool sys_app_window_list_Details::KeyIsValid( const Key& key )
	{
		Sequence sequence = ItemSequence();
		
		return std::find( sequence.begin(), sequence.end(), key ) != sequence.end();
	}
	
	FSTreePtr sys_app_window_list_Details::GetChildNode( const FSTreePtr&    parent,
		                                                 const std::string&  name,
		                                                 const Key&          key )
	{
		return Premapped_Factory< sys_app_window_list_REF_Mappings >( parent, name );
	}
	
}

