/*
	FixMul.cc
	---------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif

// Standard C
#include <stdio.h>
#include <stdlib.h>

// quickdraw
#include "qd/fixed.hh"


using quickdraw::fix_mul;


int main( int argc, char** argv )
{
	if ( argc != 3 )
	{
		return 2;
	}
	
	const int a = atoi( argv[ 1 ] );
	const int b = atoi( argv[ 2 ] );
	
	const int original = FixMul ( a, b );
	const int copyleft = fix_mul( a, b );
	
	printf( "original QD: %d\n", original );
	printf( "copyleft qd: %d\n", copyleft );
	
	return 0;
}
