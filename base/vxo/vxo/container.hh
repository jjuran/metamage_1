/*
	container.hh
	------------
*/

#ifndef VXO_CONTAINER_HH
#define VXO_CONTAINER_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	/*
		g++ 4.0 has a built-in operator[](<ptr>, int), which makes our own
		operator[](size_t) on Container and its derived classes ambiguous.
		So we define our own operator[](int) to disambiguate.
	*/
	
	class Container : public Box
	{
		public:
			enum
			{
				box_type_base = 0x70,  // 0111 0000
				box_type_mask = 0xF8,  // 1111 1000
			};
			
			typedef Box Item;
			
			static bool test( const Box& box );
			
			Container( box_type type, size_t n_items = 0 );
			
			size_t size() const  { return u.str.length; }
			
			bool empty() const  { return size() == 0; }
			
			Item* begin()  { unshare(); return (Item*) u.str.pointer; }
			Item* end()    { return begin() + size(); }
			
			Item const* begin() const  { return (Item const*) u.str.pointer; }
			Item const* end()   const  { return begin() + size(); }
			
			Item const* cbegin() const  { return begin(); }
			Item const* cend()   const  { return end();   }
			
			Item&       operator[]( size_t i )        { return begin()[ i ]; }
			Item const& operator[]( size_t i ) const  { return begin()[ i ]; }
			
			Item&       operator[]( int i )        { return begin()[ i ]; }
			Item const& operator[]( int i ) const  { return begin()[ i ]; }
			
			Item&       front()        { return begin()[ 0 ]; }
			Item const& front() const  { return begin()[ 0 ]; }
			
			Item&       back()        { return end()[ -1 ]; }
			Item const& back() const  { return end()[ -1 ]; }
			
			Item* expand_by( size_t n );
			
			Item* insert_n( Item* loc, size_t n );
			
			void erase_n( Item* loc, size_t n );
			
			void push_back( const Item& item )  { *expand_by( 1 ) = item; }
			
			Item* insert( Item* loc, const Item& item )
			{
				*(loc = insert_n( loc, 1 )) = item;
				
				return loc;
			}
			
			void erase( Item* loc )  { erase_n( loc, 1 ); }
	};
	
	template < class Type >
	class Array_of : public Container
	{
		public:
			typedef Type Item;
			
			static bool test( const Box& box );  // unimplemented
			
			explicit Array_of( size_t n = 0 ) : Container( Box_array, n )
			{
			}
			
			Item* begin()  { unshare(); return (Item*) u.str.pointer; }
			Item* end()    { return begin() + size(); }
			
			Item const* begin() const  { return (Item const*) u.str.pointer; }
			Item const* end()   const  { return begin() + size(); }
			
			Item const* cbegin() const  { return begin(); }
			Item const* cend()   const  { return end();   }
			
			Item&       operator[]( size_t i )        { return begin()[ i ]; }
			Item const& operator[]( size_t i ) const  { return begin()[ i ]; }
			
			Item&       operator[]( int i )        { return begin()[ i ]; }
			Item const& operator[]( int i ) const  { return begin()[ i ]; }
			
			Item&       front()        { return begin()[ 0 ]; }
			Item const& front() const  { return begin()[ 0 ]; }
			
			Item&       back()        { return end()[ -1 ]; }
			Item const& back() const  { return end()[ -1 ]; }
			
			void push_back( const Item& item )  { *expand_by( 1 ) = item; }
	};
	
}

#endif
