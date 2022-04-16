/*
	v68k-neg.cc
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


static const unsigned n_tests = 4;


using v68k::big_word;
using v68k::big_longword;


static void neg()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4400 );  // NEG.B  D0
	code[ 1 ] = big_word( 0x4440 );  // NEG.W  D0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.d(0) = 0x00000000;
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000000 );
	
	EXPECT( emu.sr.nzvc == 0x4 );
	
	
	emu.d(0) = 0x00000001;
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x0000FFFF );
	
	EXPECT( emu.sr.nzvc == 0x9 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-neg", n_tests );
	
	neg();
	
	return 0;
}
