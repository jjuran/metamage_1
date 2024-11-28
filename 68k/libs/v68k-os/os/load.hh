/*
	load.hh
	-------
*/

#ifndef OS_LOAD_HH
#define OS_LOAD_HH

// Standard C
#include <stdint.h>


namespace v68k {
namespace os   {

void load( uint8_t* mem, uint32_t mem_size );

}  // namespace os
}  // namespace v68k

#endif
