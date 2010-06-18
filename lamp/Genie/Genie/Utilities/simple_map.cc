/*
	simple_map.cc
	-------------
*/

#include "Genie/Utilities/simple_map.hh"

// Standard C++
#include <algorithm>
#include <map>


namespace Genie
{
	
	struct simple_map_impl
	{
		std::map< const void*, void* > map;
	};
	
	class map_destroyer
	{
		private:
			typedef void (*deallocator)( void* );
			
			typedef std::map< const void*, void* >::value_type value_type;
			
			deallocator its_deallocator;
		
		public:
			map_destroyer( deallocator d ) : its_deallocator( d )
			{
			}
			
			void operator()( const value_type& value ) const
			{
				its_deallocator( value.second );
			}
	};
	
	map_base::~map_base()
	{
		if ( its_map == NULL )
		{
			return;
		}
		
		std::map< Key, void* >& map = its_map->map;
		
		std::for_each( map.begin(),
		               map.end(),
		               map_destroyer( its_deallocator ) );
		
		delete its_map;
	}
	
	void* map_base::get( Key key, allocator a )
	{
		if ( its_map == NULL )
		{
			its_map = new simple_map_impl;
		}
		
		std::map< Key, void* >& map = its_map->map;
		
		void*& result = map[ key ];
		
		if ( result == NULL )
		{
			result = a();
		}
		
		return result;
	}
	
	void* map_base::find( Key key )
	{
		if ( its_map )
		{
			const std::map< Key, void* >& map = its_map->map;
			
			std::map< Key, void* >::const_iterator it = map.find( key );
			
			if ( it != map.end() )
			{
				return it->second;
			}
		}
		
		return NULL;
	}
	
	void map_base::erase( Key key )
	{
		if ( its_map )
		{
			its_map->map.erase( key );
		}
	}
	
}

