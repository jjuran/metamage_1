/*
	shared_memory.hh
	----------------
*/

#ifndef SCREEN_SHAREDMEMORY_HH
#define SCREEN_SHAREDMEMORY_HH


namespace v68k   {
namespace screen {

void* open_shared_memory( const char* path );

}  // namespace screen
}  // namespace v68k

#endif
