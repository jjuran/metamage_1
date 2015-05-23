/*
	spew.hh
	-------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_SPEW_HH
#define POSEVEN_EXTRAS_SPEW_HH

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
	
	inline void spew( fd_t fd, const char* buffer, size_t length )
	{
		write_all( fd, buffer, length );
	}
	
	template < class String >
	inline void spew( fd_t fd, const String& s )
	{
		write_all( fd, s );
	}
	
	
	void spew( const char* path, const char* buffer, size_t length );
	
	template < class String >
	inline void spew( const char* path, const String& s )
	{
		using iota::data;
		using iota::size;
		
		spew( path, data( s ), size( s ) );
	}
	
}

#endif
