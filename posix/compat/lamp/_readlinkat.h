/*
	_readlinkat.h
	-------------
	
	Joshua Juran
*/

#ifndef LAMP_READLINKAT_H
#define LAMP_READLINKAT_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t _readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size, int flags );

#ifdef __cplusplus
}
#endif

#endif

