/*
	Geometry.hh
	-----------
*/

#ifndef GEOMETRY_HH
#define GEOMETRY_HH

enum
{
	zoom = 4,
	
	nyan_width  = zoom * 70,  // 280
	nyan_height = zoom * 72,  // 288
	
	nyan_stride = (nyan_width + 15) / 16 * 2,  // 36
	
	frame_size = nyan_stride * nyan_height,  // 10368
};

#endif
