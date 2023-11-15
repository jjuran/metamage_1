/*
	surface.hh
	----------
*/

#ifndef SCREEN_SURFACE_HH
#define SCREEN_SURFACE_HH


namespace v68k   {
namespace screen {

struct surface_shape
{
	unsigned width;
	unsigned height;
	unsigned stride;
};

extern surface_shape the_surface_shape;

}  // namespace screen
}  // namespace v68k


#endif
