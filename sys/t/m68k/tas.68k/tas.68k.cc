/*
	tas.68k.cc
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


static const unsigned n_tests = 8 * 2;


static uint16_t the_CCR = 0;


static uint8_t the_data;

static asm uint8_t tas_reg( uint8_t data : __D0 )
{
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	TAS.B   D0              ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	RTS
}

static asm uint8_t tas_mem( uint8_t data : __D0 )
{
	MOVE.B  D0,the_data     ;
	
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	TAS.B   the_data        ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	MOVE.B  the_data,D0     ;
	
	RTS
}


typedef uint8_t (*test_and_setter)( uint8_t data : __D0 );


static void tas( test_and_setter test_and_set )
{
	the_CCR = 0x00;
	
	EXPECT( test_and_set( 0x00 ) == 0x80 );
	
	EXPECT( the_CCR == 0x04 );
	
	
	the_CCR = 0x1F;
	
	EXPECT( test_and_set( 0x7F ) == 0xFF );
	
	EXPECT( the_CCR == 0x10 );
	
	
	EXPECT( test_and_set( 0x80 ) == 0x80 );
	
	EXPECT( the_CCR == 0x18 );
	
	
	the_CCR = 0x00;
	
	EXPECT( test_and_set( 0xFF ) == 0xFF );
	
	EXPECT( the_CCR == 0x08 );
	
}


int main( int argc, char** argv )
{
	tap::start( "tas.68k", n_tests );
	
	tas( &tas_reg );
	tas( &tas_mem );
	
	return 0;
}
