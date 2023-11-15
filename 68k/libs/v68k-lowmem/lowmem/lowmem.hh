/*
	lowmem.hh
	---------
*/

#ifndef LOWMEM_LOWMEM_HH
#define LOWMEM_LOWMEM_HH

// v68k
#include "v68k/memory.hh"


namespace v68k   {
namespace lowmem {

extern bool ticking;

short get_CurPageOption();

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access );

}  // namespace lowmem
}  // namespace v68k


#endif
