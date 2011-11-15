/*
	update_CCR.cc
	-------------
*/

#include "v68k/update_CCR.hh"

// v68k
#include "v68k/op_params.hh"
#include "v68k/state.hh"


namespace v68k
{
	
	static void update_CCR_BTST( processor_state& s, const op_params& pb )
	{
		const uint32_t bit = pb.first;
		
		s.regs.nzvc &= ~0x4;
		s.regs.nzvc |= (~pb.second >> bit & 0x1) << 2;
	}
	
	static void spare_CCR( processor_state& s, const op_params& pb )
	{
	}
	
	
	CCR_updater the_CCR_updaters[] =
	{
		&spare_CCR,
		&spare_CCR,
		&spare_CCR,
		&update_CCR_BTST
	};
	
}

