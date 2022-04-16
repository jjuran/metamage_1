/*
	VIA.hh
	------
*/

#ifndef VIA_HH
#define VIA_HH

// v68k
#include "v68k/memory.hh"


namespace VIA {

using v68k::addr_t;
using v68k::fc_t;
using v68k::mem_t;

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access );

}


#endif
