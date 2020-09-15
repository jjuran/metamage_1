/*
	memory.hh
	---------
*/

#ifndef CALLOUTMEMORY_HH
#define CALLOUTMEMORY_HH

// v68k
#include "v68k/memory.hh"


namespace v68k    {
namespace callout {

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access );

}  // namespace callout
}  // namespace v68k


#endif
