/*
	iota/iterator.hh
	----------------
*/

#ifndef IOTA_ITERATOR_HH
#define IOTA_ITERATOR_HH

// Standard C/C++
#include <cstddef>


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
	
}

#endif
