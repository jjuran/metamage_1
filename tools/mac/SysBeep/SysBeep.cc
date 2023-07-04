/*
	SysBeep.cc
	----------
*/

// Mac OS
#include <ConditionalMacros.h>

// <Sound.h>
extern "C" pascal void SysBeep( short ticks )  ONEWORDINLINE( 0xA9C8 );

// gear
#include "gear/parse_decimal.hh"


int main( int argc, char** argv )
{
	int ticks = argv[1] ? gear::parse_unsigned_decimal( argv[1] )
	                    : 30;
	
	SysBeep( ticks );
	
	return 0;
}
