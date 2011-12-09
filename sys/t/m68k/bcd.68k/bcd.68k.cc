/*
	bcd.68k.cc
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


static const unsigned n_tests = 14 + 10 + 10 * 2;


using tap::ok_if;


static uint16_t the_CCR = 0;


static uint8_t the_data;

static uint16_t the_data_a;
static uint16_t the_data_b;


static asm uint8_t abcd_reg( uint8_t a : __D1, uint8_t b : __D0 )
{
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	ABCD.B   D1,D0          ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	RTS
}

static asm uint8_t sbcd_reg( uint8_t a : __D1, uint8_t b : __D0 )
{
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	SBCD.B   D1,D0          ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	RTS
}

static asm uint16_t abcd_2( uint16_t a : __D0, uint16_t b : __D1 )
{
	MOVE.W  D0,the_data_a   ;
	MOVE.W  D1,the_data_b   ;
	
	LEA  the_data_a+2,A0
	LEA  the_data_b+2,A1
	
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	ABCD.B   -(A0),-(A1)    ;
	ABCD.B   -(A0),-(A1)    ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	MOVE.W  the_data_b,D0
	
	RTS
}

static asm uint16_t sbcd_2( uint16_t a : __D0, uint16_t b : __D1 )
{
	MOVE.W  D0,the_data_a   ;
	MOVE.W  D1,the_data_b   ;
	
	LEA  the_data_a+2,A0
	LEA  the_data_b+2,A1
	
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	SBCD.B   -(A0),-(A1)    ;
	SBCD.B   -(A0),-(A1)    ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	MOVE.W  the_data_b,D0
	
	RTS
}


static asm uint8_t nbcd_reg( uint8_t a : __D0 )
{
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	NBCD.B   D0             ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	RTS
}

static asm uint8_t nbcd_mem( uint8_t a : __D0 )
{
	MOVE.B  D0,the_data     ;
	
	MOVE.W  the_CCR,D2      ;
	MOVE    D2,CCR          ;
	
	NBCD.B   the_data       ;
	
	MOVE    SR,the_CCR      ;
	ANDI.W  #0x00FF,the_CCR ;
	
	MOVE.B  the_data,D0     ;
	
	RTS
}


static void abcd()
{
	the_CCR = 0x00;
	
	ok_if( abcd_reg( 0x00, 0x00 ) == 0x00 );
	
	ok_if( (the_CCR & 0x15) == 0x00 );
	
	
	the_CCR = 0x0F;
	
	ok_if( abcd_reg( 0x00, 0x00 ) == 0x00 );
	
	ok_if( (the_CCR & 0x15) == 0x04 );
	
	
	the_CCR = 0x1F;
	
	ok_if( abcd_reg( 0x00, 0x00 ) == 0x01 );
	
	ok_if( (the_CCR & 0x15) == 0x00 );
	
	
	ok_if( abcd_reg( 0x09, 0x90 ) == 0x99 );
	
	ok_if( (the_CCR & 0x15) == 0x00 );
	
	
	the_CCR = 0x1F;
	
	ok_if( abcd_reg( 0x09, 0x90 ) == 0x00 );
	
	ok_if( (the_CCR & 0x15) == 0x15 );
	
	
	ok_if( abcd_2( 0x0003, 0x0002 ) == 0x0006 );
	
	ok_if( (the_CCR & 0x15) == 0x00 );
	
	
	ok_if( abcd_2( 0x6003, 0x4002 ) == 0x0005 );
	
	ok_if( (the_CCR & 0x15) == 0x11 );
	
}

static void sbcd()
{
	the_CCR = 0x00;
	
	ok_if( sbcd_reg( 0x00, 0x00 ) == 0x00 );
	
	ok_if( (the_CCR & 0x15) == 0x00 );
	
	
	the_CCR = 0x0F;
	
	ok_if( sbcd_reg( 0x00, 0x00 ) == 0x00 );
	
	ok_if( (the_CCR & 0x15) == 0x04 );
	
	
	the_CCR = 0x1F;
	
	ok_if( sbcd_reg( 0x00, 0x00 ) == 0x99 );
	
	ok_if( (the_CCR & 0x15) == 0x11 );
	
	
	ok_if( sbcd_2( 0x0002, 0x0002 ) == 0x9999 );
	
	ok_if( (the_CCR & 0x15) == 0x11 );
	
	
	the_CCR = 0x1F;
	
	ok_if( sbcd_2( 0x1202, 0x1203 ) == 0x0000 );
	
	ok_if( (the_CCR & 0x15) == 0x04 );
	
}

typedef uint8_t (*nbcd_call)( uint8_t data : __D0 );


static void nbcd( nbcd_call f )
{
	the_CCR = 0x00;
	
	ok_if( f( 0x00 ) == 0x00 );
	
	ok_if( (the_CCR & 0x15) == 0x00 );
	
	
	the_CCR = 0x0F;
	
	ok_if( f( 0x00 ) == 0x00 );
	
	ok_if( (the_CCR & 0x15) == 0x04 );
	
	
	the_CCR = 0x1F;
	
	ok_if( f( 0x00 ) == 0x99 );
	
	ok_if( (the_CCR & 0x15) == 0x11 );
	
	
	ok_if( f( 0x01 ) == 0x98 );
	
	ok_if( (the_CCR & 0x15) == 0x11 );
	
	
	ok_if( f( 0x99 ) == 0x00 );
	
	ok_if( (the_CCR & 0x15) == 0x11 );
}


int main( int argc, char** argv )
{
	tap::start( "bcd.68k", n_tests );
	
	abcd();
	
	sbcd();
	
	nbcd( &nbcd_reg );
	nbcd( &nbcd_mem );
	
	return 0;
}

