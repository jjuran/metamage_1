/*
	update_CCR.cc
	-------------
*/

#include "v68k/update_CCR.hh"


namespace v68k
{
	
	static void spare_CCR( processor_state& s, const op_params& pb )
	{
	}
	
	
	CCR_updater the_CCR_updaters[] =
	{
		&spare_CCR,
		&spare_CCR,
		&spare_CCR,
		&spare_CCR
	};
	
}

