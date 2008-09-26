/*	======
 *	pump.h
 *	======
 */

#ifndef PUMP_H
#define PUMP_H

// POSIX
#include <sys/types.h>

#ifndef __LAMP__

#ifdef __cplusplus
extern "C"
#endif

ssize_t pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count );

#endif

#endif

