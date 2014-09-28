/*
	ext.68k.cc
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


static const unsigned n_tests = 2 * 6;


static uint16_t the_CCR = 0;


static uint8_t the_data;

static uint16_t the_data_a;
static uint16_t the_data_b;


#define DEFINE_EXTENDER( mnemonic, size )                        \
	static asm int32_t mnemonic##_##size( int32_t data : __D0 )  \
	{                                                            \
		MOVE.W  the_CCR,D2      ;                                \
		MOVE    D2,CCR          ;                                \
		                                                         \
		mnemonic.size   D0      ;                                \
		                                                         \
		MOVE    SR,the_CCR      ;                                \
		ANDI.W  #0x00FF,the_CCR ;                                \
		                                                         \
		RTS                                                      \
	}

DEFINE_EXTENDER( ext, W )
DEFINE_EXTENDER( ext, L )


static void ext_word()
{
	the_CCR = 0x00;
	
	EXPECT( ext_W( 0x00000000 ) == 0x00000000 );
	
	EXPECT( the_CCR == 0x04 );
	
	EXPECT( ext_W( 0xFFFFFFFF ) == 0xFFFFFFFF );
	
	EXPECT( the_CCR == 0x08 );
	
	EXPECT( ext_W( 0xDCBA7600 ) == 0xDCBA0000 );
	
	EXPECT( the_CCR == 0x04 );
}

static void ext_long()
{
	the_CCR = 0x00;
	
	EXPECT( ext_L( 0x00000000 ) == 0x00000000 );
	
	EXPECT( the_CCR == 0x04 );
	
	EXPECT( ext_L( 0xFFFFFFFF ) == 0xFFFFFFFF );
	
	EXPECT( the_CCR == 0x08 );
	
	EXPECT( ext_L( 0xDCBA7600 ) == 0x00007600 );
	
	EXPECT( the_CCR == 0x00 );
}


int main( int argc, char** argv )
{
	tap::start( "ext.68k", n_tests );
	
	ext_word();
	
	ext_long();
	
	return 0;
}
