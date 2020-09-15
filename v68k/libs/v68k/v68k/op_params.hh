/*
	op_params.hh
	------------
*/

#ifndef V68K_OPPARAMS_HH
#define V68K_OPPARAMS_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	enum op_size_t
	{
		byte_sized = 1,
		word_sized = 2,
		long_sized = 3,
		
		max_actual_size = long_sized,
		
		op_size_in_0040 = 0x0040 | 6 | 1,  // MOVEM, MOVEP
		op_size_in_00C0 = 0x00C0 | 6 | 0,  // normal
		op_size_in_0100 = 0x0100 | 8 | 1,  // ADDA/SUBA
		
		op_size_shift_mask = 6|8,
		
		unsized = 0
	};
	
	struct op_params
	{
		op_size_t size;
		
		uint32_t target;
		uint32_t address;
		
		uint32_t first;
		uint32_t second;
		uint32_t result;
	};
	
}

#endif
