/*
	ptrvec.hh
	---------
*/

#ifndef VXO_PTRVEC_HH
#define VXO_PTRVEC_HH

// vxo
#include "vxo/box.hh"


namespace vxo
{
	
	class PtrVec : public Box
	{
		public:
			typedef anyptr_t Item, value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type& reference;
			typedef value_type  const_reference;
			
			static bool test( const Box& box );  // unimplemented
			
			explicit PtrVec( size_t n = 0 );
			
			size_t size() const  { return u.str.length; }
			
			bool empty() const  { return size() == 0; }
			
			Item* begin()  { return (Item*) u.str.pointer; }
			Item* end()    { return begin() + size(); }
			
			Item const* begin() const  { return (Item const*) u.str.pointer; }
			Item const* end()   const  { return begin() + size(); }
			
			Item const* cbegin() const  { return begin(); }
			Item const* cend()   const  { return end();   }
			
			Item& operator[]( size_t i )        { return begin()[ i ]; }
			Item  operator[]( size_t i ) const  { return begin()[ i ]; }
			
			Item& operator[]( int i )        { return begin()[ i ]; }
			Item  operator[]( int i ) const  { return begin()[ i ]; }
			
			Item& front()        { return begin()[ 0 ]; }
			Item  front() const  { return begin()[ 0 ]; }
			
			Item& back()        { return end()[ -1 ]; }
			Item  back() const  { return end()[ -1 ]; }
			
			Item* expand_by_nothrow( size_t n );
			Item* expand_by        ( size_t n );
			
			Item* insert_n_nothrow( Item* loc, size_t n );
			Item* insert_n        ( Item* loc, size_t n );
			
			void insert( Item* loc, const_iterator begin, const_iterator end );
			
			Item* insert( Item* loc )  { return insert_n( loc, 1 ); }
			
			void erase_n( Item* loc, size_t n );
			
			void push_back( Item item )  { *expand_by( 1 ) = item; }
			
			void pop_back()  { --u.str.length; }
			
			void clear()    { u.str.length = 0; }
	};
	
	bool operator==( const PtrVec& a, const PtrVec& b );
	
	inline
	bool operator!=( const PtrVec& a, const PtrVec& b )
	{
		return !( a == b );
	}
	
	template < class T >
	class PtrVec_ : public PtrVec
	{
		public:
			typedef T* Item;
			typedef Item value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type& reference;
			typedef value_type  const_reference;
			
			static bool test( const Box& box );  // unimplemented
			
			explicit PtrVec_( size_t n = 0 ) : PtrVec( n )
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
			
			void insert( Item* loc, const_iterator begin, const_iterator end )
			{
				unshare();
				
				PtrVec::insert( (anyptr_t*) loc,
				                (anyptr_t*) begin,
				                (anyptr_t*) end );
			}
			
			Item* insert( Item* loc )
			{
				unshare();
				
				return (Item*) PtrVec::insert( (anyptr_t*) loc );
			}
			
			void insert( Item* loc, Item item )
			{
				*insert( loc ) = item;
			}
			
			void erase( Item* loc )
			{
				unshare();
				
				PtrVec::erase_n( (anyptr_t*) loc, 1 );
			}
			
			void push_back( Item item )  { unshare(); *expand_by( 1 ) = item; }
			
			void pop_back()              { unshare(); --u.str.length; }
			void clear()                 { unshare(); u.str.length = 0; }
	};
	
	template < class T >
	class UniPtrVec_ : public PtrVec
	{
		private:
			// non-copyable
			UniPtrVec_           ( const UniPtrVec_& );
			UniPtrVec_& operator=( const UniPtrVec_& );
		
		public:
			typedef T* Item;
			typedef Item value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type& reference;
			typedef value_type  const_reference;
			
			static bool test( const Box& box );  // unimplemented
			
			explicit UniPtrVec_( size_t n = 0 ) : PtrVec( n )
			{
			}
			
			Item* begin()  { return (Item*) u.str.pointer; }
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
			
			void insert( Item* loc, const_iterator begin, const_iterator end )
			{
				PtrVec::insert( (anyptr_t*) loc,
				                (anyptr_t*) begin,
				                (anyptr_t*) end );
			}
			
			Item* insert( Item* loc )
			{
				return (Item*) PtrVec::insert( (anyptr_t*) loc );
			}
			
			void insert( Item* loc, Item item )
			{
				*insert( loc ) = item;
			}
			
			void erase( Item* loc )
			{
				PtrVec::erase_n( (anyptr_t*) loc, 1 );
			}
	};
	
}

#endif
