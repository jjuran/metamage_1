/*
	functions.hh
	------------
*/

#ifndef VLIB_FUNCTIONS_HH
#define VLIB_FUNCTIONS_HH

// plus
#include "plus/string.hh"

// vlib
#include "vlib/proc_info.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	void define( const proc_info& proc );
	
	bool install_basic_functions();
	
	extern const proc_info proc_area;
	extern const proc_info proc_rep;
	extern const proc_info proc_unbin;
	extern const proc_info proc_unhex;
	
}

#endif
