/*
	FrameDeltas.hh
	--------------
*/

// Standard C
#include <stdint.h>

void save_frame_delta( int i, uint8_t* dst, const uint8_t* src );
void load_frame_delta( int i, uint8_t* dst,       uint8_t* tmp );
