/*
	SoundDispatch.cc
	----------------
*/

#include "SoundDispatch.hh"

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// log-of-war
#include "logofwar/report.hh"


static NumVersion version;

static
pascal
NumVersion SndSoundManagerVersion_handler()
{
	return version;
}

static
void unimplemented_API( short api : __D0, short call : __D1 )
{
	FATAL = "unimplemented SoundDispatch API: ", api, " (call ", call, ")";
	
	asm { ILLEGAL }
}

static
void unimplemented_call( short api : __D0, short call : __D1 )
{
	FATAL = "unimplemented SoundDispatch API ", api, " call: ", call;
	
	asm { ILLEGAL }
}

asm
void SoundDispatch_patch( long selector : __D0 )
{
	MOVE.L   D0,D1
	SWAP     D1
	
	CMPI.W   #0x0008,D0
	BEQ.S    dispatch_1_0
	
	JMP      unimplemented_API
	
dispatch_1_0:
	CMPI.W   #0x000C,D1
	BEQ.S    call_1_0_Version
	
	JMP      unimplemented_call
	
call_1_0_Version:
	JMP      SndSoundManagerVersion_handler
}
