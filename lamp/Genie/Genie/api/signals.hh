/*
	signals.hh
	----------
*/

#ifndef GENIE_API_SIGNALS_HH
#define GENIE_API_SIGNALS_HH


namespace Genie
{
	
	bool check_signals( bool may_throw );
	
	void send_signal_to_current_process( int signo );
	
}

#endif

