/*
	storage.cc
	----------
*/

#include "screen/storage.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k   {
namespace screen {

unsigned the_screen_size;

void* virtual_buffer;
void* transit_buffer;

}  // namespace screen
}  // namespace v68k
