/*
	graphics.hh
	-----------
*/

#ifndef NYANCAT_GRAPHICS_HH
#define NYANCAT_GRAPHICS_HH

namespace nyancat {

class bitmap;

const int n_frames = 12;

void draw_frame( bitmap& bits, int t );

}  // namespace nyancat

#endif
