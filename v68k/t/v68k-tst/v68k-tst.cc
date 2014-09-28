/*
	v68k-tst.cc
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


static const unsigned n_tests = 3;


using v68k::big_word;
using v68k::big_longword;


static void tst()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4A80 );  // TST.L  D0
	code[ 1 ] = big_word( 0x4A40 );  // TST.W  D0
	code[ 2 ] = big_word( 0x4A00 );  // TST.B  D0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.d(0) = 0x0000FF80;
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x8 );
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x8 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-tst", n_tests );
	
	tst();
	
	return 0;
}
