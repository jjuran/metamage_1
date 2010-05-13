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

// POSIX
#include <utime.h>

// iota
#include "iota/string_traits.hh"

// poseven
#include "poseven/types/errno_t.hh"


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
	
	
	template < class String >
	inline void utime( const String& path )
	{
		utime( iota::get_string_c_str( path ) );
	}
	
	template < class String >
	inline void utime( const String& path, const utimbuf& time_buffer )
	{
		utime( iota::get_string_c_str( path ), time_buffer );
	}
	
	template < class String >
	inline void utime( const String& path, const time_t& mod_time )
	{
		utime( iota::get_string_c_str( path ), mod_time );
	}
	
}

#endif

