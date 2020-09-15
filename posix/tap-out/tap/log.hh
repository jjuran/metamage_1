/*
	log.hh
	------
*/

#ifndef TAPOUT_LOG_HH
#define TAPOUT_LOG_HH

// iota
#include "iota/iterator.hh"


namespace tap
{
	
	void log( const void* p, unsigned p_len, const void* q, unsigned q_len );
	
	void log_expected( const iota::span& s );
	void log_received( const iota::span& s );
	
}

#endif
