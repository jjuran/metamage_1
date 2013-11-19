/*
	execve.cc
	---------
*/


#ifndef POSEVEN_FUNCTIONS_EXECVE_HH
#define POSEVEN_FUNCTIONS_EXECVE_HH

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	void execve( const char* pathname, char const* const* argv, char const* const* envp );
	
	inline void execve( char const *const *argv, char const* const* envp )
	{
		execve( argv[ 0 ], argv, envp );
	}
	
	template < class String >
	inline void execve( const String& path, char const *const *argv, char const* const* envp )
	{
		using iota::get_string_c_str;
		
		execve( get_string_c_str( path ), argv, envp );
	}
	
}

#endif

