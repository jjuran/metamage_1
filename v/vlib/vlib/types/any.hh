/*
	any.hh
	------
*/

#ifndef VLIB_TYPES_ANY_HH
#define VLIB_TYPES_ANY_HH


namespace vlib
{
	
	class Value;
	struct type_info;
	
	extern const type_info etc_vtype;
	
	bool is_etc( const Value& type );
	
}

#endif
