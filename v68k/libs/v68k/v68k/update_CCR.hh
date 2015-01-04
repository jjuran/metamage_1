/*
	update_CCR.hh
	----------
*/

#ifndef V68K_UPDATECCR_HH
#define V68K_UPDATECCR_HH


namespace v68k
{
	
	struct processor_state;
	struct op_params;
	
	
	typedef void (*CCR_updater)( processor_state& s, const op_params& pb );
	
	extern CCR_updater the_CCR_updaters[];
	
}

#endif
