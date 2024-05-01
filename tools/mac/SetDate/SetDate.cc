/*
	SetDate.cc
	----------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

// Standard C
#include <stdio.h>
#include <string.h>

// iota
#include "iota/char_types.hh"


static
int get_year( const char* arg, size_t len )
{
	if ( len != 4 )
	{
		return 0;
	}
	
	Byte a = (Byte) *arg++ - '0';
	Byte b = (Byte) *arg++ - '0';
	Byte c = (Byte) *arg++ - '0';
	Byte d = (Byte) *arg   - '0';
	
	int ok = a < 10
	       & b < 10
	       & c < 10
	       & d < 10;
	
	if ( ! ok )
	{
		return 0;
	}
	
	int year = ((a * 10 + b) * 10 + c) * 10 + d;
	
	/*
		Technically, the year /can/ be 2040, but only for a month or so.
		If you're still using 32-bit seconds-since-1904 in January 2040,
		you've got bigger problems than this utility not working...
	*/
	
	if ( year >= 1904  &&  year < 2040 )
	{
		return year;
	}
	
	return 0;
}

int main( int argc, char** argv )
{
	bool update = false;
	
	DateTimeRec d;
	
	GetTime( &d );
	
	if ( argc > 1 )
	{
		const char* arg = argv[ 1 ];
		
		size_t len = strlen( arg );
		
		if ( int year = get_year( arg, len ) )
		{
			d.year = year;
			
			update = true;
		}
		else
		{
			fprintf( stderr, "Invalid argument '%s'\n", arg );
			
			return 1;
		}
	}
	
	printf( "%.4d-%.2d-%.2d\n", d.year, d.month, d.day );
	
	if ( update )
	{
		unsigned long s;
		
		DateToSeconds( &d, &s );
		
		OSErr err = SetDateTime( s );
		
		if ( err )
		{
			fprintf( stderr, "SetDateTime: OSErr %d\n", err );
			
			return 1;
		}
	}
	
	return 0;
}
