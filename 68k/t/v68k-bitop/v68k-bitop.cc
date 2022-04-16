/*
	v68k-bitop.cc
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


static const unsigned n_tests = 4 + 12 + 4;


using v68k::big_word;
using v68k::big_longword;


static void btst()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x08C0 );  // BTST  #0,D0
	code[ 1 ] = big_word( 0x0000 );
	code[ 2 ] = big_word( 0x60FA );  // BRA.S *-4
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	
	emu.d(0) = 0x00000123;
	
	
	emu.sr.nzvc = 0x0;
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000123 );
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	
	emu.step();  // BRA.S *-4
	
	
	emu.sr.nzvc = 0xF;
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0xB );
	
	
	emu.step();  // BRA.S *-4
	
	
	emu.d(0) = 0x00000122;
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0xF );
	
}

static void modify()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x03C0 );  // BSET  D1,D0
	code[ 1 ] = big_word( 0x0340 );  // BCHG  D1,D0
	code[ 2 ] = big_word( 0x03C0 );  // BSET  D1,D0
	code[ 3 ] = big_word( 0x0380 );  // BCLR  D1,D0
	code[ 4 ] = big_word( 0x0380 );  // BCLR  D1,D0
	code[ 5 ] = big_word( 0x0340 );  // BCHG  D1,D0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	
	emu.d(0) = 0x00000123;
	emu.d(1) = 0x00000000;
	
	emu.sr.nzvc = 0x0;
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000123 );
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000122 );
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000123 );
	
	EXPECT( emu.sr.nzvc == 0x4 );
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000122 );
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000122 );
	
	EXPECT( emu.sr.nzvc == 0x4 );
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000123 );
	
	EXPECT( emu.sr.nzvc == 0x4 );
	
}

static void modulo()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x03D0 );  // BSET  D1,(A0)
	code[ 1 ] = big_word( 0x03D0 );  // BSET  D1,(A0)
	code[ 2 ] = big_word( 0x03C0 );  // BSET  D1,D0
	code[ 3 ] = big_word( 0x03C0 );  // BSET  D1,D0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	
	emu.d(0) = 0x00000000;
	emu.d(1) = 0x00000004;
	emu.a(0) = 1024;
	
	
	emu.step();
	
	EXPECT( mem[1024] == 0x13 );
	
	
	emu.d(1) = 0x0000000A;
	
	
	emu.step();
	
	EXPECT( mem[1024] == 0x17 );
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000400 );
	
	
	emu.d(1) = 0x00000020;
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000401 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-bitop", n_tests );
	
	btst();
	
	modify();
	
	modulo();
	
	return 0;
}
