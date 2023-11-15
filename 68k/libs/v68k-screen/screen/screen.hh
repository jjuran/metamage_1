/*
	screen.hh
	---------
*/

#ifndef SCREEN_SCREEN_HH
#define SCREEN_SCREEN_HH

// Standard C
#include <stdint.h>


namespace v68k   {
namespace screen {

extern uint8_t* page_1_virtual_buffer;
extern uint8_t* page_2_virtual_buffer;

extern uint8_t* page_1_transit_buffer;
extern uint8_t* page_2_transit_buffer;

void page_flip();

int set_screen_backing_store_file( const char* path );

}  // namespace screen
}  // namespace v68k

#endif
