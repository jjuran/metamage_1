/*
	v68k-chk.cc
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


static const unsigned n_tests = 7;


using v68k::big_word;
using v68k::big_longword;


static void chk()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4382 );  // CHK.W  D2,D1
	code[ 1 ] = big_word( 0x4382 );  // CHK.W  D2,D1
	code[ 2 ] = big_word( 0x4382 );  // CHK.W  D2,D1
	code[ 3 ] = big_word( 0x4382 );  // CHK.W  D2,D1
	code[ 4 ] = big_word( 0x4382 );  // CHK.W  D2,D1
	
	code[ 10 ] = big_word( 0x4E73 );  // RTE
	
	const uint32_t chk_address = 1024 + sizeof (uint16_t) * 10;
	
	vectors[6] = big_longword( chk_address );
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0;
	
	emu.d(1) = 0x00000001;
	emu.d(2) = 0x00000002;
	
	
	// 0 < 1 < 2
	
	emu.step();
	
	EXPECT( emu.pc() < chk_address );
	
	
	// 0 <= 0 < 2
	
	emu.d(1) = 0x00000000;
	
	emu.step();
	
	EXPECT( emu.pc() < chk_address );
	
	
	// 0 < 2 <= 2
	
	emu.d(1) = 0x00000002;
	
	emu.step();
	
	EXPECT( emu.pc() < chk_address );
	
	
	// 0 < 3 > 2
	
	emu.d(1) = 0x00000003;
	
	emu.step();
	
	EXPECT( emu.pc() == chk_address );
	
	EXPECT( !(emu.sr.nzvc & 0x8) );
	
	emu.step();  // RTE
	
	
	// 0 > -1 < 2
	
	emu.d(1) = 0x0000FFFF;
	
	emu.step();
	
	EXPECT( emu.pc() == chk_address );
	
	EXPECT( emu.sr.nzvc & 0x8 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-chk", n_tests );
	
	chk();
	
	return 0;
}
