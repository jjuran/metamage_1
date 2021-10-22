/*
	BinaryDecimal.cc
	----------------
*/

#include "BinaryDecimal.hh"

// Mac OS
#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif

// Standard C
#include <string.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"


static
StringPtr NumToString_call( long x : __D0, StringPtr p : __A0 )
{
	const char* s = gear::inscribe_decimal( x );
	
	const int len = strlen( s );
	
	StringPtr q = p;
	
	*p++ = len;
	
	fast_memcpy( p, s, len );
	
	return q;
}

static
long StringToNum_call( ConstStr255Param string : __A0 )
{
	return gear::parse_decimal( CSTR( string ) );
}

static
void unimplemented_call( short selector : __D1 )
{
	FATAL = "unimplemented Pack7 call ", selector;
	
	asm { ILLEGAL }
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
	
	JMP      unimplemented_call
	
dispatch_NumToString:
	JMP      NumToString_call
	
dispatch_StringToNum:
	JMP      StringToNum_call
}
