/*
	readlinkat_k.h
	--------------
	
	Joshua Juran
*/

#ifndef LAMP_READLINKATK_H
#define LAMP_READLINKATK_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t readlinkat_k( int dirfd, const char *path, char *buffer, size_t buffer_size );

#ifdef __cplusplus
}
#endif

#endif

