/*
	printable.hh
	------------
*/

#ifndef PLUS_PRINTABLE_HH
#define PLUS_PRINTABLE_HH

// plus
#include "plus/string.hh"


namespace plus
{
	
	inline const string& printable( const string& s )
	{
		return s;
	}
	
	inline const char* printable( const char* s )
	{
		return s;
	}
	
	string printable( const void* x );
	
	string printable( unsigned long long x );
	string printable(          long long x );
	
	inline string printable( unsigned long x )
	{
		unsigned long long xx = x;
		
		return printable( xx );
	}
	
	inline string printable( long x )
	{
		long long xx = x;
		
		return printable( xx );
	}
	
	inline string printable( unsigned int x )
	{
		unsigned long long xx = x;
		
		return printable( xx );
	}
	
	inline string printable( int x )
	{
		long long xx = x;
		
		return printable( xx );
	}
	
	inline const char* printable( bool b )
	{
		return b ? "true" : "false";
	}
	
}

#endif
