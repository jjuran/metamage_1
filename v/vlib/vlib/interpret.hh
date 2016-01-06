/*
	interpret.hh
	------------
*/

#ifndef VLIB_INTERPRET_HH
#define VLIB_INTERPRET_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value interpret( const char* program, const char* file = 0 );  // NULL
	
}

#endif
