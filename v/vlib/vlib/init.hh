/*
	init.hh
	-------
*/

#ifndef VLIB_INIT_HH
#define VLIB_INIT_HH


namespace vlib
{
	
	struct proc_info;
	
	void define( const proc_info& proc );
	
	bool install_basic_symbols();
	
}

#endif
