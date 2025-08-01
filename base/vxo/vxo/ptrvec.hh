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
			
			PtrVec();
			
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
			
			Item* insert_nothrow( Item* loc, const_iterator begin, const_iterator end );
			
			void insert( Item* loc, const_iterator begin, const_iterator end );
			
			Item* insert_nothrow( Item* loc )  { return insert_n_nothrow( loc, 1 ); }
			
			Item* insert( Item* loc )  { return insert_n( loc, 1 ); }
			
			Item* resize_nothrow( size_t n );
			
			void resize( size_t n );
			
			void erase_n( Item* loc, size_t n );
			
			Item* push_back_nothrow( Item item )
			{
				Item* loc = expand_by_nothrow( 1 );
				
				if ( loc )
				{
					*loc = item;
				}
				
				return loc;
			}
			
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
			
			PtrVec_()
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
	class UPtrVec : public PtrVec
	{
		private:
			// non-copyable
			UPtrVec           ( const UPtrVec& );
			UPtrVec& operator=( const UPtrVec& );
		
		public:
			typedef T* Item;
			typedef Item value_type;
			
			typedef value_type*       iterator;
			typedef value_type const* const_iterator;
			
			typedef value_type& reference;
			typedef value_type  const_reference;
			
			static bool test( const Box& box );  // unimplemented
			
			UPtrVec()
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
			
			Item* insert_nothrow( Item* loc )
			{
				return (Item*) PtrVec::insert_nothrow( (anyptr_t*) loc );
			}
			
			Item* insert( Item* loc )
			{
				return (Item*) PtrVec::insert( (anyptr_t*) loc );
			}
			
			Item* insert_nothrow( Item* loc, Item item )
			{
				if ( (loc = insert_nothrow( loc )) )
				{
					*loc = item;
				}
				
				return loc;
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
