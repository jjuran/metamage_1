/*
	cmp.68k.cc
	----------
*/

#ifndef __MC68K__
#error This unit is for 68K only.
#endif

// Standard C
#include <stdint.h>

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 11;


static uint16_t the_CCR = 0;


static asm void cmpa_w( uint16_t a : __D0, uint32_t b : __A0 )
{
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	CMPA.W  D0,A0          ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	RTS
}

static asm void cmpa_l( uint32_t a : __D0, uint32_t b : __A0 )
{
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	CMPA.L  D0,A0          ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	RTS
}


static void cmpa()
{
	the_CCR = 0x00;
	
	
	cmpa_l( 0, 0 );
	
	EXPECT( (the_CCR & 0x1F) == 0x04 );
	
	cmpa_l( 0, 1 );
	
	EXPECT( (the_CCR & 0x1F) == 0x00 );
	
	cmpa_l( 1, 0 );
	
	EXPECT( (the_CCR & 0x1F) == 0x09 );
	
	cmpa_l( 0, 0xFFFFFFFF );
	
	EXPECT( (the_CCR & 0x1F) == 0x08 );
	
	cmpa_l( 0xFFFFFFFF, 0 );
	
	EXPECT( (the_CCR & 0x1F) == 0x01 );
	
	
	cmpa_w( 0, 0 );
	
	EXPECT( (the_CCR & 0x1F) == 0x04 );
	
	cmpa_w( 0, 1 );
	
	EXPECT( (the_CCR & 0x1F) == 0x00 );
	
	cmpa_w( 1, 0 );
	
	EXPECT( (the_CCR & 0x1F) == 0x09 );
	
	cmpa_w( 0, 0xFFFFFFFF );
	
	EXPECT( (the_CCR & 0x1F) == 0x08 );
	
	cmpa_w( 0xFFFFFFFF, 0 );
	
	EXPECT( (the_CCR & 0x1F) == 0x01 );
	
	
	const uint32_t x = 0x7654abcd;
	
	cmpa_w( x, x );  // 0x7654abcd - 0xffffabcd
	
	EXPECT( (the_CCR & 0x1F) == 0x01 );
}


int main( int argc, char** argv )
{
	tap::start( "cmp.68k", n_tests );
	
	cmpa();
	
	return 0;
}
