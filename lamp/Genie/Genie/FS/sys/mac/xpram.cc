/*
	Genie/FS/sys/mac/xpram.cc
	-------------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/xpram.hh"

// Mac OS
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif


#if TARGET_CPU_68K

static asm void ReadXPRam( void* buffer, UInt16 length, UInt16 offset )
{
	MOVEA.L	4(SP),A0  ;  // load buffer address
	MOVE.L	8(SP),D0  ;  // load combined length/offset
	
	_ReadXPRam
	
	RTS
}

#else

static const short ReadXPRam_68k_code[] =
{
	0x206f, 0x0004,  // MOVEA.L 4(SP),A0
	0x202f, 0x0008,  // MOVE.L 8(SP),D0
	_ReadXPRam,
	0x4e75           // RTS
};

static const UInt32 kReadXPRamProcInfo = kCStackBased
                                       | STACK_ROUTINE_PARAMETER( 1, SIZE_CODE( sizeof (void*)     ) )
                                       | STACK_ROUTINE_PARAMETER( 2, SIZE_CODE( sizeof (short) * 2 ) );

static UniversalProcPtr gReadXPRamUPP = NewRoutineDescriptor( (ProcPtr) ReadXPRam_68k_code,
                                                              kReadXPRamProcInfo,
                                                              kM68kISA );

inline void ReadXPRam( void* buffer, UInt16 length, UInt16 offset )
{
	CallUniversalProc( gReadXPRamUPP, kReadXPRamProcInfo, buffer, (length << 16) | offset );
}

#endif

namespace Genie
{
	
	plus::string sys_mac_xpram::Read( const vfs::node* parent, const plus::string& name )
	{
		const UInt16 length = 256;
		const UInt16 offset = 0;
		
		plus::string xpram;
		
		char* p = xpram.reset( length );
		
		::ReadXPRam( p, length, offset );
		
		return xpram;
	}
	
}

#endif
