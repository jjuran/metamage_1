/*
	FrameDeltas.hh
	--------------
*/

// Standard C
#include <stdint.h>

typedef unsigned char Byte;

void save_frame_delta( int i, Byte* dst, const Byte* src );
void load_frame_delta( int i, Byte* dst,       Byte* tmp );
