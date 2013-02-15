/*
	deadline.hh
	-----------
*/

#ifndef DEADLINE_HH
#define DEADLINE_HH

// POSIX
//#include <time.h>
struct timespec;


bool deadline_expired( const timespec& deadline );

#endif

