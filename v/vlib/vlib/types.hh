/*
	types.hh
	--------
*/

#ifndef VLIB_TYPES_HH
#define VLIB_TYPES_HH


namespace vlib
{
	
	struct type_info;
	
	extern const type_info type_vtype;
	extern const type_info string_vtype;
	extern const type_info boolean_vtype;
	extern const type_info integer_vtype;
	extern const type_info function_vtype;
	
}

#endif
