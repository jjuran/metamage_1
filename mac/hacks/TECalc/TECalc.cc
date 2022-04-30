/*
	
	TECalc
	
	Joshua Juran
	
*/

// Mac OS
#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
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

// mac-sys-utils
#include "mac_sys/trap_address.hh"


#pragma exceptions off


extern long expression( const char* expr );


static UniversalProcPtr old_TEKey;

static
Boolean TEKey_handler( char c : __D0, TEHandle hTE : __A0 )
{
	int start = hTE[0]->selStart;
	int end   = hTE[0]->selEnd;
	
	if ( c == '='  &&  start != end )
	{
		int len = end - start;
		
		if ( len > 255 )
		{
			::SysBeep( 30 );
			
			return true;
		}
		
		char buf[256];
		
		BlockMoveData( *hTE[0]->hText, buf, len );
		buf[ len ] = '\0';
		
		long value = expression( buf );
		
		::NumToString( (short) value, (unsigned char*) buf );
		
		::TEDelete( hTE );
		::TEInsert( buf + 1, buf[0], hTE );
		::TESetSelect( start, start + buf[0], hTE );
		
		return true;
	}
	
	return false;
}

static
pascal
asm
void TEKey_patch( short c, TEHandle hTE )
{
	LINK     A6,#0
	
	MOVE.W   12(A6),D0
	MOVE.L   8(A6),A0
	JSR      TEKey_handler
	
	UNLK     A6
	
	TST.B    D0
	MOVEA.L  old_TEKey,A0
	BEQ.S    pass
	
	MOVEA.L  (SP)+,A0  // pop return address
	ADDQ.L   #6,SP     // pop arguments
	
pass:
	JMP      (A0)
}

int main()
{
	Handle self = Get1Resource( 'INIT', 0 );
	
	DetachResource( self );
	
	old_TEKey = mac::sys::get_trap_address( _TEKey );
	
	mac::sys::set_trap_address( (ProcPtr) TEKey_patch, _TEKey );
	
	return 0;
}
