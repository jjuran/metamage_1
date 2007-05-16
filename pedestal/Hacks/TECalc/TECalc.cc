/*
	
	TECalc
	
	Joshua Juran
	
*/

// Universal Interfaces
#include <Events.h>
#include <NumberFormatting.h>
#include <Sound.h>
#include <TextEdit.h>
#include <Traps.h>

// Standard C/C++
#include <cstring>

// Silver
#include "Silver/Patches.hh"


namespace Ag = Silver;


extern long expression( const char* expr );


namespace
{
	
	void PatchedTEKey( short c, TEHandle hTE, Ag::TEKeyProcPtr nextHandler )
	{
		int start = hTE[0]->selStart;
		int end   = hTE[0]->selEnd;
		
		if ( c == '='  &&  start != end )
		{
			int len = end - start;
			
			if ( len > 255 )
			{
				::SysBeep( 30 );
				
				return;
			}
			
			char buf[256];
			
			std::memcpy( buf, *hTE[0]->hText, len );
			buf[ len ] = '\0';
			
			long value = expression( buf );
			
			::NumToString( (short) value, (unsigned char*) buf );
			
			::TEDelete( hTE );
			::TEInsert( buf + 1, buf[0], hTE );
			::TESetSelect( start, start + buf[0], hTE );
		}
		else
		{
			nextHandler( c, hTE );
		}
	}
	
}

static bool Install()
{
	bool locked = Ag::LoadAndLock();
	
	if ( !locked )
	{
		return false;
	}
	
	Ag::MyA4 a4;
	
	//gTEKeyPatch = TEKeyPatch(PatchedTEKey);
	Ag::TrapPatch< _TEKey, PatchedTEKey >::Install();
	
	return true;
}

void main()
{
	bool installed = Install();
}

