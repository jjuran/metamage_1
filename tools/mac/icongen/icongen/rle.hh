/*
	rle.hh
	------
*/

#ifndef ICONGEN_RLE_HH
#define ICONGEN_RLE_HH

// Standard C
#include <stdint.h>

uint8_t* rle_encode( uint8_t* dst, const uint8_t* src, const uint8_t* src_end );


#endif
