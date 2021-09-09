/*
	bitfields.hh
	------------
*/

#ifndef V68K_BITFIELDS_HH
#define V68K_BITFIELDS_HH

// v68k
#include "v68k/op.hh"
#include "v68k/op_params.hh"


namespace v68k
{
	
	struct processor_state;
	
	
	op_result normalize_bitfield_operands( processor_state& s, op_params& pb );
	
	op_result microcode_BFTST_Dn ( processor_state& state, op_params& pb );
	op_result microcode_BFCHG_Dn ( processor_state& state, op_params& pb );
	op_result microcode_BFCLR_Dn ( processor_state& state, op_params& pb );
	op_result microcode_BFSET_Dn ( processor_state& state, op_params& pb );
	op_result microcode_BFINS_Dn ( processor_state& state, op_params& pb );
	op_result microcode_BFFFO_Dn ( processor_state& state, op_params& pb );
	op_result microcode_BFEXTU_Dn( processor_state& state, op_params& pb );
	op_result microcode_BFEXTS_Dn( processor_state& state, op_params& pb );
	
	op_result microcode_BFTST ( processor_state& state, op_params& pb );
	op_result microcode_BFCHG ( processor_state& state, op_params& pb );
	op_result microcode_BFCLR ( processor_state& state, op_params& pb );
	op_result microcode_BFSET ( processor_state& state, op_params& pb );
	op_result microcode_BFINS ( processor_state& state, op_params& pb );
	op_result microcode_BFFFO ( processor_state& state, op_params& pb );
	op_result microcode_BFEXTU( processor_state& state, op_params& pb );
	op_result microcode_BFEXTS( processor_state& state, op_params& pb );
	
}

#endif
