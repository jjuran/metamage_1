/*	==========
 *	keymods.cc
 *	==========
 */

// Universal Interfaces
#ifndef __EVENTS__
#include <Events.h>
#endif

// Standard C/C++
#include <cstdio>


#pragma export on

enum
{
	kKeyModifierBits = 32
};

const char* keyModifierNames[ kKeyModifierBits ] =
{
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	
	"command",
	"shift",
	"alpha",
	"option",
	"control",
	"right-shift",
	"right-option",
	"right-control",
	
	"",
	"function",
	"",
	"",
	"",
	"",
	"",
	"",
	
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};

int main( int argc, char const *const argv[] )
{
	UInt32 keymods = ::GetCurrentKeyModifiers();
	
	for ( int i = 0;  i < kKeyModifierBits;  ++i )
	{
		const char* name = keyModifierNames[i];
		
		if ( keymods & (1 << i) )
		{
			std::printf( "%s\n", name );
		}
	}
	
	return 0;
}

#pragma export reset

