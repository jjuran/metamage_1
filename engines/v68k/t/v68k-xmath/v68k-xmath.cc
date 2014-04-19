/*
	v68k-xmath.cc
	-------------
*/

// Standard C
#include <string.h>

// v68k
#include "v68k/emulator.hh"
#include "v68k/endian.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 12 + 2 + 4;


using v68k::big_word;
using v68k::big_longword;

using tap::ok_if;


static void addx()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xD340 );  // ADDX.W  D0,D1
	code[ 1 ] = big_word( 0xD300 );  // ADDX.B  D0,D1
	code[ 2 ] = big_word( 0xD308 );  // ADDX.B  -(A0),-(A1)
	code[ 3 ] = big_word( 0xD308 );  // ADDX.B  -(A0),-(A1)
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.regs.   x = 1;
	emu.regs.nzvc = 0;
	
	emu.d(0) = 0x00000123;
	emu.d(1) = 0x00000567;
	
	emu.a(0) = 1027;
	emu.a(1) = 1028;
	
	
	emu.step();
	
	ok_if( emu.d(1) == 0x0000068B );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	emu.d(0) = 0x00000000;
	emu.d(1) = 0x00000000;
	
	emu.regs.   x = 0;
	emu.regs.nzvc = 0;
	
	emu.step();
	
	ok_if( emu.d(1) == 0x00000000 );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	emu.step();
	
	ok_if( emu.a(0) == 1026 );
	ok_if( emu.a(1) == 1027 );
	
	ok_if( code[1] == big_word( 0xD3D3 ) );
	
	ok_if( emu.regs.   x == 0x0 );
	ok_if( emu.regs.nzvc == 0x8 );
	
	
	emu.step();
	
	ok_if( code[1] == big_word( 0x13D3 ) );
	
	ok_if( emu.regs.   x == 0x1 );
	ok_if( emu.regs.nzvc == 0x1 );
}

static void subx()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x9340 );  // ADDX.W  D0,D1
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.regs.   x = 1;
	emu.regs.nzvc = 0;
	
	emu.d(0) = 0x00000000;
	emu.d(1) = 0x00000001;
	
	
	emu.step();
	
	ok_if( emu.d(1) == 0x00000000 );
	
	ok_if( emu.regs.nzvc == 0x0 );
}

static void negx()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4000 );  // NEGX.B  D0
	code[ 1 ] = big_word( 0x4040 );  // NEGX.W  D0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.regs.   x = 1;
	emu.regs.nzvc = 0;
	
	emu.d(0) = 0x000000FF;
	
	
	emu.step();
	
	ok_if( emu.d(0) == 0x00000000 );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	emu.regs.x = 1;
	
	emu.step();
	
	ok_if( emu.d(0) == 0x0000FFFF );
	
	ok_if( emu.regs.nzvc == 0x9 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-xmath", n_tests );
	
	addx();
	
	subx();
	
	negx();
	
	return 0;
}

