/*
	splat.hh
	--------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_SPLAT_HH
#define POSEVEN_EXTRAS_SPLAT_HH

// iota
#ifndef IOTA_ITERATOR_HH
#include "iota/iterator.hh"
#endif

// poseven
#ifndef POSEVEN_EXTRAS_WRITEALL_HH
#include "poseven/extras/write_all.hh"
#endif


namespace poseven
{
	
	inline
	void splat( fd_t fd, const char* buffer, size_t length )
	{
		write_all( fd, buffer, length );
	}
	
	template < class String >
	inline
	void splat( fd_t fd, const String& s )
	{
		write_all( fd, s );
	}
	
	
	void splat( const char* path, const char* buffer, size_t length );
	
	template < class String >
	inline
	void splat( const char* path, const String& s )
	{
		using iota::data;
		using iota::size;
		
		splat( path, data( s ), size( s ) );
	}
	
}

#endif
