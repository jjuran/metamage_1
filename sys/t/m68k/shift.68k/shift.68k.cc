/*
	shift.68k.cc
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


static const unsigned n_tests = 6 * 4 + (18 + 8 + 2 + 2) * 3 + (18 + 2 + 2) * 3 + (12 + 2) * 3 * 2;


static uint16_t the_CCR = 0;


static uint16_t the_data;

#define DEFINE_MEM_SHIFTER( mnemonic )                          \
	static asm uint16_t mnemonic##_mem( uint16_t data : __D0 )  \
	{                                                           \
		MOVE.W  D0,the_data     ;                               \
		                                                        \
		MOVE.W  the_CCR,D2      ;                               \
		MOVE    D2,CCR          ;                               \
		                                                        \
		mnemonic.W   the_data   ;                               \
		                                                        \
		MOVE    SR,the_CCR      ;                               \
		ANDI.W  #0x00FF,the_CCR ;                               \
		                                                        \
		MOVE.W  the_data,D0     ;                               \
		                                                        \
		RTS                                                     \
	}

DEFINE_MEM_SHIFTER( asr )
DEFINE_MEM_SHIFTER( asl )
DEFINE_MEM_SHIFTER( lsr )
DEFINE_MEM_SHIFTER( lsl )


typedef int32_t (*shifter)( int32_t data : __D0, uint32_t count : __D1 );

#define DEFINE_REG_SHIFTER( mnemonic, size )                        \
	static asm int32_t mnemonic##_##size( int32_t   data  : __D0,   \
	                                      uint32_t  count : __D1 )  \
	{                                                               \
		MOVE.W  the_CCR,D2      ;                                   \
		MOVE    D2,CCR          ;                                   \
		                                                            \
		mnemonic.size   D1,D0   ;                                   \
		                                                            \
		MOVE    SR,the_CCR      ;                                   \
		ANDI.W  #0x00FF,the_CCR ;                                   \
		                                                            \
		RTS                                                         \
	}

DEFINE_REG_SHIFTER( asr, B )
DEFINE_REG_SHIFTER( asr, W )
DEFINE_REG_SHIFTER( asr, L )

DEFINE_REG_SHIFTER( asl, B )
DEFINE_REG_SHIFTER( asl, W )
DEFINE_REG_SHIFTER( asl, L )

DEFINE_REG_SHIFTER( lsr, B )
DEFINE_REG_SHIFTER( lsr, W )
DEFINE_REG_SHIFTER( lsr, L )

DEFINE_REG_SHIFTER( lsl, B )
DEFINE_REG_SHIFTER( lsl, W )
DEFINE_REG_SHIFTER( lsl, L )


static void asr_memory()
{
	EXPECT( asr_mem( 0x0000 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x04 );
	
	EXPECT( asr_mem( 0x0001 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x15 );
	
	EXPECT( asr_mem( 0xFFFF ) == 0xFFFF );
	
	EXPECT( the_CCR == 0x19 );
}

static void asl_memory()
{
	EXPECT( asl_mem( 0x0000 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x04 );
	
	EXPECT( asl_mem( 0x8000 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x17 );
	
	EXPECT( asl_mem( 0xFFFF ) == 0xFFFE );
	
	EXPECT( the_CCR == 0x19 );
}

static void lsr_memory()
{
	EXPECT( lsr_mem( 0x0000 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x04 );
	
	EXPECT( lsr_mem( 0x0001 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x15 );
	
	EXPECT( lsr_mem( 0xFFFF ) == 0x7FFF );
	
	EXPECT( the_CCR == 0x11 );
}

static void lsl_memory()
{
	EXPECT( lsl_mem( 0x0000 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x04 );
	
	EXPECT( lsl_mem( 0x8000 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x15 );
	
	EXPECT( lsl_mem( 0xFFFF ) == 0xFFFE );
	
	EXPECT( the_CCR == 0x19 );
}


static void shift_either( shifter shift )
{
	the_CCR = 0x00;
	
	EXPECT( shift( 0x0000, 0 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x04 );
	
	
	// Test that a 0 count clears C but not X
	
	the_CCR = 0x1F;
	
	EXPECT( shift( 0x0001, 0 ) == 0x0001 );
	
	EXPECT( the_CCR == 0x10 );
	
	
	// Test that count is mod 64
	
	EXPECT( shift( 0x0001, 64 ) == 0x0001 );
	
	EXPECT( the_CCR == 0x10 );
	
	
	EXPECT( shift( 0x0000, 1 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x04 );
}

static void shift_32( shifter shift )
{
	// Test that count is not mod 32
	
	EXPECT( shift( 0x0002, 32 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x04 );
	
	
	EXPECT( shift( 0x0002, 33 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x04 );
}

static void shift_right( shifter shift )
{
	shift_either( shift );
	shift_32    ( shift );
	
	EXPECT( shift( 0x0001, 1 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x15 );
	
	
	EXPECT( shift( 0x0002, 1 ) == 0x0001 );
	
	EXPECT( the_CCR == 0x00 );
	
	
	EXPECT( shift( 0x0003, 1 ) == 0x0001 );
	
	EXPECT( the_CCR == 0x11 );
}


static void shift_right_word( shifter shift )
{
	EXPECT( shift( 0x76543210, 4 ) == 0x76540321 );
	
	EXPECT( the_CCR == 0x00 );
}

static void shift_right_byte( shifter shift )
{
	EXPECT( shift( 0x76543210, 4 ) == 0x76543201 );
	
	EXPECT( the_CCR == 0x00 );
}

static void shift_right_long( shifter shift )
{
	EXPECT( shift( 0x76543210, 4 ) == 0x07654321 );
	
	EXPECT( the_CCR == 0x00 );
}


static void asr( shifter shift )
{
	shift_right( shift );
	
	
	EXPECT( shift( 0xFFFFFFFF, 1 ) == 0xFFFFFFFF );
	
	EXPECT( the_CCR == 0x19 );
	
	
	EXPECT( shift( 0xFFFFFFFE, 1 ) == 0xFFFFFFFF );
	
	EXPECT( the_CCR == 0x08 );
	
	
	EXPECT( shift( 0xFFFFFFFD, 1 ) == 0xFFFFFFFE );
	
	EXPECT( the_CCR == 0x19 );
	
	
	EXPECT( shift( 0xFFFFFFFD, 2 ) == 0xFFFFFFFF );
	
	EXPECT( the_CCR == 0x08 );
}


static void asr_byte()
{
	shifter shift = &asr_B;
	
	asr( shift );
	
	shift_right_byte( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x89ABCDFE );
	
	EXPECT( the_CCR == 0x19 );
}

static void asr_word()
{
	shifter shift = &asr_W;
	
	asr( shift );
	
	shift_right_word( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x89ABFCDE );
	
	EXPECT( the_CCR == 0x19 );
}

static void asr_long()
{
	shifter shift = &asr_L;
	
	asr( shift );
	
	shift_right_long( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0xF89ABCDE );
	
	EXPECT( the_CCR == 0x19 );
}


static void lsr_byte()
{
	shifter shift = &lsr_B;
	
	shift_right     ( shift );
	shift_right_byte( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x89ABCD0E );
	
	EXPECT( the_CCR == 0x11 );
}

static void lsr_word()
{
	shifter shift = &lsr_W;
	
	shift_right     ( shift );
	shift_right_word( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x89AB0CDE );
	
	EXPECT( the_CCR == 0x11 );
}

static void lsr_long()
{
	shifter shift = &lsr_L;
	
	shift_right     ( shift );
	shift_right_long( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x089ABCDE );
	
	EXPECT( the_CCR == 0x11 );
}


static void asl( shifter shift )
{
	shift_either( shift );
	
	// Test that count is not mod 32
	
	EXPECT( shift( 0x0002, 32 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x06 );
	
	
	EXPECT( shift( 0x0002, 33 ) == 0x0000 );
	
	EXPECT( the_CCR == 0x06 );
}

static void lsl( shifter shift )
{
	shift_either( shift );
	shift_32    ( shift );
}


static void asl_byte()
{
	shifter shift = &asl_B;
	
	asl( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x89ABCDF0 );
	
	EXPECT( the_CCR == 0x0A );
}

static void asl_word()
{
	shifter shift = &asl_W;
	
	asl( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x89ABDEF0 );
	
	EXPECT( the_CCR == 0x0A );
}

static void asl_long()
{
	shifter shift = &asl_L;
	
	asl( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x9ABCDEF0 );
	
	EXPECT( the_CCR == 0x0A );
}


static void lsl_byte()
{
	shifter shift = &lsl_B;
	
	lsl( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x89ABCDF0 );
	
	EXPECT( the_CCR == 0x08 );
}

static void lsl_word()
{
	shifter shift = &lsl_W;
	
	lsl( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x89ABDEF0 );
	
	EXPECT( the_CCR == 0x08 );
}

static void lsl_long()
{
	shifter shift = &lsl_L;
	
	lsl( shift );
	
	EXPECT( shift( 0x89ABCDEF, 4 ) == 0x9ABCDEF0 );
	
	EXPECT( the_CCR == 0x08 );
}


int main( int argc, char** argv )
{
	tap::start( "shift.68k", n_tests );
	
	asr_memory();
	asl_memory();
	lsr_memory();
	lsl_memory();
	
	asr_byte();
	asr_word();
	asr_long();
	
	lsr_byte();
	lsr_word();
	lsr_long();
	
	asl_byte();
	asl_word();
	asl_long();
	
	lsl_byte();
	lsl_word();
	lsl_long();
	
	return 0;
}
