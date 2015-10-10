/*
	library.hh
	----------
*/

#ifndef LIBRARY_HH
#define LIBRARY_HH

// vlib
#include "vlib/proc_info.hh"


namespace vlib
{
	
	extern const proc_info proc_getenv;
	extern const proc_info proc_print;
	extern const proc_info proc_system;
	extern const proc_info proc_SYSTEM;  // unrestricted
	extern const proc_info proc_time;
	
}

#endif
