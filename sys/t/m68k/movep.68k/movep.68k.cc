/*
	movep.68k.cc
	------------
*/

#ifndef __MC68K__
#error This unit is for 68K only.
#endif

// Standard C
#include <stdint.h>

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 4;


static inline asm uint32_t MOVEP_L_from( const char* addr : __A0 )
{
	MOVEP.L  0(A0),D0
}

static inline asm void MOVEP_L_to( uint32_t data : __D0, char* addr : __A0 )
{
	MOVEP.L  D0,0(A0)
}

static void movep()
{
	char buffer[] = "01234567";
	
	EXPECT( MOVEP_L_from( buffer     ) == 0x30323436 );
	EXPECT( MOVEP_L_from( buffer + 1 ) == 0x31333537 );
	
	MOVEP_L_to( 0x61626364, buffer );
	
	EXPECT_CMP( buffer, 8, "a1b3c5d7", 8 );
	
	MOVEP_L_to( 0x5758595A, buffer + 1 );
	
	EXPECT_CMP( buffer, 8, "aWbXcYdZ", 8 );
}

int main( int argc, char** argv )
{
	tap::start( "movep.68k", n_tests );
	
	movep();
	
	return 0;
}
