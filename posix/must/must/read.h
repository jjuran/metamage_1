/*
	must/read.h
	-----------
*/

#ifndef MUST_READ_H
#define MUST_READ_H

// POSIX
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


ssize_t must_read( int fd, void* buffer, size_t n );


#ifdef __cplusplus
}
#endif


#endif
