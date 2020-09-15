/*
	ops.cc
	------
*/

#include "v68k/ops.hh"

// v68k
#include "v68k/op_params.hh"


#pragma exceptions off


namespace v68k
{
	
	op_result sign_extend_byte( processor_state& s, op_params& pb )
	{
		pb.result = int8_t( pb.second );
		
		return Ok;
	}
	
	op_result sign_extend_word( processor_state& s, op_params& pb )
	{
		pb.result = int16_t( pb.second );
		
		return Ok;
	}
	
}
