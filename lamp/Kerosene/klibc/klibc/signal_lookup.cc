/*
	signal_lookup.cc
	----------------
*/

#include "klibc/signal_lookup.hh"

// Standard C
#include <signal.h>
#include <string.h>


namespace klibc
{

struct signal_record
{
	const char*  name;
	int          number;
};

#define SIG(name) { #name, SIG##name }

static const signal_record signals[] =
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

static const unsigned n_signals = sizeof signals / sizeof signals[ 0 ] - 1;

int signal_lookup( const char* signame )
{
	const signal_record* end = signals + n_signals;
	
	const signal_record* it;
	
	for ( it = signals;  it < end;  ++it )
	{
		if ( strcmp( it->name, signame ) == 0 )
		{
			break;
		}
	}
	
	// The end-of-sequence iterator points to an element with number 0
	//if ( it == end )  return 0;
	
	return it->number;
}

}
