/*
	virtualization.cc
	-----------------
*/

#include "mac_sys/has/virtualization.hh"

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/get_machine_name.hh"


namespace mac {
namespace sys {

#ifdef __MC68K__

typedef unsigned long uint32_t;

asm unsigned long core_signature()
{
	MOVE     #0,CCR
	MOVEQ.L  #0,D0
	MOVEQ.L  #7,D1
	
loop:
	CHK.W    D1,D1
	MOVE     SR,D2
	OR.B     D2,D0
	ROR.L    #4,D0
	DBRA     D1,loop
	
	RTS
}

#else

unsigned long core_signature()
{
	return 0;
}

#endif

bool has_MinivMac()
{
#ifndef __MACH__
#ifndef __POWERPC__
	
	typedef unsigned long uint32_t;
	
	struct SonyVars_record
	{
		uint32_t zeros[ 3 ];
		uint32_t reserved;
		uint32_t magic;
		uint32_t address;
	};
	
	SonyVars_record* SonyVars = *(SonyVars_record**) 0x0134;
	
	if ( SonyVars  &&  ((uint32_t) SonyVars & 1) == 0 )
	{
		if ( is_driver_installed( "\p" ".Sony" ) )
		{
			const uint32_t magic = 0x841339E2;
			
			const uint32_t* p = SonyVars->zeros;
			
			uint32_t zero = 0;
			
			zero |= *p++;
			zero |= *p++;
			zero |= *p++;
			
			return zero == 0  &&  SonyVars->magic == magic;
		}
	}
	
#endif
#endif
	
	return false;
}

bool has_PCE()
{
#ifdef __MC68K__
	
	const uint32_t sign_addr = 0xf80000;
	const uint32_t sign_data = 'PCEX';
	
	void* SonyVars = *(void**) 0x0134;
	
	if ( SonyVars  &&  is_driver_installed( "\p" ".Sony" ) )
	{
		return *(uint32_t*) sign_addr == sign_data;
	}
	
#endif
	
	return false;
}

bool has_MacOnLinux_MacOSX()
{
	const unsigned char* name = "\p" "AAPL,MOL";
	const unsigned char* mnam = get_machine_name();
	
	return mnam  &&  memcmp( mnam, name, sizeof "AAPL,MOL" ) == 0;
}

}
}
