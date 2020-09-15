/*
	surface.hh
	----------
*/

#ifndef SCREENSURFACE_HH
#define SCREENSURFACE_HH


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
