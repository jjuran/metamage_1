/*
	v68k-eor.cc
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


static void eor()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xB181 );  // EOR.L  D0,D1
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.d(0) = 0x00000001;
	emu.d(1) = 0x00000002;
	
	
	emu.step();
	
	EXPECT( emu.sr.nzvc == 0x0 );
	
	EXPECT( emu.d(0) == 1 );
	EXPECT( emu.d(1) == 3 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-eor", n_tests );
	
	eor();
	
	return 0;
}
