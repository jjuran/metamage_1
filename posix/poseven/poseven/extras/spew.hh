/*
	spew.hh
	-------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_SPEW_HH
#define POSEVEN_EXTRAS_SPEW_HH

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
	inline void spew( fd_t fd, const String& string )
	{
		spew( fd,
		      iota::get_string_data( string ),
		      iota::get_string_size( string ) );
	}
	
	
	void spew( const char* path, const char* buffer, size_t length );
	
	template < class String >
	inline void spew( const char* path, const String& stuff )
	{
		spew( path,
		      iota::get_string_data( stuff ),
		      iota::get_string_size( stuff ) );
	}
	
}

#endif

