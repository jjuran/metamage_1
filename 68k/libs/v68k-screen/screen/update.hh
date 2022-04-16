/*
	update.hh
	---------
*/

#ifndef SCREENUPDATE_HH
#define SCREENUPDATE_HH


namespace raster
{
	
	struct sync_relay;
	
}

namespace v68k   {
namespace screen {

extern raster::sync_relay* the_sync_relay;

void update();


}  // namespace screen
}  // namespace v68k


#endif
