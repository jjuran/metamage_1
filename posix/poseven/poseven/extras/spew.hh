/*
	spew.hh
	-------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_SPEW_HH
#define POSEVEN_EXTRAS_SPEW_HH

// plus
#include "plus/string.hh"

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
	
	
	void spew( const char* path, const char* buffer, size_t length );
	
	
	inline void spew( const char* path, const plus::string& stuff )
	{
		spew( path, stuff.data(), stuff.length() );
	}
	
}

#endif

