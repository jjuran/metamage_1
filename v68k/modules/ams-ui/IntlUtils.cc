/*
	IntlUtils.cc
	------------
*/

#include "IntlUtils.hh"

// Mac OS
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"


static
pascal short IUMagString_call( char* a, char* b, short an, short bn )
{
	typedef unsigned char uint8_t;
	
	return RelString( (uint8_t*) a, (uint8_t*) b, an, bn );
}

static
pascal short IUMagIDString_call( char* a, char* b, short an, short bn )
{
	typedef unsigned char uint8_t;
	
	return CmpString_sans_case( (uint8_t*) a, an, (uint8_t*) b, bn );
}

static
void unimplemented_call( short selector : __D0 )
{
	FATAL = "unimplemented Pack6 call ", selector;
	
	asm { ILLEGAL }
}

asm void Pack6_patch( short selector )
{
	MOVEA.L  (SP)+,A0
	MOVE.W   (SP)+,D0
	MOVE.L   A0,-(SP)
	
	CMPI.W   #0x000A,D0
	BEQ      dispatch_IUMagString
	
	CMPI.W   #0x000C,D0
	BEQ      dispatch_IUMagIDString
	
	JMP      unimplemented_call
	
dispatch_IUMagString:
	JMP      IUMagString_call
	
dispatch_IUMagIDString:
	JMP      IUMagIDString_call
}
