/*
	simple_map.hh
	-------------
*/

#ifndef GENIE_UTILITIES_SIMPLEMAP_HH
#define GENIE_UTILITIES_SIMPLEMAP_HH

// Standard C++
#include <map>


namespace Genie
{
	
	class map_base
	{
		private:
			typedef const void*  Key;
			typedef void*      (*allocator)();
			typedef void       (*deallocator)( void* );
			
			std::map< Key, void* > its_map;
			deallocator            its_deallocator;
			
			map_base           ( const map_base& );
			map_base& operator=( const map_base& );
		
		public:
			map_base( deallocator d ) : its_deallocator( d )
			{
			}
			
			~map_base();
			
			void* find( Key key );
			void* get( Key key, allocator a );
			
			void erase( Key key );
	};
	
	template < class Data >
	void* map_allocate()
	{
		return new Data();
	}
	
	template < class Data >
	void map_deallocate( void* data )
	{
		delete static_cast< Data* >( data );
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
			
			const_iterator end() const  { return NULL; }
			iterator       end()        { return NULL; }
			
			Data& get  ( Key key )  { return *(Data*) map_base::get  ( key, &map_allocate< Data > ); }
			Data* find ( Key key )  { return  (Data*) map_base::find ( key ); }
			void  erase( Key key )  {                 map_base::erase( key ); }
			
			Data& operator[]( Key key )  { return get( key ); }
	};
	
}

#endif

