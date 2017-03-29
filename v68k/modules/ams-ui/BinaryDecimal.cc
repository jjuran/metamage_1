/*
	BinaryDecimal.cc
	----------------
*/

#include "BinaryDecimal.hh"

// Mac OS
#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// Standard C
#include <string.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// ams-common
#include "callouts.hh"


static
StringPtr NumToString_method( long x : __D0, StringPtr p : __A0 )
{
	const char* s = gear::inscribe_decimal( x );
	
	const int len = strlen( s );
	
	StringPtr q = p;
	
	*p++ = len;
	
	fast_memcpy( p, s, len );
	
	return q;
}

static
long StringToNum_method( ConstStr255Param string : __A0 )
{
	char buffer[ 256 ];
	
	short len = string[ 0 ];
	
	fast_memcpy( buffer, string + 1, len );
	
	buffer[ len ] = '\0';
	
	return gear::parse_decimal( buffer );
}

asm void Pack7_patch( short method )
{
	MOVEA.L  (SP)+,A1
	MOVE.W   (SP)+,D1
	MOVE.L   A1,-(SP)
	
	TST.W    D1
	BEQ      dispatch_NumToString
	
	CMPI.W   #0x0001,D1
	BEQ      dispatch_StringToNum
	
	_Debugger
	_ExitToShell
	
dispatch_NumToString:
	JMP      NumToString_method
	
dispatch_StringToNum:
	JMP      StringToNum_method
}
