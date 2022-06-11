/*
	stdint.hh
	---------
*/

#ifndef VLIB_TYPES_ENDEC_STDINT_HH
#define VLIB_TYPES_ENDEC_STDINT_HH


namespace vlib
{
	
	struct proc_info;
	
	extern const proc_info proc_encode_i8;
	extern const proc_info proc_decode_i8;
	
	extern const proc_info proc_encode_i16;
	extern const proc_info proc_decode_i16;
	
	extern const proc_info proc_encode_i32;
	extern const proc_info proc_decode_i32;
	
	extern const proc_info proc_encode_i64;
	extern const proc_info proc_decode_i64;
	
	extern const proc_info proc_encode_u8;
	extern const proc_info proc_decode_u8;
	
	extern const proc_info proc_encode_u16;
	extern const proc_info proc_decode_u16;
	
	extern const proc_info proc_encode_u32;
	extern const proc_info proc_decode_u32;
	
	extern const proc_info proc_encode_u64;
	extern const proc_info proc_decode_u64;
	
}

#endif
