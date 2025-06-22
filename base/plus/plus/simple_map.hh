/*
	simple_map.hh
	-------------
*/

#ifndef PLUS_SIMPLEMAP_HH
#define PLUS_SIMPLEMAP_HH

// iota
#include "iota/class.hh"
#include "iota/swap.hh"


namespace plus
{
	
	struct simple_map_impl;
	
	class map_base
	{
		NO_NEW_DELETE
		
		private:
			typedef const void*  key_t;
			typedef void*      (*allocator)();
			typedef void*      (*duplicator)( const void* );
			typedef void       (*deallocator)( const void* );
			
			simple_map_impl*  its_map;
			deallocator       its_deallocator;
			
			map_base           ( const map_base& );
			map_base& operator=( const map_base& );
		
		public:
			map_base( deallocator d ) : its_map(), its_deallocator( d )
			{
			}
			
			map_base( const map_base& other, duplicator d );
			
			~map_base();
			
			const void* find( key_t key );
			const void* get( key_t key, allocator a );
			
			void erase( key_t key );
			
			void clear();
			
			void swap( map_base& x )
			{
				using iota::swap;
				
				swap( its_map,         x.its_map         );
				swap( its_deallocator, x.its_deallocator );
			}
	};
	
	inline void swap( map_base& a, map_base& b )
	{
		a.swap( b );
	}
	
	template < class Data >
	void* map_allocate()
	{
		return new Data();
	}
	
	template < class Data >
	void* map_duplicate( const void* data )
	{
		return new Data( *static_cast< const Data* >( data ) );
	}
	
	template < class Data >
	void map_deallocate( const void* data )
	{
		delete static_cast< const Data* >( data );
	}
	
	template < class Key, class Data >
	class simple_map : private map_base
	{
		public:
			typedef const Data* const_iterator;
			typedef       Data*       iterator;
			
			simple_map() : map_base( &map_deallocate< Data > )
			{
			}
			
			simple_map( const simple_map& other ) : map_base( other, &map_duplicate< Data > )
			{
			}
			
			Data& get  ( Key key )  { return *(Data*) map_base::get  ( key, &map_allocate< Data > ); }
			Data* find ( Key key )  { return  (Data*) map_base::find ( key ); }
			void  erase( Key key )  {                 map_base::erase( key ); }
			
			void clear()  { map_base::clear(); }
			
			Data& operator[]( Key key )  { return get( key ); }
			
			void swap( simple_map& x )
			{
				map_base::swap( x );
			}
	};
	
	template < class Key, class Data >
	inline void swap( simple_map< Key, Data >& a, simple_map< Key, Data >& b )
	{
		a.swap( b );
	}
	
}

#endif
