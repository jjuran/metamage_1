// perror.cc
// ---------

// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.

#include "poseven/functions/perror.hh"

// Standard C
#include <string.h>

// poseven
#include "poseven/functions/write.hh"


namespace poseven
{
	
	static void write_line( std::string& message )
	{
		message += '\n';
		
		int written = ::write( stderr_fileno, message.data(), message.size() );
		
		if ( written != message.size() )
		{
			// Rationale:  If the write to stderr fails, something's whacked
			// (which will happen if the terminal is disconnected) and we have to
			// avoid further attempts to report diagnostics.  The safest thing to
			// do is exit immediately.
			
			const int exit_stderr_broken = 63;
			
			::_exit( exit_stderr_broken );
		}
	}
	
	static void write_error_line( std::string& message, bool extend, int errnum )
	{
		if ( extend  &&  errnum != 0 )
		{
			message += ": ";
		}
		
		if ( !extend  ||  errnum != 0 )
		{
			message += strerror( errnum );
		}
		
		write_line( message );
	}
	
	
	void perror( int errnum )
	{
		std::string message;
		
		write_error_line( message, false, errnum );
	}
	
	void perror( const char* s, int errnum )
	{
		std::string message = s;
		
		write_error_line( message, true, errnum );
	}
	
	void perror( const char* s1, const char* s2, int errnum )
	{
		std::string message = s1;
		
		message += ": ";
		message += s2;
		
		write_error_line( message, true, errnum );
	}
	
	void perror( const char* s1, const char* s2, const char* s3 )
	{
		std::string message = s1;
		
		message += ": ";
		message += s2;
		message += ": ";
		message += s3;
		
		write_line( message );
	}
	
	void perror( const std::string& s, int errnum )
	{
		std::string message = s;
		
		write_error_line( message, true, errnum );
	}
	
	void perror( const std::string& s1, const std::string& s2, int errnum )
	{
		std::string message = s1;
		
		message += ": ";
		message += s2;
		
		write_error_line( message, true, errnum );
	}
	
	void perror( const std::string& s1, const std::string& s2, const std::string& s3 )
	{
		std::string message = s1;
		
		message += ": ";
		message += s2;
		message += ": ";
		message += s3;
		
		write_line( message );
	}
	
}

