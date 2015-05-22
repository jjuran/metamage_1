/*
	iota/iterator.hh
	----------------
*/

#ifndef IOTA_ITERATOR_HH
#define IOTA_ITERATOR_HH

// Standard C/C++
#include <cstring>


namespace iota
{
	
	template < class T > typename T::iterator begin( T& x )  { return x.begin(); }
	template < class T > typename T::iterator end  ( T& x )  { return x.end  (); }
	
	template < class T > typename T::const_iterator begin( const T& x )  { return x.begin(); }
	template < class T > typename T::const_iterator end  ( const T& x )  { return x.end  (); }
	
	template < class T > typename T::const_iterator cbegin( const T& x )  { return begin( x ); }
	template < class T > typename T::const_iterator cend  ( const T& x )  { return end  ( x ); }
	
	// Must be customized for types where const_iterator isn't (const char*).
	template < class T >
	inline const char* data( const T& x )  { return begin( x ); }
	
	template < class T > std::size_t size( const T& x )  { return x.size(); }
	
	struct span
	{
		const char*  ptr;
		std::size_t  len;
		
		span( const char* p, std::size_t n ) : ptr( p ), len( n )
		{
		}
		
		const char* begin() const  { return ptr;       }
		const char* end  () const  { return ptr + len; }
		const char* data () const  { return ptr;       }
		std::size_t size () const  { return       len; }
		
		const char& operator[]( std::size_t i ) const  { return ptr[ i ]; }
	};
	
	inline span make_span( const span& s )
	{
		return s;
	}
	
	inline span make_span( const char* s )
	{
		return span( s, std::strlen( s ) );
	}
	
	template < class T >
	inline span make_span( const T& x )
	{
		return span( data( x ), size( x ) );
	}
}

#endif
