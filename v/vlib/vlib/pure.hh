/*
	pure.hh
	-------
*/

#ifndef VLIB_PURE_HH
#define VLIB_PURE_HH


namespace vlib
{
	
	struct proc_info;
	class Value;
	
	bool is_pure( const Value& v );
	
	extern const proc_info proc_is_pure;
	
}

#endif
