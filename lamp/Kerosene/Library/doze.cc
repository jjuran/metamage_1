/*
	doze.cc
	-------
	
	Copyright 2009, Joshua Juran
*/

// Standard C
#include "time.h"


#pragma exceptions off


int doze( struct timespec const  *minimum,
          struct timespec const  *maximum,
          struct timespec        *remaining )
{
	const struct timespec zero = { 0, 0 };
	
	const struct timespec never = { 1000000000, 0 };
	
	if ( minimum == NULL )
	{
		minimum = &zero;
	}
	
	if ( maximum == NULL )
	{
		maximum = &never;
	}
	
	const struct timespec input[3] = { { 0, NANOSLEEP_DOZE }, *minimum, *maximum };
	
	return nanosleep( input, remaining );
}
