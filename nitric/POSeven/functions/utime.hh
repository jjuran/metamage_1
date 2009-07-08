// utime.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_UTIME_HH
#define POSEVEN_FUNCTIONS_UTIME_HH

// Standard C++
#include <string>

// POSIX
#include <utime.h>

// POSeven
#include "POSeven/types/errno_t.hh"


namespace poseven
{
	
	inline void utime( const char* path )
	{
		throw_posix_result( ::utime( path, NULL ) );
	}
	
	inline void utime( const char* path, const utimbuf& time_buffer )
	{
		throw_posix_result( ::utime( path, &time_buffer ) );
	}
	
	inline void utime( const char* path, const time_t& mod_time )
	{
		utimbuf time_buffer = { 0, mod_time };
		
		utime( path, time_buffer );
	}
	
	inline void utime( const std::string& path )
	{
		utime( path.c_str() );
	}
	
	inline void utime( const std::string& path, const utimbuf& time_buffer )
	{
		utime( path.c_str(), time_buffer );
	}
	
	inline void utime( const std::string& path, const time_t& mod_time )
	{
		utime( path.c_str(), mod_time );
	}
	
}

#endif

