/*
	VIA.hh
	------
*/

#ifndef VIA_VIA_HH
#define VIA_VIA_HH

// v68k
#include "v68k/memory.hh"


namespace v68k {
namespace VIA  {

using v68k::addr_t;
using v68k::fc_t;
using v68k::mem_t;

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access );

}  // namespace VIA
}  // namespace v68k


#endif
