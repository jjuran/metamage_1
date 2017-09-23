/*
	exception.hh
	------------
*/

#ifndef EXCEPTION_HH
#define EXCEPTION_HH

// Standard C
#include <errno.h>

// plus
#include "plus/string_fwd.hh"


namespace vlib
{
	
	class Value;
	
	Value mapping( const plus::string& key, const Value& v );
	
	Value error_desc( int err );
	
	void fd_error( int fd, int err = errno );
	void fd_error( int fd, const char* msg );
	
	void path_error( const char* path, int err = errno );
	void path_error( const char* path, const char* msg );
	
	void throw_exception_object( const Value& fields );
	
}

#endif
