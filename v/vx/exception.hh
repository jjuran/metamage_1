/*
	exception.hh
	------------
*/

#ifndef EXCEPTION_HH
#define EXCEPTION_HH

// Standard C
#include <errno.h>


namespace vlib
{
	
	class Value;
	
	Value error_desc( int err );
	
	void fd_error( int fd, int err = errno );
	void fd_error( int fd, const char* msg );
	
	void path_error( const char* path, int err = errno );
	void path_error( const char* path, const char* msg );
	
	void throw_exception_object( const Value& fields );
	
}

#endif
