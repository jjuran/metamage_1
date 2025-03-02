/*
	update.hh
	---------
*/

#ifndef SCREEN_UPDATE_HH
#define SCREEN_UPDATE_HH


namespace raster
{
	
	struct sync_relay;
	
}

namespace v68k   {
namespace screen {

extern raster::sync_relay* the_sync_relay;

void update_bits();

}  // namespace screen
}  // namespace v68k


#endif
