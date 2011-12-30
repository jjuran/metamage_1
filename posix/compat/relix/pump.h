/*
	pump.h
	------
	
	Joshua Juran
*/

#ifndef RELIX_PUMP_H
#define RELIX_PUMP_H

// POSIX
#include <sys/types.h>

#ifndef __LAMP__

#ifdef __cplusplus
extern "C"
#endif

ssize_t pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count, unsigned flags );

#endif

#endif

