/*	==========
 *	keymods.cc
 *	==========
 */

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <unistd.h>

// iota
#include "iota/strings.hh"


enum
{
	kKeyModifierBits = 32
};

struct string_constant
{
	const char*  data;
	unsigned     length;
};

#define _( s )  { STR_LEN( s "\n" ) }

const string_constant keyModifierNames[ kKeyModifierBits ] =
{
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	
	_( "command" ),
	_( "shift"   ),
	_( "alpha"   ),
	_( "option"  ),
	_( "control" ),
	_( "right-shift"   ),
	_( "right-option"  ),
	_( "right-control" ),
	
	_( "" ),
	_( "function" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" ),
	_( "" )
};

int main( int argc, char const *const argv[] )
{
	UInt32 keymods = ::GetCurrentKeyModifiers();
	
	for ( int i = 0;  i < kKeyModifierBits;  ++i )
	{
		const string_constant& name = keyModifierNames[i];
		
		if ( keymods & (1 << i) )
		{
			::write( STDOUT_FILENO, name.data, name.length );
		}
	}
	
	return 0;
}

