/*
	utils.hh
	--------
*/

#ifndef UTILS_LOAD_HH
#define UTILS_LOAD_HH

// Standard C
#include <stdint.h>


namespace v68k  {
namespace utils {

void* load_file( const char* path, uint32_t* size = 0 );  // NULL

}  // namespace utils
}  // namespace v68k


#endif

