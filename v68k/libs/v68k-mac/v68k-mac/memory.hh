/*
	memory.hh
	---------
*/

#ifndef V68KMAC_MEMORY_HH
#define V68KMAC_MEMORY_HH

// v68k
#include "v68k/memory.hh"


namespace v68k {
namespace mac  {

extern bool ticking;

short get_CurPageOption();

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access );

}  // namespace mac
}  // namespace v68k


#endif
