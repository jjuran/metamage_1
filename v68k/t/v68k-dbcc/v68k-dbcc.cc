/*
	v68k-dbcc.cc
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


static const unsigned n_tests = 4;


using v68k::big_word;
using v68k::big_longword;


static void dbcc()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x50C8 );  // DBT  D0,*+0
	code[ 1 ] = big_word( 0xFFFE );
	code[ 2 ] = big_word( 0x51C8 );  // DBF  D0,*+0
	code[ 3 ] = big_word( 0xFFFE );
	code[ 4 ] = big_word( 0x7000 );  // MOVEQ #0,D0
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.sr.nzvc = 0xF;
	
	emu.d(0) = 0x00010001;
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00010001 );
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00010000 );
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x0001FFFF );
	
	
	emu.step();
	
	EXPECT( emu.d(0) == 0x00000000 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-dbcc", n_tests );
	
	dbcc();
	
	return 0;
}
