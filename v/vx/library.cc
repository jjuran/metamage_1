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

// debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/interpret.hh"
#include "vlib/list-utils.hh"
#include "vlib/string-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/types.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// vx
#include "exception.hh"


namespace vlib
{
	
	static
	void eval_error_handler( const plus::string& msg, const source_spec& src )
	{
		throw user_exception( String( msg ), src );
	}
	
	static
	Value v_eval( const Value& v )
	{
		Expr* expr = v.expr();
		
		const char* code = expr->left .string().c_str();
		const char* file = expr->right.string().c_str();
		
		const Value result = interpret( code, file, NULL, &eval_error_handler );
		
		if ( Expr* expr = result.expr() )
		{
			if ( expr->op == Op_export )
			{
				const Value& exported = expr->right;
				
				if ( const Symbol* sym = exported.sym() )
				{
					return sym->get();
				}
				
				list_builder    mappings;
				array_iterator  it( exported );
				
				while ( it )
				{
					const Symbol* sym = it.use().sym();
					
					ASSERT( sym );
					
					mappings.append( mapping( sym->name(), sym->get() ) );
				}
				
				return Value( Op_module, make_array( mappings ) );
			}
		}
		
		return result;
	}
	
	typedef int (*exec_f)( const char* arg0, char* const* argv );
	
	static
	Value exec_impl( const Value& v, const char* empty_msg, exec_f exec )
	{
		Expr* expr = v.expr();
		
		const Value& file = expr->left;
		const Value& args = expr->right;
		
		if ( is_empty_array( args ) )
		{
			throw user_exception( String( empty_msg ), source_spec() );
		}
		
		array_iterator it( args );
		
		expr = args.expr();
		
		size_t n = count( expr->right );
		
		const char** argv = new const char*[ n + 1 ];
		
		for ( size_t i = 0;  i < n;  ++i )
		{
			argv[ i ] = it.use().string().c_str();
		}
		
		argv[ n ] = NULL;
		
		const char* path = file.string().c_str();
		
		if ( *path == '\0' )
		{
			path = argv[ 0 ];
		}
		
		exec( path, (char**) argv );
		
		const int saved_errno = errno;
		
		delete [] argv;
		
		Value program = mapping( "program", file );
		
		Value exception( program, error_desc( saved_errno ) );
		
		throw_exception_object( exception );
		
		return Value();  // not reached
	}
	
	static
	Value v_execv( const Value& v )
	{
		return exec_impl( v, "empty array passed to execv()", &execv );
	}
	
	static
	Value v_execvp( const Value& v )
	{
		return exec_impl( v, "empty array passed to execvp()", &execvp );
	}
	
	static
	Value v_exit( const Value& v )
	{
		exit( v.number().clipped() );
		
		return Value();  // not reached
	}
	
	static
	Value v_getenv( const Value& v )
	{
		if ( const char* var = getenv( v.string().c_str() ) )
		{
			return String( var );
		}
		
		return Value_empty_list;
	}
	
	static
	Value v_print( const Value& v )
	{
		plus::var_string s = str( v ).move();
		
		s += "\n";
		
		must_write( STDOUT_FILENO, s.data(), s.size() );
		
		return Value_nothing;
	}
	
	static
	Value v_secret( const Value& v )
	{
		return v.secret();
	}
	
	static
	Value v_sleep( const Value& v )
	{
		sleep( v.number().clipped() );
		
		return Value_nothing;
	}
	
	static
	Value v_system( const Value& v )
	{
		return Value_empty_list;
	}
	
	static
	Value v_SYSTEM( const Value& v )
	{
		if ( is_empty_list( v ) )
		{
			return Boolean( system( NULL ) != 0 );
		}
		
		const int status = system( v.string().c_str() );
		
		if ( status == 0 )
		{
			return Integer();
		}
		
		Value command = mapping( "command", v );
		
		if ( status < 0 )
		{
			Value exception( command, error_desc( errno ) );
			
			throw_exception_object( exception );
		}
		
		if ( WIFSIGNALED( status ) )
		{
			const Integer signo = WTERMSIG( status );
			
			Value signal = mapping( "signal", signo );
			
			Value exception( command, signal );
			
			throw_exception_object( exception );
		}
		
		if ( int exit_status = WEXITSTATUS( status ) )
		{
			Value exit = mapping( "exit", Integer( exit_status ) );
			
			Value exception( command, exit );
			
			throw_exception_object( exception );
		}
		
		return Integer();  // not reached
	}
	
	static
	Value v_time( const Value& v )
	{
		const time_t t = time( NULL );
		
		return Integer( t );
	}
	
	static
	Value v_warn( const Value& v )
	{
		plus::var_string s = str( v ).move();
		
		s += "\n";
		
		must_write( STDERR_FILENO, s.data(), s.size() );
		
		return Value_nothing;
	}
	
	static const Type c_str = c_str_vtype;
	static const Type u8    = u8_vtype;
	static const Type u32   = u32_vtype;
	
	static const Value maybe_cstr( c_str, Op_union, Value_empty_list );
	
	static const Value cstr_eval = Value( c_str,
	                                      Op_duplicate,
	                                      String( "<eval>" ) );
	
	static const Value c_str_array = Value( c_str, Op_subscript, empty_list );
	
	static const Value eval = Value( c_str, cstr_eval );
	static const Value exec = Value( c_str, c_str_array );
	
	#define DESTRUCT  "self-destructing"
	
	const proc_info proc_eval   = { "eval",   &v_eval,   &eval       };
	const proc_info proc_EXECV  = { "execv",  &v_execv,  &exec       };
	const proc_info proc_EXECVP = { "execvp", &v_execvp, &exec       };
	const proc_info proc_exit   = { "exit",   &v_exit,   &u8         };
	const proc_info proc_getenv = { "getenv", &v_getenv, &c_str      };
	const proc_info proc_print  = { "print",  &v_print,  NULL        };
	const proc_info proc_secret = { DESTRUCT, &v_secret, NULL        };
	const proc_info proc_sleep  = { "sleep",  &v_sleep,  &u32        };
	const proc_info proc_system = { "system", &v_system, &empty_list };
	const proc_info proc_SYSTEM = { "system", &v_SYSTEM, &maybe_cstr };
	const proc_info proc_time   = { "time",   &v_time,   &empty_list };
	const proc_info proc_warn   = { "warn",   &v_warn,   NULL        };
	
}
