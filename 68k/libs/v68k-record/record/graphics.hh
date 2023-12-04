/*
	graphics.hh
	-----------
*/

#ifndef RECORD_GRAPHICS_HH
#define RECORD_GRAPHICS_HH


namespace v68k   {
namespace record {

void screen_size( int width, int height, int stride, int weight );

void frame( const void* buffer );

}  // namespace record
}  // namespace v68k

#endif
