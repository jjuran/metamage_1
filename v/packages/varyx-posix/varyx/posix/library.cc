/*
	library.cc
	----------
*/

#include "library.hh"

// POSIX
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <time.h>

// must
#include "must/write.h"

// plus
#include "plus/var_string.hh"

// bignum
#include "bignum/integer.hh"

// poseven
#include "poseven/types/thread.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/exceptions.hh"
#include "vlib/list-utils.hh"
#include "vlib/string-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/types.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/fraction.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// varyx-posix
#include "varyx/posix/exception.hh"


#ifdef __APPLE__
#ifndef _USECONDS_T
typedef unsigned int useconds_t;
#endif
#endif

#ifdef ANDROID
#define FORK() fork()
#else
#define FORK() vfork()
#endif


namespace varyx
{
namespace posix
{
	
	using namespace vlib;
	
	
	typedef int (*exec_f)( const char* arg0, char* const* argv );
	
	static
	Value exec_impl( const Value&  v,
	                 const char*   empty_msg,
	                 exec_f        exec,
	                 bool          throwing = true )
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
		
		if ( throwing )
		{
			throw_exception_object( exception );
		}
		
		return Integer( saved_errno );
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
	Value v_getpid( const Value& v )
	{
		return Integer( getpid() );
	}
	
	static
	Value v_print( const Value& v )
	{
		plus::var_string s = str( v ).move();
		
		s += "\n";
		
		must_write( STDOUT_FILENO, s.data(), s.size() );
		
		return Value_nothing;
	}
	
	enum output_capture
	{
		Capture_none,
		Capture_stdout,
	};
	
	static
	void throw_run_exception( const char* type, int status, const Value& argv )
	{
		Value stat = mapping( type, Integer( status ) );
		Value args = mapping( "argv", argv );
		
		throw_exception_object( Value( stat, args ) );
	}
	
	static
	Value run( const Value& v, const char* empty, output_capture capture )
	{
		if ( is_empty_array( v ) )
		{
			throw user_exception( String( empty ), source_spec() );
		}
		
		int fds[ 2 ];
		
		if ( capture )
		{
			if ( pipe( fds ) < 0 )
			{
				throw_exception_object( error_desc( errno ) );
			}
		}
		
		pid_t pid = FORK();
		
		if ( pid < 0 )
		{
			if ( capture )
			{
				close( fds[ 0 ] );
				close( fds[ 1 ] );
			}
			
			throw_exception_object( error_desc( errno ) );
		}
		
		if ( pid == 0 )
		{
			if ( capture )
			{
				close( fds[ 0 ] );
				
				if ( dup2( fds[ 1 ], STDOUT_FILENO ) < 0 )
				{
					_exit( 126 );
				}
				
				close( fds[ 1 ] );
			}
			
			const Value& arg0 = first( v.expr()->right );
			
			const Value err = exec_impl( Value( arg0, v ), "", &execvp, false );
			
			const int errnum = integer_cast< int >( err );
			
			_exit( errnum == ENOENT ? 127 : 126 );
		}
		
		Value result = Integer();
		
		if ( capture )
		{
			close( fds[ 1 ] );
			
			plus::var_string output;
			
			char buffer[ 4096 ];
			
			ssize_t n_read;
			
			while ( (n_read = read( fds[ 0 ], buffer, sizeof buffer )) > 0 )
			{
				output.append( buffer, n_read );
			}
			
			int saved_errno = errno;
			
			close( fds[ 0 ] );
			
			if ( n_read < 0 )
			{
				throw_exception_object( error_desc( saved_errno ) );
			}
			
			result = String( output );
		}
		
	retry:
		
		int status = -1;
		pid_t waited = waitpid( pid, &status, 0 );
		
		if ( waited < 0 )
		{
			if ( errno == EINTR )
			{
				goto retry;
			}
			
			throw_exception_object( error_desc( errno ) );
		}
		
		if ( WIFSIGNALED( status ) )
		{
			const int termsig = WTERMSIG( status );
			
			throw_run_exception( "signal", termsig, v );
		}
		
		if ( const int exit_status = WEXITSTATUS( status ) )
		{
			throw_run_exception( "exit", exit_status, v );
		}
		
		return result;
	}
	
	static
	Value v_run( const Value& v )
	{
		const char* empty = "empty array passed to run()";
		
		return run( v, empty, Capture_none );
	}
	
	static
	Value v_runout( const Value& v )
	{
		const char* empty = "empty array passed to output-from-run()";
		
		return run( v, empty, Capture_stdout );
	}
	
	static
	Value v_setenv( const Value& v )
	{
		Expr* expr = v.expr();
		
		const char* name  = expr->left.string().c_str();
		const char* value = expr->right.string().c_str();
		
		int nok = setenv( name, value, 1 );
		
		if ( nok )
		{
			throw_exception_object( error_desc( errno ) );
		}
		
		return Value_nothing;
	}
	
	static
	Value v_sleep( const Value& v )
	{
		const useconds_t max_useconds = useconds_t( -1 );
		
		if ( const Fraction* fract = v.is< Fraction >() )
		{
			const bignum::integer& numer = fract->numerator  ().get();
			const bignum::integer& denom = fract->denominator().get();
			
			const bignum::integer approx_useconds = numer * 1000000 / denom;
			
			if ( numer.is_negative()  ||  approx_useconds > max_useconds )
			{
				// FIXME:  This isn't a clear error message.
				
				Value exception( mapping( "sleep", v ) );
				
				throw_exception_object( exception );
			}
			
			const useconds_t delay = approx_useconds.clipped_to< useconds_t >();
			
			usleep( delay );
		}
		else
		{
			sleep( v.number().clipped() );
		}
		
		poseven::thread::testcancel();
		
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
	Value v_utime( const Value& v )
	{
		timeval tv = { 0 };
		
		int nok = gettimeofday( &tv, NULL );
		
		return Integer( tv.tv_sec * 1000000ull + tv.tv_usec );
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
	static const Type fract = fraction_vtype;
	
	static const Value cstr_x2( c_str, c_str );
	
	static const Value sleep_arg ( u32,   Op_union, fract );
	static const Value maybe_cstr( c_str, Op_union, Value_empty_list );
	
	static const Value c_str_array( c_str, Op_subscript, empty_list );
	
	static const Value exec( c_str, c_str_array );
	
	#define RUNOUT    "output-from-run"
	
	const proc_info proc_EXECV  = { "execv",  &v_execv,  &exec       };
	const proc_info proc_EXECVP = { "execvp", &v_execvp, &exec       };
	const proc_info proc_exit   = { "exit",   &v_exit,   &u8         };
	const proc_info proc_getenv = { "getenv", &v_getenv, &c_str      };
	const proc_info proc_getpid = { "getpid", &v_getpid, &empty_list };
	const proc_info proc_print  = { "print",  &v_print,  NULL        };
	const proc_info proc_RUN    = { "run",    &v_run,    &c_str_array};
	const proc_info proc_RUNOUT = { RUNOUT,   &v_runout, &c_str_array};
	const proc_info proc_setenv = { "setenv", &v_setenv, &cstr_x2    };
	const proc_info proc_sleep  = { "sleep",  &v_sleep,  &sleep_arg  };
	const proc_info proc_system = { "system", &v_system, &empty_list };
	const proc_info proc_SYSTEM = { "system", &v_SYSTEM, &maybe_cstr };
	const proc_info proc_time   = { "time",   &v_time,   &empty_list };
	const proc_info proc_utime  = { "utime",  &v_utime,  &empty_list };
	const proc_info proc_warn   = { "warn",   &v_warn,   NULL        };
	
}
}
