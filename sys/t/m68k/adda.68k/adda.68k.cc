/*
	adda.68k.cc
	-----------
*/

#ifndef __MC68K__
#error This unit is for 68K only.
#endif

// Standard C
#include <stdint.h>

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 2;


static uint16_t the_CCR = 0;


static
asm uint32_t adda_neg1_w( uint32_t b : __A0 ) : __A0
{
	MOVEQ.L  #0,D2
	MOVE     D2,CCR
	
	ADDA.W   #-1,A0
	
	MOVE     SR,the_CCR
	ANDI.W   #0x00FF,the_CCR
	
	RTS
}


static void adda()
{
	the_CCR = 0x00;
	
	EXPECT_EQ( adda_neg1_w( 0x12345678 ), 0x12345677 );
	
	EXPECT( (the_CCR & 0x1F) == 0x00 );
}


int main( int argc, char** argv )
{
	tap::start( "adda.68k", n_tests );
	
	adda();
	
	return 0;
}
