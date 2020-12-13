/*
	strmap.hh
	---------
*/

#ifndef VXO_STRMAP_HH
#define VXO_STRMAP_HH

// vxo
#include "vxo/container.hh"
#include "vxo/string.hh"


namespace vxo
{
	
	class StrMap : public Container
	{
		public:
			struct Pair
			{
				String const  first;
				Box           second;
			};
			
			typedef Pair value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type&       reference;
			typedef value_type const& const_reference;
			
			static bool test( const Box& box );  // unimplemented
			
			explicit StrMap( size_t n = 0 ) : Container( Box_map, n * 2 )
			{
			}
			
			size_t size() const  { return u.str.length / 2; }
			
			Pair* begin()  { unshare(); return (Pair*) u.str.pointer; }
			Pair* end()    { return begin() + size(); }
			
			Pair const* begin() const  { return (Pair const*) u.str.pointer; }
			Pair const* end()   const  { return begin() + size(); }
			
			Pair const* cbegin() const  { return begin(); }
			Pair const* cend()   const  { return end();   }
			
			iterator find( const plus::string& key );
			iterator found( const plus::string& key );
			
			const_iterator find( const plus::string& key ) const
			{
				return const_cast< StrMap* >( this )->find( key );
			}
			
			const_iterator found( const plus::string& key ) const
			{
				return const_cast< StrMap* >( this )->found( key );
			}
			
			Box& operator[]( const plus::string& key );
			
			void erase( iterator loc )
			{
				erase_n( (Box*) loc, 2 );
			}
			
			bool erase( const plus::string& key )
			{
				if ( iterator loc = found( key ) )
				{
					erase( loc );
					
					return true;
				}
				
				return false;
			}
	};
	
	template < class Value >
	class StrMap_to : public Container
	{
		public:
			struct Pair
			{
				String const  first;
				Value         second;
			};
			
			typedef Pair value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type&       reference;
			typedef value_type const& const_reference;
			
			static bool test( const Box& box );  // unimplemented
			
			explicit StrMap_to( size_t n = 0 ) : Container( Box_map, n * 2 )
			{
			}
			
			size_t size() const  { return u.str.length / 2; }
			
			Pair* begin()  { unshare(); return (Pair*) u.str.pointer; }
			Pair* end()    { return begin() + size(); }
			
			Pair const* begin() const  { return (Pair const*) u.str.pointer; }
			Pair const* end()   const  { return begin() + size(); }
			
			Pair const* cbegin() const  { return begin(); }
			Pair const* cend()   const  { return end();   }
			
			iterator find( const plus::string& key )
			{
				return (iterator) ((StrMap*) this)->find( key );
			}
			
			iterator found( const plus::string& key )
			{
				return (iterator) ((StrMap*) this)->found( key );
			}
			
			const_iterator find( const plus::string& key ) const
			{
				return const_cast< StrMap_to* >( this )->find( key );
			}
			
			const_iterator found( const plus::string& key ) const
			{
				return const_cast< StrMap_to* >( this )->found( key );
			}
			
			Value& operator[]( const plus::string& key )
			{
				return static_cast< Value& >( (*(StrMap*) this)[ key ] );
			}
			
			void erase( iterator loc )
			{
				erase_n( (Box*) loc, 2 );
			}
			
			bool erase( const plus::string& key )
			{
				if ( iterator loc = found( key ) )
				{
					erase( loc );
					
					return true;
				}
				
				return false;
			}
	};
	
}

#endif
