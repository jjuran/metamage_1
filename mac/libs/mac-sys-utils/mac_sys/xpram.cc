/*
	xpram.cc
	--------
*/

#include "mac_sys/xpram.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif


namespace mac {
namespace sys {

enum
{
	_ReadXPRam = 0xA051,
};

#if TARGET_CPU_68K

asm
void ReadXPRam( void* buffer, UInt16 length, UInt16 offset )
{
	MOVEA.L	4(SP),A0  ;  // load buffer address
	MOVE.L	8(SP),D0  ;  // load combined length/offset
	
	_ReadXPRam
	
	RTS
}

#elif ! TARGET_API_MAC_CARBON

static const short ReadXPRam_asm[] =
{
	0x206f, 0x0004,  // MOVEA.L 4(SP),A0
	0x202f, 0x0008,  // MOVE.L 8(SP),D0
	_ReadXPRam,
	0x4e75           // RTS
};

void ReadXPRam( void* buffer, UInt16 length, UInt16 offset )
{
	enum
	{
		procInfo = kCStackBased
		         | STACK_ROUTINE_PARAMETER( 1, SIZE_CODE( sizeof (void*)     ) )
		         | STACK_ROUTINE_PARAMETER( 2, SIZE_CODE( sizeof (short) * 2 ) )
	};
	
	static UniversalProcPtr upp = NewRoutineDescriptor( (ProcPtr) ReadXPRam_asm,
	                                                    procInfo,
	                                                    kM68kISA );
	
	CallUniversalProc( upp, procInfo, buffer, (length << 16) | offset );
}

#else

int dummy;

#endif

}
}
