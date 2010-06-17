/*
	simple_map.cc
	-------------
*/

#include "Genie/Utilities/simple_map.hh"

// Standard C++
#include <algorithm>


namespace Genie
{
	
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
		std::for_each( its_map.begin(),
		               its_map.end(),
		               map_destroyer( its_deallocator ) );
	}
	
	void* map_base::get( Key key, allocator a )
	{
		void*& result = its_map[ key ];
		
		if ( result == NULL )
		{
			result = a();
		}
		
		return result;
	}
	
	void* map_base::find( Key key )
	{
		std::map< Key, void* >::iterator it = its_map.find( key );
		
		if ( it != its_map.end() )
		{
			return it->second;
		}
		
		return NULL;
	}
	
	void map_base::erase( Key key )
	{
		its_map.erase( key );
	}
	
}

