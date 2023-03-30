/*
	cursor.hh
	---------
*/

#ifndef CURSOR_CURSOR_HH
#define CURSOR_CURSOR_HH

// Standard C
#include <stdint.h>


namespace v68k   {
namespace cursor {

struct shared_cursor_state
{
	uint16_t face[ 16 ];
	uint16_t mask[ 16 ];
	
	uint16_t hotspot[ 2 ];
	uint16_t pointer[ 2 ];
	
	uint16_t visible;
	uint16_t reserved1;
	uint16_t reserved2;
	uint16_t seed;
};

}  // namespace cursor
}  // namespace v68k


#endif
