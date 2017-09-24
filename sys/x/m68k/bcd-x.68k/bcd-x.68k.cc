/*
	bcd-x.68k.cc
	------------
*/

#ifndef __MC68K__
#error This unit is for 68K only.
#endif

// Standard C
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#pragma exceptions off


static uint16_t the_CCR = 0;


static uint8_t the_data;

static uint16_t the_data_a;
static uint16_t the_data_b;


static asm uint8_t abcd_reg( uint8_t a : __D1, uint8_t b : __D0 )
{
	MOVE.W   the_CCR,D2
	MOVE     D2,CCR
	
	ABCD.B   D1,D0
	
	MOVE     SR,the_CCR
	ANDI.W   #0x00FF,the_CCR
	
	RTS
}

static asm uint8_t sbcd_reg( uint8_t a : __D1, uint8_t b : __D0 )
{
	MOVE.W   the_CCR,D2
	MOVE     D2,CCR
	
	SBCD.B   D1,D0
	
	MOVE     SR,the_CCR
	ANDI.W   #0x00FF,the_CCR
	
	RTS
}

static asm uint16_t abcd_2( uint16_t a : __D0, uint16_t b : __D1 )
{
	MOVE.W   D0,the_data_a
	MOVE.W   D1,the_data_b
	
	LEA      the_data_a+2,A0
	LEA      the_data_b+2,A1
	
	MOVE.W   the_CCR,D2
	MOVE     D2,CCR
	
	ABCD.B   -(A0),-(A1)
	ABCD.B   -(A0),-(A1)
	
	MOVE     SR,the_CCR
	ANDI.W   #0x00FF,the_CCR
	
	MOVE.W   the_data_b,D0
	
	RTS
}

static asm uint16_t sbcd_2( uint16_t a : __D0, uint16_t b : __D1 )
{
	MOVE.W   D0,the_data_a
	MOVE.W   D1,the_data_b
	
	LEA      the_data_a+2,A0
	LEA      the_data_b+2,A1
	
	MOVE.W   the_CCR,D2
	MOVE     D2,CCR
	
	SBCD.B   -(A0),-(A1)
	SBCD.B   -(A0),-(A1)
	
	MOVE     SR,the_CCR
	ANDI.W   #0x00FF,the_CCR
	
	MOVE.W   the_data_b,D0
	
	RTS
}


static asm uint8_t nbcd_reg( uint8_t a : __D0 )
{
	MOVE.W   the_CCR,D2
	MOVE     D2,CCR
	
	NBCD.B   D0
	
	MOVE     SR,the_CCR
	ANDI.W   #0x00FF,the_CCR
	
	RTS
}

static asm uint8_t nbcd_mem( uint8_t a : __D0 )
{
	MOVE.B   D0,the_data
	
	MOVE.W   the_CCR,D2
	MOVE     D2,CCR
	
	NBCD.B   the_data
	
	MOVE     SR,the_CCR
	ANDI.W   #0x00FF,the_CCR
	
	MOVE.B   the_data,D0
	
	RTS
}


static
void run_nbcd( uint8_t ccr, bool verbose )
{
	const int limit = verbose ? 0xFF : 0x99;
	
	for ( int i = 0;  i <= limit;  ++i )
	{
		if ( ! verbose  &&  (i & 0xF) > 9 )
		{
			continue;
		}
		
		the_CCR = ccr;
		
		uint8_t bcd = nbcd_reg( i );
		
		if ( verbose )
			printf( "NBCD #%x (%x): %x(%x)\n", i, ccr, bcd, the_CCR );
		
		printf( "NBCD #%x (%x): %x(%x)\n", i, ccr, bcd, the_CCR & 0x15 );
	}
}

static
void run_abcd( uint8_t ccr, bool verbose )
{
	const int limit = verbose ? 0xFF : 0x99;
	
	for ( int i = 0;  i <= limit;  ++i )
	{
		if ( ! verbose  &&  (i & 0xF) > 9 )
		{
			continue;
		}
		
		for ( int j = 0;  j <= limit;  ++j )
		{
			if ( ! verbose  &&  (j & 0xF) > 9 )
			{
				continue;
			}
			
			the_CCR = ccr;
			
			uint8_t bcd = abcd_reg( i, j );
			
			if ( verbose )
				printf( "ABCD #%x,#%x (%x): %x(%x)\n", i, j, ccr, bcd, the_CCR );
			
			printf( "ABCD #%x,#%x (%x): %x(%x)\n", i, j, ccr, bcd, the_CCR & 0x15 );
		}
	}
}

static
void run_sbcd( uint8_t ccr, bool verbose )
{
	const int limit = verbose ? 0xFF : 0x99;
	
	for ( int i = 0;  i <= limit;  ++i )
	{
		if ( ! verbose  &&  (i & 0xF) > 9 )
		{
			continue;
		}
		
		for ( int j = 0;  j <= limit;  ++j )
		{
			if ( ! verbose  &&  (j & 0xF) > 9 )
			{
				continue;
			}
			
			the_CCR = ccr;
			
			uint8_t bcd = sbcd_reg( i, j );
			
			if ( verbose )
				printf( "SBCD #%x,#%x (%x): %x(%x)\n", i, j, ccr, bcd, the_CCR );
			
			printf( "SBCD #%x,#%x (%x): %x(%x)\n", i, j, ccr, bcd, the_CCR & 0x15 );
		}
	}
}

int main( int argc, char** argv )
{
	bool verbose = argc > 1  &&  strcmp( argv[ 1 ], "-v" ) == 0;
	
	run_nbcd( 0x00, verbose );
	run_nbcd( 0x04, verbose );
	run_nbcd( 0x10, verbose );
	run_nbcd( 0x14, verbose );
	
	run_abcd( 0x00, verbose );
	run_abcd( 0x04, verbose );
	run_abcd( 0x10, verbose );
	run_abcd( 0x14, verbose );
	
	run_sbcd( 0x00, verbose );
	run_sbcd( 0x04, verbose );
	run_sbcd( 0x10, verbose );
	run_sbcd( 0x14, verbose );
	
	return 0;
}
