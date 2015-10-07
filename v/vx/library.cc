/*
	library.cc
	----------
*/

#include "library.hh"

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <time.h>

// must
#include "must/write.h"

// plus
#include "plus/var_string.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/interpret.hh"
#include "vlib/string-utils.hh"
#include "vlib/types.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	static
	void eval_error_handler( const plus::string& msg, const source_spec& src )
	{
		throw user_exception( msg, src );
	}
	
	static
	Value v_eval( const Value& v )
	{
		Expr* expr = v.expr();
		
		const char* code = expr->left .string().c_str();
		const char* file = expr->right.string().c_str();
		
		return interpret( code, file, NULL, &eval_error_handler );
	}
	
	static
	Value v_getenv( const Value& v )
	{
		if ( const char* var = getenv( get_str( v ).c_str() ) )
		{
			return var;
		}
		
		return Value_empty_list;
	}
	
	static
	Value v_print( const Value& v )
	{
		plus::var_string s = make_string( v, Stringified_to_print ).move();
		
		s += "\n";
		
		must_write( STDOUT_FILENO, s.data(), s.size() );
		
		return Value();
	}
	
	static
	Value v_system( const Value& v )
	{
		return Value_empty_list;
	}
	
	static
	Value v_SYSTEM( const Value& v )
	{
		if ( is_empty( v ) )
		{
			return system( NULL ) != 0;
		}
		
		const int status = system( v.string().c_str() );
		
		if ( status == 0 )
		{
			return 0;
		}
		
		Value command( "command", Op_mapping, v );
		
		if ( status < 0 )
		{
			int err = errno;
			
			Value error( "errno", Op_mapping,           err   );
			Value desc ( "desc",  Op_mapping, strerror( err ) );
			
			Value exception( command, Value( error, desc ) );
			
			throw user_exception( exception, source_spec( NULL ) );
		}
		
		if ( WIFSIGNALED( status ) )
		{
			const int signo = WTERMSIG( status );
			
			Value signal( "signal", Op_mapping, signo );
			
			Value exception( command, signal );
			
			throw user_exception( exception, source_spec( NULL ) );
		}
		
		if ( int exit_status = WEXITSTATUS( status ) )
		{
			Value exit( "exit", Op_mapping, exit_status );
			
			Value exception( command, exit );
			
			throw user_exception( exception, source_spec( NULL ) );
		}
		
		return status;  // not reached
	}
	
	static
	Value v_time( const Value& v )
	{
		const time_t t = time( NULL );
		
		return t;
	}
	
	static const Value c_str = c_str_vtype;
	
	static const Value maybe_cstr( c_str_vtype, Op_union, Value_empty_list );
	
	static const Value cstr_eval = Value( c_str_vtype, Op_duplicate, "<eval>" );
	
	static const Value eval = Value( c_str, cstr_eval );
	
	const proc_info proc_eval   = { &v_eval,   "eval",   &eval       };
	const proc_info proc_getenv = { &v_getenv, "getenv", &c_str      };
	const proc_info proc_print  = { &v_print,  "print",  NULL        };
	const proc_info proc_system = { &v_system, "system", &empty_list };
	const proc_info proc_SYSTEM = { &v_SYSTEM, "system", &maybe_cstr };
	const proc_info proc_time   = { &v_time,   "time",   &empty_list };
	
}
