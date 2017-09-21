/*
	dynamic_globals.hh
	------------------
*/

#ifndef V68KMAC_DYNAMICGLOBALS_HH
#define V68KMAC_DYNAMICGLOBALS_HH

// Standard C
#include <stdint.h>


namespace v68k {
namespace mac  {

uint32_t get_Ticks();
uint32_t get_Time();

}  // namespace mac
}  // namespace v68k


#endif
