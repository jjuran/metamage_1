/*
	more/string.h
	-------------
*/

#ifndef MORE_STRING_H
#define MORE_STRING_H

#ifdef __linux__
#include <string.h>
#endif

// more-libc
#include "more/size.h"

#ifdef __cplusplus
extern "C" {
#endif


/* GNU extensions */

#if ! defined( __linux__ )  ||  defined( ANDROID )
#ifdef __MC68K__

void* mempcpy( void* dest : __A0, const void* src : __A1, size_t n : __D0 );

#else

void* mempcpy( void* dest, const void* src, size_t n );

#endif
#endif  // #if ! defined( __linux__ )  ||  defined( ANDROID )


#ifdef __cplusplus
}
#endif

#endif
