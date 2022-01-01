/*
	simple_map.cc
	-------------
*/

#include "plus/simple_map.hh"

// Standard C++
#include <map>

// iota
#include "iota/swap.hh"


namespace plus
{
	
	typedef std::map< unsigned long, const void* > simple_map_type;
	
	
	struct simple_map_impl
	{
		simple_map_type map;
	};
	
	class map_destroyer
	{
		private:
			typedef void (*deallocator)( const void* );
			
			typedef simple_map_type::value_type value_type;
			
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
	
	map_base::map_base( const map_base& other, duplicator duplicate )
	:
		its_map(),
		its_deallocator( other.its_deallocator )
	{
		if ( other.its_map )
		{
			map_base temp( its_deallocator );
			
			temp.its_map = new simple_map_impl;
			
			simple_map_type& map       =  temp.its_map->map;
			simple_map_type& other_map = other.its_map->map;
			
			typedef simple_map_type::const_iterator Iter;
			
			for ( Iter it = other_map.begin();  it != other_map.end();  ++it )
			{
				const void*& slot = map[ it->first ];
				
				slot = duplicate( it->second );
			}
			
			using iota::swap;
			
			swap( its_map, temp.its_map );
		}
	}
	
	void map_base::clear()
	{
		if ( its_map == NULL )
		{
			return;
		}
		
		simple_map_type& map = its_map->map;
		
		typedef simple_map_type::const_iterator Iter;
		
		Iter end = map.end();
		
		for ( Iter it = map.begin();  it != end;  ++it )
		{
			its_deallocator( it->second );
		}
		
		map.clear();
	}
	
	map_base::~map_base()
	{
		clear();
		
		delete its_map;
	}
	
	const void* map_base::get( key_t key, allocator a )
	{
		if ( its_map == NULL )
		{
			its_map = new simple_map_impl;
		}
		
		simple_map_type& map = its_map->map;
		
		const void*& result = map[ (unsigned long) key ];
		
		if ( result == NULL )
		{
			result = a();
		}
		
		return result;
	}
	
	const void* map_base::find( key_t key )
	{
		if ( its_map )
		{
			const simple_map_type& map = its_map->map;
			
			simple_map_type::const_iterator it = map.find( (unsigned long) key );
			
			if ( it != map.end() )
			{
				return it->second;
			}
		}
		
		return NULL;
	}
	
	void map_base::erase( key_t key )
	{
		if ( its_map )
		{
			typedef simple_map_type::iterator Iter;
			
			simple_map_type& map = its_map->map;
			
			Iter it = map.find( (unsigned long) key );
			
			if ( it != map.end() )
			{
				its_deallocator( it->second );
				
				map.erase( it );
			}
		}
	}
	
}
