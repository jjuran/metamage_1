/*
	v68k-add.cc
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

using tap::ok_if;


static void add()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xD240 );  // ADD.W  D0,D1
	code[ 1 ] = big_word( 0xD200 );  // ADD.B  D0,D1
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.regs.nzvc = 0;
	
	emu.regs.d[0] = 0x00000123;
	emu.regs.d[1] = 0x00000567;
	
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x0000068A );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	emu.regs.d[1] = 0x00000567;
	
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x0000058A );
	
	ok_if( emu.regs.nzvc == 0xA );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-add", n_tests );
	
	add();
	
	return 0;
}

