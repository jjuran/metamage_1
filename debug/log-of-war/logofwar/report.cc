/*
	report.cc
	---------
*/

#include "logofwar/report.hh"

// Standard C
#include <string.h>

// log-of-war
#include "logofwar/clock.hh"
#include "logofwar/print.hh"


namespace logofwar
{

bool verbose_reports;

#ifdef __MC68K__

static inline
asm
void split_3_digits( char* digits : __A0, unsigned x : __D0 )
{
	MOVE.L   #'000\0',(A0)  // trust that this is a word boundary
	
	DIVU.W   #10,D0         // x / 10, x % 10 (ones)
	MOVE.L   D0,D1
	EXT.L    D1
	DIVU.W   #10,D1         // x / 100 (hundreds), x / 10 % 10 (tens)
	
	ADD.B    D1,(A0)+       // hundreds
	SWAP     D1
	ADD.B    D1,(A0)+       // tens
	SWAP     D0
	ADD.B    D0,(A0)+       // ones
}

#else

static inline
void split_3_digits( char* digits, unsigned x )
{
	*digits++ = '0' + x / 100;
	*digits++ = '0' + x / 10 % 10;
	*digits++ = '0' + x      % 10;
	
	*digits = '\0';
}

#endif

void report( diagnostic_level level, const char* file, int line )
{
	if ( verbose_reports )
	{
		print( "\n" );
		print( file );
		print( ":" );
		print( line );
		print( "\n" );
	}
	
	unsigned long t = runclock();
	
	unsigned long ts = t / 1000;
	unsigned     tms = t % 1000;
	
	char ms_digits[ 4 ];
	
	split_3_digits( ms_digits, tms );
	
	const char* level_name = diagnostic_level_names[ level ];
	
	const char* space = &"     "[ strlen( level_name ) - 4 ];
	
	print( "T=" );
	print( ts );
	print( "." );
	print( ms_digits );
	print( "s  " );
	print( level_name );
	print( ":" );
	print( space );
}

}
