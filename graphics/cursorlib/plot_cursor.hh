/*
	plot_cursor.hh
	--------------
*/

#ifndef PLOTCURSOR_HH
#define PLOTCURSOR_HH

// Standard C
#include <stdint.h>


void plot_cursor( const uint16_t*  crsr_face,
                  const uint16_t*  crsr_mask,
                  uint8_t*         addr,
                  short            shift,
                  short            h_trim,
                  short            v_skip,
                  short            n,
                  short            stride );

#endif
