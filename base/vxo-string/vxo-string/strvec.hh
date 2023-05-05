/*
	strvec.hh
	---------
*/

#ifndef VXOSTRING_STRVEC_HH
#define VXOSTRING_STRVEC_HH

// vxo
#include "vxo/container.hh"

// vxo-string
#include "vxo-string/string.hh"


namespace vxo
{
	
	class StrVec : public Container
	{
		public:
			typedef plus::string Item, value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type&       reference;
			typedef value_type const& const_reference;
			
			static bool test( const Box& box );  // unimplemented
			
			explicit StrVec( size_t n = 0 ) : Container( Box_array, n )
			{
			}
			
			explicit StrVec( const Item& string ) : Container( Box_array, 1 )
			{
				push_back( string );
			}
			
			Item* begin()  { unshare();  return (Item*) u.str.pointer; }
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
			
			void insert( Item* loc, const_iterator begin, const_iterator end );
			
			void insert( Item* loc, const Item& item )
			{
				insert( loc, &item, &item + 1 );
			}
			
			void resize_down( size_t n );
			
			void push_back( const Item& item )
			{
				static_cast< String& >( *expand_by( 1 ) ) = item;
			}
			
			void pop_back()  { resize_down( size() - 1 ); }
	};
	
	bool operator==( const StrVec& a, const StrVec& b );
	
	inline
	bool operator!=( const StrVec& a, const StrVec& b )
	{
		return !( a == b );
	}
	
}

#endif
