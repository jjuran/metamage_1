/*	=======
 *	pump.hh
 *	=======
 */

#ifndef PUMP_HH
#define PUMP_HH

// POSIX
#include <sys/types.h>

int pump( int fd_in, off_t* off_in, int fd_out, off_t* off_out, size_t count );

#endif

