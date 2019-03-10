/*
	clock.hh
	--------
*/

#ifndef V68KTIME_CLOCK_HH
#define V68KTIME_CLOCK_HH

// Standard C
#include <stdint.h>


namespace v68k {
namespace time {

uint64_t guest_uptime_microseconds();

}  // namespace time
}  // namespace v68k

#endif
