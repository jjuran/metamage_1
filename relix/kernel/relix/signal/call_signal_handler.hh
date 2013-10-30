/*
	call_signal_handler.hh
	----------------------
*/

#ifndef RELIX_SIGNAL_CALLSIGNALHANDLER_HH
#define RELIX_SIGNAL_CALLSIGNALHANDLER_HH


namespace relix
{
	
	struct caught_signal;
	
	void call_signal_handler( const caught_signal& signal );
	
}

#endif

