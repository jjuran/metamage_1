/*
	buffers.hh
	----------
*/

#ifndef MEMORY_BUFFERS_HH
#define MEMORY_BUFFERS_HH

// v68k
#include "v68k/state.hh"


namespace v68k   {
namespace memory {

int place_buffers( const processor_state& emu, uint8_t* mem, uint32_t mem_top );

}  // namespace memory
}  // namespace v68k

#endif
