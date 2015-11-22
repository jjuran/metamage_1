/*
	types.hh
	--------
*/

#ifndef VLIB_TYPES_HH
#define VLIB_TYPES_HH


namespace vlib
{
	
	struct proc_info;
	struct type_info;
	
	extern const type_info i8_vtype;
	extern const type_info u8_vtype;
	extern const type_info i16_vtype;
	extern const type_info u16_vtype;
	extern const type_info i32_vtype;
	extern const type_info u32_vtype;
	extern const type_info i64_vtype;
	extern const type_info u64_vtype;
	extern const type_info type_vtype;
	extern const type_info string_vtype;
	extern const type_info boolean_vtype;
	extern const type_info integer_vtype;
	extern const type_info function_vtype;
	
	extern const proc_info proc_join;
	extern const proc_info proc_typeof;
	
}

#endif
