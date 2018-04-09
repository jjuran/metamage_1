/*
	signals.cc
	----------
*/

#include "varyx/posix/signals.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>

// vlib
#include "vlib/types/integer.hh"
#include "vlib/types/type.hh"

// varyx-posix
#include "varyx/posix/exception.hh"


namespace varyx
{
namespace posix
{
	
	using namespace vlib;
	
	
	typedef void (*signal_handler)(int);
	
	static
	Value signal_set_handler( const Value& v, signal_handler handler )
	{
		const int signo = integer_cast< int >( v );
		
		signal_handler prev = signal( signo, handler );
		
		if ( prev == SIG_ERR )
		{
			throw_exception_object( error_desc( errno ) );
		}
		
		return nothing;
	}
	
	static
	Value v_sig_dfl( const Value& v )
	{
		return signal_set_handler( v, SIG_DFL );
	}
	
	static
	Value v_sig_ign( const Value& v )
	{
		return signal_set_handler( v, SIG_IGN );
	}
	
	static const Type integer = integer_vtype;
	
	const proc_info proc_sig_dfl = { "signal-default", &v_sig_dfl, &integer };
	const proc_info proc_sig_ign = { "signal-ignore",  &v_sig_ign, &integer };
	
}
}
