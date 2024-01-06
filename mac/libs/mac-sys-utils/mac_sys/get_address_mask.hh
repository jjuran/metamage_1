/*
	get_address_mask.hh
	-------------------
*/

#ifndef MACSYS_GETADDRESSMASK_HH
#define MACSYS_GETADDRESSMASK_HH

#ifdef __MC68K__
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#endif


namespace mac {
namespace sys {

#define CPUFlag     (*(unsigned short*) 0x012f)
#define SysVersion  (*(unsigned short*) 0x015a)
#define Lo3Bytes    (*(unsigned long *) 0x031a)

inline
unsigned long get_address_mask()
{
#ifdef __MC68K__
	
	if ( CPUFlag < 2  ||  SysVersion < 0x0604 )
	{
		return Lo3Bytes;
	}
	
	return (unsigned long) StripAddress( (Ptr) -1 );
	
#endif
	
	return (unsigned long) -1;
}

#undef CPUFlag
#undef SysVersion
#undef Lo3Bytes

}
}

#endif
