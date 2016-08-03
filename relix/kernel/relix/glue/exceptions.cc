/*
	exceptions.cc
	-------------
*/

#include "relix/glue/exceptions.hh"

// Standard C
#include <signal.h>

// relix
#include "relix/api/deliver_fatal_signal.hh"


namespace relix
{
	
	void bus_error()
	{
		deliver_fatal_signal( SIGSEGV );
	}
	
	void address_error()
	{
		deliver_fatal_signal( SIGBUS );
	}
	
	void illegal_instruction()
	{
		deliver_fatal_signal( SIGILL );
	}
	
	void division_by_zero()
	{
		deliver_fatal_signal( SIGFPE );
	}
	
	void integer_range_check()
	{
		deliver_fatal_signal( SIGFPE );
	}
	
	void integer_overflow()
	{
		deliver_fatal_signal( SIGFPE );
	}
	
	void privilege_violation()
	{
		deliver_fatal_signal( SIGILL );
	}
	
}
