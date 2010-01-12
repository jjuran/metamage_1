/*
	
	TECalc
	
	Joshua Juran
	
*/

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// Standard C/C++
#include <cstring>

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


#pragma exceptions off


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

static OSErr Installer()
{
	Ag::TrapPatch< _TEKey, PatchedTEKey >::Install();
	
	return noErr;
}

int main()
{
	return Ag::Install( Installer );
}

