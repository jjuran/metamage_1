/*
	rotate.68k.cc
	-------------
*/

#ifndef __MC68K__
#error This unit is for 68K only.
#endif

// Standard C
#include <stdint.h>

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 8 * 4 + (8 + 8) * 3 * 2 + 8 * 3 * 2;


using tap::ok_if;


static uint16_t the_CCR = 0;


static uint16_t the_data;

#define DEFINE_MEM_ROTATOR( mnemonic )                          \
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

DEFINE_MEM_ROTATOR( ror  )
DEFINE_MEM_ROTATOR( rol  )
DEFINE_MEM_ROTATOR( roxr )
DEFINE_MEM_ROTATOR( roxl )


typedef int32_t (*rotator)( int32_t data : __D0, uint32_t count : __D1 );

#define DEFINE_REG_ROTATOR( mnemonic, size )                        \
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

DEFINE_REG_ROTATOR( ror, B )
DEFINE_REG_ROTATOR( ror, W )
DEFINE_REG_ROTATOR( ror, L )

DEFINE_REG_ROTATOR( rol, B )
DEFINE_REG_ROTATOR( rol, W )
DEFINE_REG_ROTATOR( rol, L )

DEFINE_REG_ROTATOR( roxr, B )
DEFINE_REG_ROTATOR( roxr, W )
DEFINE_REG_ROTATOR( roxr, L )

DEFINE_REG_ROTATOR( roxl, B )
DEFINE_REG_ROTATOR( roxl, W )
DEFINE_REG_ROTATOR( roxl, L )


static void ror_memory()
{
	ok_if( ror_mem( 0x0000 ) == 0x0000 );
	
	ok_if( the_CCR == 0x04 );
	
	ok_if( ror_mem( 0x0001 ) == 0x8000 );
	
	ok_if( the_CCR == 0x09 );
	
	ok_if( ror_mem( 0x8000 ) == 0x4000 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( ror_mem( 0xFFFF ) == 0xFFFF );
	
	ok_if( the_CCR == 0x09 );
}

static void rol_memory()
{
	ok_if( rol_mem( 0x0000 ) == 0x0000 );
	
	ok_if( the_CCR == 0x04 );
	
	ok_if( rol_mem( 0x0001 ) == 0x0002 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rol_mem( 0x8000 ) == 0x0001 );
	
	ok_if( the_CCR == 0x01 );
	
	ok_if( rol_mem( 0xFFFF ) == 0xFFFF );
	
	ok_if( the_CCR == 0x09 );
}

static void roxr_memory()
{
	the_CCR = 0;
	
	ok_if( roxr_mem( 0x0000 ) == 0x0000 );
	
	ok_if( the_CCR == 0x04 );
	
	ok_if( roxr_mem( 0x0001 ) == 0x0000 );
	
	ok_if( the_CCR == 0x15 );
	
	ok_if( roxr_mem( 0x0000 ) == 0x8000 );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( roxr_mem( 0xFFFF ) == 0x7FFF );
	
	ok_if( the_CCR == 0x11 );
}

static void roxl_memory()
{
	the_CCR = 0;
	
	ok_if( roxl_mem( 0x0000 ) == 0x0000 );
	
	ok_if( the_CCR == 0x04 );
	
	ok_if( roxl_mem( 0x8000 ) == 0x0000 );
	
	ok_if( the_CCR == 0x15 );
	
	ok_if( roxl_mem( 0x0000 ) == 0x0001 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( roxl_mem( 0xFFFF ) == 0xFFFE );
	
	ok_if( the_CCR == 0x19 );
}


static void rotate_any( rotator rotate )
{
	the_CCR = 0x00;
	
	ok_if( rotate( 0x0000, 0 ) == 0x0000 );
	
	ok_if( the_CCR == 0x04 );
	
	
	the_CCR = 0x1F;
	
	ok_if( rotate( 0x0001, 0 ) == 0x0001 );
	
	ok_if( the_CCR == 0x10 );
	
	
	// Test that count is mod 64
	
	ok_if( rotate( 0x0001, 64 ) == 0x0001 );
	
	ok_if( the_CCR == 0x10 );
}

static void rotate_either( rotator rotate )
{
	rotate_any( rotate );
	
	
	ok_if( rotate( 0x0000, 32 ) == 0x0000 );
	
	ok_if( the_CCR == 0x14 );
}


static void ror_byte()
{
	rotator rotate = &ror_B;
	
	rotate_either( rotate );
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x76543210, 4 ) == 0x76543201 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x76543201, 4 ) == 0x76543210 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x89ABCDFE );
	
	ok_if( the_CCR == 0x09 );
	
	ok_if( rotate( 0x89ABCDFE, 4 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x09 );
}

