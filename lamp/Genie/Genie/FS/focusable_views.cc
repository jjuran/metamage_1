/*
	focusable_views.cc
	------------------
*/

#include "Genie/FS/focusable_views.hh"

// Standard C++
#include <map>


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	typedef std::map< unsigned long, const void* > Map;
	
	static Map the_map;
	
	
	void add_focusable_view( const FSTree* key, Ped::View* value )
	{
		the_map[ (unsigned long) key ] = value;
	}
	
	Ped::View* get_focusable_view( const FSTree* key )
	{
		Map::const_iterator it = the_map.find( (unsigned long) key );
		
		if ( it != the_map.end() )
		{
			return (Ped::View*) it->second;
		}
		
		return NULL;
	}
	
	void remove_focusable_view( const FSTree* key )
	{
		the_map.erase( (unsigned long) key );
	}
	
}

