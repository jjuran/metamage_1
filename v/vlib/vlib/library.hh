/*
	library.hh
	----------
*/

#ifndef VLIB_LIBRARY_HH
#define VLIB_LIBRARY_HH

// vlib
#include "vlib/proc_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	extern const proc_info proc_getenv;
	extern const proc_info proc_print;
	extern const proc_info proc_time;
	
}

#endif
