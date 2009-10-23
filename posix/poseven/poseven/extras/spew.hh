/*
	spew.hh
	-------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_SPEW_HH
#define POSEVEN_EXTRAS_SPEW_HH

// Standard C++
#include <string>

// poseven
#include "poseven/extras/write_all.hh"


namespace poseven
{
	
	inline void spew( fd_t fd, const char* buffer, std::size_t length )
	{
		write_all( fd, buffer, length );
	}
	
	
	void spew( const char* path, const char* buffer, std::size_t length );
	
	
	inline void spew( const char* path, const std::string& stuff )
	{
		spew( path, stuff.data(), stuff.length() );
	}
	
	inline void spew( const std::string& path, const char* buffer, std::size_t length )
	{
		spew( path.c_str(), buffer, length );
	}
	
	inline void spew( const std::string& path, const std::string& stuff )
	{
		spew( path.c_str(), stuff );
	}
	
}

#endif

