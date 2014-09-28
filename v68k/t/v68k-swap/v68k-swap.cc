/*
	v68k-swap.cc
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


static const unsigned n_tests = 2;


using v68k::big_word;
using v68k::big_longword;


static void swap()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4840 );  // SWAP  D0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0xF;
	
	emu.d(0) = 0x12345678;
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x56781234 );
	
	EXPECT( emu.sr.nzvc == 0x0 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-swap", n_tests );
	
	swap();
	
	return 0;
}
