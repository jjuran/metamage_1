/*
	clock/time.h
	------------
*/

#ifndef CLOCK_TIME_H
#define CLOCK_TIME_H

// Standard C
#include <time.h>

/*
	__MACH__ is predefined.
	__MAC_10_12 is defined in <Availability.h> (included by <time.h>).
*/

#if defined( __MACH__ )  &&  ! defined( __MAC_10_12 )

#ifdef __cplusplus
extern "C" {
#endif

typedef int clockid_t;

#define CLOCK_REALTIME   0
#define CLOCK_MONOTONIC  1

int clock_getres ( clockid_t clock_id, struct timespec* ts );
int clock_gettime( clockid_t clock_id, struct timespec* ts );

#ifdef __cplusplus
}
#endif

#endif  // #if defined( __MACH__ )  &&  ! defined( __MAC_10_12 )

#endif
