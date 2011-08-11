/*
	v68k-exceptions.cc
	------------------
*/

// Standard C
#include <string.h>

// v68k
#include "v68k/emulator.hh"
#include "v68k/endian.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 5 + 4 + 4;


using v68k::big_word;
using v68k::big_longword;

using tap::ok_if;


static void illegal_instruction()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ] = { 0 };
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	vectors[4] = big_longword( 2048 );  // Illegal Instruction
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4AFC );  // ILLEGAL
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	ok_if( emu.step() );  // Illegal Instruction
	
	ok_if( emu.regs.pc == 2048 );
	
	code[ 0 ] = big_word( 0x4848 );  // BKPT  #0
	
	emu.reset();
	
	ok_if( !emu.step() );  // Breakpoint 0
	
	ok_if( emu.step() );  // Illegal Instruction (unacknowledged breakpoint)
	
	ok_if( emu.regs.pc == 2048 );
}

static void privilege_violation()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ] = { 0 };
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	vectors[8] = big_longword( 2048 );  // Privilege Violation
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x42C0 );  // MOVE SR,D0
	
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.set_SR( 0x0700 );  // Clear S
	
	emu.regs.a[7] = 3072;
	
	ok_if( emu.step() );  // unprivileged on 68000
	
	ok_if( emu.regs.pc == 1024 + 2 );
	
	
	emulator emu2( mc68010, mem, sizeof mem );
	
	emu2.reset();
	
	emu2.set_SR( 0x0700 );  // Clear S
	
	emu2.regs.a[7] = 3072;
	
	ok_if( emu2.step() );  // Privilege Violation
	
	ok_if( emu2.regs.pc == 2048 );
}

static void trap()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ] = { 0 };
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	vectors[32] = big_longword( 2048 );  // TRAP  #0
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4E40 );  // TRAP  #0
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	ok_if( emu.step() );  // Trap 0
	
	ok_if( emu.regs.pc == 2048 );
	
	vectors[32] = 0xFFFFFFFF;  // TRAP  #0
	
	vectors[47] = big_longword( 2048 );  // TRAP  #15
	
	code[ 0 ] = big_word( 0x4E4F );  // TRAP  #15
	
	emu.reset();
	
	ok_if( emu.step() );  // Trap  15
	
	ok_if( emu.regs.pc == 2048 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-exceptions", n_tests );
	
	illegal_instruction();
	
	privilege_violation();
	
	trap();
	
	return 0;
}

