/*
	types.hh
	--------
*/

#ifndef VLIB_TYPES_HH
#define VLIB_TYPES_HH


namespace vlib
{
	
	class Value;
	struct proc_info;
	struct type_info;
	
	extern const type_info c_str_vtype;
	extern const type_info function_vtype;
	
	extern const proc_info proc_typeof;
	
	extern Value generic_array_type;
	
	bool is_functional( const Value& v );
	
}

#endif
