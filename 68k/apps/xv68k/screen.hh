/*
	screen.hh
	---------
*/

#ifndef SCREEN_HH
#define SCREEN_HH

// v68k
#include "v68k/memory.hh"


extern uint8_t* page_1_transit_buffer;
extern uint8_t* page_2_transit_buffer;

void page_flip();

int set_screen_backing_store_file( const char* path );

namespace screen {

using v68k::addr_t;
using v68k::fc_t;
using v68k::mem_t;

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access );
uint8_t* translate2( addr_t addr, uint32_t length, fc_t fc, mem_t access );

}


#endif
