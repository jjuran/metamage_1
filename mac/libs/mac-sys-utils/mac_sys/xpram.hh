/*
	xpram.hh
	--------
*/

#ifndef MACSYS_XPRAM_HH
#define MACSYS_XPRAM_HH


typedef unsigned short UInt16;

namespace mac {
namespace sys {
	
	void ReadXPRam( void* buffer, UInt16 length, UInt16 offset );
	
}
}

#endif
