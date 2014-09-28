/*
	v68k-mul.cc
	-----------
*/

// Standard C
#include <string.h>

// v68k
#include "v68k/emulator.hh"
#include "v68k/endian.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 6 + 6;


using v68k::big_word;
using v68k::big_longword;


static void mulu()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xC2C0 );  // MULU.W  D0,D1
	code[ 1 ] = big_word( 0xC2C0 );  // MULU.W  D0,D1
	code[ 2 ] = big_word( 0xC2C0 );  // MULU.W  D0,D1
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.d(0) = 0x0001FFFF;
	emu.d(1) = 0x00010000;
	
	
	emu.step();
	
	EXPECT( emu.d(1) == 0x00000000 );
	
	EXPECT( emu.sr.nzvc == 0x4 );
	
	emu.d(1) = 0x00000001;
	
	
	emu.step();
	
	EXPECT( emu.d(1) == 0x0000FFFF );
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	
	emu.step();
	
	EXPECT( emu.d(1) == 0xFFFE0001 );
	
	EXPECT( emu.sr.nzvc == 0x8 );
}

static void muls()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xC3C0 );  // MULS.W  D0,D1
	code[ 1 ] = big_word( 0xC3C0 );  // MULS.W  D0,D1
	code[ 2 ] = big_word( 0xC3C0 );  // MULS.W  D0,D1
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.d(0) = 0x0001FFFF;
	emu.d(1) = 0x00010000;
	
	
	emu.step();
	
	EXPECT( emu.d(1) == 0x00000000 );
	
	EXPECT( emu.sr.nzvc == 0x4 );
	
	emu.d(1) = 0x00000001;
	
	
	emu.step();
	
	EXPECT( emu.d(1) == 0xFFFFFFFF );
	
	EXPECT( emu.sr.nzvc == 0x8 );
	
	
	emu.step();
	
	EXPECT( emu.d(1) == 0x00000001 );
	
	EXPECT( emu.sr.nzvc == 0x0 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-mul", n_tests );
	
	mulu();
	
	muls();
	
	return 0;
}
