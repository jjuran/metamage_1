/*
	must/write.h
	------------
*/

#ifndef MUST_WRITE_H
#define MUST_WRITE_H

// POSIX
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


ssize_t must_write( int fd, const void* buffer, size_t n );


#ifdef __cplusplus
}
#endif


#endif

