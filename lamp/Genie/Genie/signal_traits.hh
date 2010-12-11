/*
	signal_traits.hh
	----------------
*/

#ifndef GENIE_SIGNALTRAITS_HH
#define GENIE_SIGNALTRAITS_HH


namespace Genie
{
	
	enum signal_traits
	{
		signal_discard   = 0x00,
		signal_terminate = 0x01,
		signal_stop      = 0x02,
		signal_continue  = 0x03,
		
		signal_default_action_mask = 0x03,
		
		signal_immutable = 0x40,
		
		signal_core      = 0x80,
		
		signal_terminate_core = signal_terminate | signal_core,
		
		signal_sigkill = signal_terminate | signal_immutable,
		signal_sigstop = signal_stop      | signal_immutable,
		signal_null    = signal_discard   | signal_immutable
	};
	
	extern const signal_traits global_signal_traits[];
	
}

#endif

