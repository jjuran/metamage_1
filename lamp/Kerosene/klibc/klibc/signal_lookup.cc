/*
	signal_lookup.cc
	----------------
	
	Joshua Juran
*/

#include "klibc/signal_lookup.hh"

// Standard C
#include <signal.h>

// Standard C++
#include <algorithm>
#include <cstring>


namespace klibc
{
	
	struct signal_record
	{
		const char*  name;
		int          number;
	};
	
	#define SIG(name) { #name, SIG##name }
	
	const signal_record gSignals[] =
	{
		// These need to be alphabetized
		SIG( ALRM ),
		SIG( CONT ),
		SIG( HUP  ),
		SIG( INT  ),
		SIG( KILL ),
		SIG( QUIT ),
		SIG( STOP ),
		SIG( TERM ),
		
		{ "~", 0 }  // The end of the sequence points *to* this element, not after.
	};
	
	static const unsigned gCountOfSignals = sizeof gSignals / sizeof (signal_record) - 1;
	
	static bool eq_strings( const char* a, const char* b )
	{
		return std::strcmp( a, b ) == 0;
	}
	
	static bool compare_signal_name( const signal_record& sig, const char* name )
	{
		return eq_strings( sig.name, name );
	}
	
	static bool operator==( const signal_record& sig, const char* name )
	{
		return compare_signal_name( sig, name );
	}
	
	int signal_lookup( const char* signame )
	{
		const signal_record* end = gSignals + gCountOfSignals;
		
		const signal_record* it = std::find( gSignals, end, signame );
		
		// The end-of-sequence iterator points to an element with number 0
		//if ( it == end )  return 0;
		
		return it->number;
	}
	
}

