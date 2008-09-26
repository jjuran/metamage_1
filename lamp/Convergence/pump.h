/*	======
 *	pump.h
 *	======
 */

#ifndef PUMP_H
#define PUMP_H

// POSIX
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count );

#ifdef __cplusplus
}
#endif

#endif

