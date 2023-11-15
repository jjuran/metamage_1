/*
	load.hh
	-------
*/

#ifndef IOUTILS_LOAD_HH
#define IOUTILS_LOAD_HH

// Standard C
#include <stdint.h>


namespace v68k    {
namespace ioutils {

#ifdef __MC68K__

void* load( const char* path : __A0 ) : __A0;

#endif

void* load_file( const char* path, uint32_t* size = 0 );  // NULL

}  // namespace ioutils
}  // namespace v68k


#endif
