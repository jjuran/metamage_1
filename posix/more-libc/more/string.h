/*
	more/string.h
	-------------
*/

#ifndef MORE_STRING_H
#define MORE_STRING_H

// iota
#include "iota/size_t.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GNU extensions */
void* mempcpy( void* dest, const void* src, size_t n );

#ifdef __cplusplus
}
#endif

#endif

