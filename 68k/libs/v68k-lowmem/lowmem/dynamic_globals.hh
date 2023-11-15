/*
	dynamic_globals.hh
	------------------
*/

#ifndef LOWMEM_DYNAMICGLOBALS_HH
#define LOWMEM_DYNAMICGLOBALS_HH

// Standard C
#include <stdint.h>


namespace v68k   {
namespace lowmem {

uint32_t get_Ticks();
uint32_t get_Time();

}  // namespace lowmem
}  // namespace v68k


#endif
