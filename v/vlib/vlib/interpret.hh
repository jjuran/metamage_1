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
	
	class lexical_scope;
	
	Value interpret( const char*     program,
	                 const char*     file    = 0,
	                 lexical_scope*  globals = 0 );  // NULL
	
}

#endif
