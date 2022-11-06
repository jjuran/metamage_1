/*
	interpret.cc
	------------
*/

#include "vlib/interpret.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// Standard C++
#include <new>

// must
#include "must/write.h"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// vlib
#include "vlib/analyze.hh"
#include "vlib/exceptions.hh"
#include "vlib/execute.hh"
#include "vlib/parse.hh"
#include "vlib/return.hh"
#include "vlib/startup.hh"
#include "vlib/string-utils.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  ::vlib::fail( STR_LEN( "ERROR: " s "\n" ) )


namespace vlib
{
	
	Value identity_function( const Value& value )
	{
		return value;
	}
	
	static
	void fail( const char* msg, unsigned len )
	{
		must_write( STDERR_FILENO, msg, len );
	
		exit( 1 );
	}
	
	static
	void fail( const plus::string& s, const source_spec& src )
	{
		plus::var_string msg;
		
		if ( ! src.empty() )
		{
			msg += src.file();
			msg += ":";
			msg += gear::inscribe_unsigned_decimal( src.line() );
			msg += "\n    ";
		}
		
		msg += s;
		msg += "\n";
		
		must_write( STDERR_FILENO, msg.data(), msg.size() );
		
		exit( 1 );
	}
	
	static
	void fail_handler( const plus::string& msg, const source_spec& src )
	{
		fail( msg, src );
	}
	
	Value interpret( const char*     program,
	                 const char*     file,
	                 lexical_scope*  globals,
	                 error_handler   handler,
	                 execute_proc    execute )
	{
		if ( handler == NULL )
		{
			handler = &fail_handler;
		}
		
		if ( execute == NULL )
		{
			execute = &vlib::execute;
		}
		
		try
		{
			static int startup = (inject_startup_header( globals ), 0);
			
			return execute( analyze( parse( program, file ), globals ) );
		}
		catch ( const std::bad_alloc& )
		{
			FAIL( "Out of memory!" );
		}
		catch ( const transfer_via_break& e )
		{
			plus::string msg = "ERROR: `break` used outside of loop";
			
			handler( msg, e.source );
		}
		catch ( const transfer_via_continue& e )
		{
			plus::string msg = "ERROR: `continue` used outside of loop";
			
			handler( msg, e.source );
		}
		catch ( const transfer_via_return& e )
		{
			plus::var_string msg = "ERROR: `return` of value ";
			
			msg += rep( e.object );
			msg += " outside of function.";
			
			fail( msg, e.source );
		}
		catch ( const language_error& e )
		{
			plus::var_string msg = "ERROR: ";
			
			msg += e.text;
			
			handler( msg, e.source );
		}
		catch ( const user_exception& e )
		{
			plus::var_string msg = "ERROR: uncaught exception: ";
			
			msg += rep( e.object );
			
			handler( msg, e.source );
		}
		catch ( const invalid_token_error& e )
		{
			plus::var_string msg = "ERROR: invalid token '";
			
			msg += e.token;
			msg += "'";
			
			handler( msg, e.source );
		}
		catch ( const undeclared_symbol_error& e )
		{
			plus::var_string msg = "ERROR: undeclared symbol '";
			
			msg += e.name;
			msg += "'";
			
			handler( msg, e.source );
		}
		
		return Value();
	}
	
}
