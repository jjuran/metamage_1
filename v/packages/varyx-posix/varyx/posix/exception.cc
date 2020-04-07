/*
	exception.cc
	------------
*/

#include "varyx/posix/exception.hh"

// POSIX
#include <string.h>

// Standard C
#include <errno.h>

// poseven
#include "poseven/types/thread.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/exceptions.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/string.hh"


namespace varyx
{
namespace posix
{
	
	using namespace vlib;
	
	
	Value error_desc( int err )
	{
		if ( err == EINTR )
		{
			poseven::thread::testcancel();
		}
		
		Value error = mapping( "errno", Integer(           err   ) );
		Value desc  = mapping( "desc",  String ( strerror( err ) ) );
		
		return Value( error, desc );
	}
	
	void fd_error( int fd, int err )
	{
		Value fd_ = mapping( "fd", Integer( fd ) );
		
		Value exception( fd_, error_desc( err ) );
		
		throw_exception_object( exception );
	}
	
	void fd_error( int fd, const char* msg )
	{
		Value desc  = mapping( "desc", String( msg ) );
		Value fd_   = mapping( "fd",   Integer( fd ) );
		
		Value exception( desc, fd_ );
		
		throw_exception_object( exception );
	}
	
	void path_error( const char* path, int err )
	{
		Value path_ = mapping( "path", String( path  ) );
		
		Value exception( path_, error_desc( err ) );
		
		throw_exception_object( exception );
	}
	
	void path_error( const char* path, const char* msg )
	{
		Value desc  = mapping( "desc", String( msg  ) );
		Value path_ = mapping( "path", String( path ) );
		
		Value exception( desc, path_ );
		
		throw_exception_object( exception );
	}
	
	void throw_exception_object( const Value& mappings )
	{
		const Value object( Op_module, make_array( mappings ) );
		
		throw user_exception( object, source_spec() );
	}
	
}
}
