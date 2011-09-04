/*
	v68k-bitshift.cc
	----------------
*/

// Standard C
#include <string.h>

// v68k
#include "v68k/emulator.hh"
#include "v68k/endian.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 2 + 2;


using v68k::big_word;
using v68k::big_longword;

using tap::ok_if;


static void asr()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xE240 );  // ASR.W  #1,D0
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.regs.nzvc = 0;
	
	emu.regs.d[0] = 0x00000123;
	
	
	emu.step();
	
	ok_if( emu.regs.d[0] == 0x00000091 );
	
	ok_if( emu.regs.nzvc == 0x1 );
}

static void lsl()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xE548 );  // LSL.W    #2,D0
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.regs.nzvc = 0xF;
	
	emu.regs.d[0] = 0x00001234;
	
	emu.step();
	
	ok_if( emu.regs.d[0] == 0x000048D0 );
	
	ok_if( emu.regs.nzvc == 0x0 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-bitshift", n_tests );
	
	asr();
	
	lsl();
	
	return 0;
}