static void ror_word()
{
	rotator rotate = &ror_W;
	
	rotate_either( rotate );
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x76543210, 4 ) == 0x76540321 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x76540321, 4 ) == 0x76541032 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x89ABFCDE );
	
	ok_if( the_CCR == 0x09 );
	
	ok_if( rotate( 0x89ABFCDE, 4 ) == 0x89ABEFCD );
	
	ok_if( the_CCR == 0x09 );
}

static void ror_long()
{
	rotator rotate = &ror_L;
	
	rotate_either( rotate );
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x76543210, 4 ) == 0x07654321 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x87654321, 4 ) == 0x18765432 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0xF89ABCDE );
	
	ok_if( the_CCR == 0x09 );
	
	ok_if( rotate( 0x789ABCDE, 4 ) == 0xE789ABCD );
	
	ok_if( the_CCR == 0x09 );
}


static void rol_byte()
{
	rotator rotate = &rol_B;
	
	rotate_either( rotate );
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x76543210, 4 ) == 0x76543201 );
	
	ok_if( the_CCR == 0x01 );
	
	ok_if( rotate( 0x76543201, 4 ) == 0x76543210 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x89ABCDFE );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDFE, 4 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x09 );
}

static void rol_word()
{
	rotator rotate = &rol_W;
	
	rotate_either( rotate );
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x76543210, 4 ) == 0x76542103 );
	
	ok_if( the_CCR == 0x01 );
	
	ok_if( rotate( 0x76542103, 4 ) == 0x76541032 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x89ABDEFC );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABDEFC, 4 ) == 0x89ABEFCD );
	
	ok_if( the_CCR == 0x09 );
}

static void rol_long()
{
	rotator rotate = &rol_L;
	
	rotate_either( rotate );
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x76543210, 4 ) == 0x65432107 );
	
	ok_if( the_CCR == 0x01 );
	
	ok_if( rotate( 0x87654321, 4 ) == 0x76543218 );
	
	ok_if( the_CCR == 0x00 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x9ABCDEF8 );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x789ABCDE, 4 ) == 0x89ABCDE7 );
	
	ok_if( the_CCR == 0x09 );
}


static void roxr_byte()
{
	rotator rotate = &roxr_B;
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 9 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x89ABCDEE );
	
	ok_if( the_CCR == 0x19 );
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x19 );
}

static void roxr_word()
{
	rotator rotate = &roxr_W;
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 17 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x89ABECDE );
	
	ok_if( the_CCR == 0x19 );
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x19 );
}

static void roxr_long()
{
	rotator rotate = &roxr_L;
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 33 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0xE89ABCDE );
	
	ok_if( the_CCR == 0x19 );
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x19 );
}


static void roxl_byte()
{
	rotator rotate = &roxl_B;
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 9 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x89ABCDF7 );
	
	ok_if( the_CCR == 0x08 );
	
	the_CCR = 0x1F;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x19 );
}

static void roxl_word()
{
	rotator rotate = &roxl_W;
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 17 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x89ABDEF6 );
	
	ok_if( the_CCR == 0x08 );
	
	the_CCR = 0x1F;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x19 );
}

static void roxl_long()
{
	rotator rotate = &roxl_L;
	
	the_CCR = 0x00;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 33 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x08 );
	
	ok_if( rotate( 0x89ABCDEF, 4 ) == 0x9ABCDEF4 );
	
	ok_if( the_CCR == 0x08 );
	
	the_CCR = 0x1F;
	
	ok_if( rotate( 0x89ABCDEF, 0 ) == 0x89ABCDEF );
	
	ok_if( the_CCR == 0x19 );
}


int main( int argc, char** argv )
{
	tap::start( "rotate.68k", n_tests );
	
	ror_memory();
	rol_memory();
	roxr_memory();
	roxl_memory();
	
	ror_byte();
	ror_word();
	ror_long();
	
	rol_byte();
	rol_word();
	rol_long();
	
	roxr_byte();
	roxr_word();
	roxr_long();
	
	roxl_byte();
	roxl_word();
	roxl_long();
	
	return 0;
}

