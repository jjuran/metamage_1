/*
	startup.hh
	----------
*/

#ifndef VLIB_STARTUP_HH
#define VLIB_STARTUP_HH


namespace vlib
{
	
	class lexical_scope;
	
	void inject_startup_header( lexical_scope* globals );
	
}

#endif
