/*
	v68k-cmp.cc
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


static const unsigned n_tests = 6 + 2 + 4;


using v68k::big_word;
using v68k::big_longword;


static void cmp()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xB200 );  // CMP.B  D0,D1
	code[ 1 ] = big_word( 0xB001 );  // CMP.B  D1,D0
	code[ 2 ] = big_word( 0xB240 );  // CMP.W  D0,D1
	code[ 3 ] = big_word( 0xB041 );  // CMP.W  D1,D0
	code[ 4 ] = big_word( 0xB280 );  // CMP.L  D0,D1
	code[ 5 ] = big_word( 0xB081 );  // CMP.L  D1,D0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.d(0) = 0x80000000;
	emu.d(1) = 0x80008001;
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x9 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x8 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x1 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x9 );
}

static void cmpa()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xB0C0 );  // CMPA.W  D0,A0
	code[ 1 ] = big_word( 0xB1C0 );  // CMPA.L  D0,A0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.d(0) = 0x7654abcd;
	emu.a(0) = 0x7654abcd;
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x1 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x4 );
}

static void cmpm()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xB348 );  // CMPM.W  A0,A1
	code[ 1 ] = big_word( 0xB348 );  // CMPM.W  A0,A1
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.a(0) = 1024;
	emu.a(1) = 1024 + 2;
	
	
	emu.step();
	
	EXPECT( emu.a(0) == 1024 + 2 );
	EXPECT( emu.a(1) == 1024 + 4 );
	
	EXPECT( emu.sr.nzvc == 0x4 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x0 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-cmp", n_tests );
	
	cmp();
	
	cmpa();
	
	cmpm();
	
	return 0;
}
