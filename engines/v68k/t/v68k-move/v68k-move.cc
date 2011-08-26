/*
	v68k-move.cc
	------------
*/

// Standard C
#include <string.h>

// v68k
#include "v68k/emulator.hh"
#include "v68k/endian.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 6 + 13;


using v68k::big_word;
using v68k::big_longword;

using tap::ok_if;


static void moveq()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x7000 );  // MOVEQ  #00,D0
	code[ 1 ] = big_word( 0x707F );  // MOVEQ  #7F,D0
	code[ 2 ] = big_word( 0x7080 );  // MOVEQ  #80,D0
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.regs.nzvc = 0;
	
	emu.regs.d[0] = 0x12345678;
	
	
	emu.step();
	
	ok_if( emu.regs.d[0] == 0x00000000 );
	
	ok_if( emu.regs.nzvc == 0x4 );
	
	
	emu.step();
	
	ok_if( emu.regs.d[0] == 0x0000007F );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	emu.step();
	
	ok_if( emu.regs.d[0] == 0xFFFFFF80 );
	
	ok_if( emu.regs.nzvc == 0x8 );
}

static void move()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x1200 );  // MOVE.B  D0,D1
	code[ 1 ] = big_word( 0x3041 );  // MOVEA.W  D1,A0
	code[ 2 ] = big_word( 0x3042 );  // MOVEA.W  D2,A0
	code[ 3 ] = big_word( 0x2F02 );  // MOVE.L  D2,-(A7)
	code[ 4 ] = big_word( 0x1F01 );  // MOVE.B  D1,-(A7)
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.regs.nzvc = 0;
	
	emu.regs.d[0] = 0x00000000;
	emu.regs.d[1] = 0x12345678;
	emu.regs.d[2] = 0xCBA98765;
	emu.regs.a[0] = 0x12345678;
	
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x12345600 );
	
	ok_if( emu.regs.nzvc == 0x4 );
	
	
	emu.step();
	
	ok_if( emu.regs.a[0] == 0x00005600 );
	
	ok_if( emu.regs.nzvc == 0x4 );
	
	
	emu.step();
	
	ok_if( emu.regs.a[0] == 0xFFFF8765 );
	
	
	emu.step();
	
	ok_if( emu.regs.a[7] == 0x0FFC );
	
	uint32_t x = 0;
	
	ok_if( emu.mem.get_long( emu.regs.a[7], x, emu.data_space() ) );
	
	ok_if( x == 0xCBA98765 );
	
	ok_if( emu.regs.nzvc == 0x8 );
	
	
	emu.step();
	
	ok_if( emu.regs.a[7] == 0x0FFA );
	
	uint8_t byte = 0;
	
	ok_if( emu.mem.get_byte( emu.regs.a[7], byte, emu.data_space() ) );
	
	ok_if( byte == 0x00 );
	
	ok_if( emu.regs.nzvc == 0x4 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-move", n_tests );
	
	moveq();
	
	move();
	
	return 0;
}

