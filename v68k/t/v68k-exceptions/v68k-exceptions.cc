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


static const unsigned n_tests = 4 + 6 + 4 + 4 + 2 + 2;


using v68k::big_word;
using v68k::big_longword;


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
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.step() );  // Illegal Instruction
	
	EXPECT( emu.pc() == 2048 );
	
	code[ 0 ] = big_word( 0x4848 );  // BKPT  #0
	
	emu.reset();
	
	EXPECT( emu.step() );  // Illegal Instruction (unacknowledged breakpoint)
	
	EXPECT( emu.pc() == 2048 );
}

static void trapv()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ] = { 0 };
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	vectors[7] = big_longword( 2048 );  // TRAPV
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4E76 );  // TRAPV
	code[ 1 ] = big_word( 0x4E76 );  // TRAPV
	code[ 2 ] = big_word( 0x4E76 );  // TRAPV
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.set_CCR( 0x0 );
	
	EXPECT( emu.step() );  // TRAPV
	
	EXPECT( emu.pc() == 1026 );
	
	emu.set_CCR( 0x1D );  // X|N|Z|0|C
	
	EXPECT( emu.step() );  // TRAPV
	
	EXPECT( emu.pc() == 1028 );
	
	emu.set_CCR( 0x2 );  // V
	
	EXPECT( emu.step() );  // TRAPV
	
	EXPECT( emu.pc() == 2048 );
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
	
	code[ 0 ] = big_word( 0x40C0 );  // MOVE SR,D0
	
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.set_SR( 0x0700 );  // Clear S
	
	emu.a(7) = 3072;
	
	EXPECT( emu.step() );  // unprivileged on 68000
	
	EXPECT( emu.pc() == 1024 + 2 );
	
	
	emulator emu2( mc68010, memory );
	
	emu2.reset();
	
	emu2.set_SR( 0x0700 );  // Clear S
	
	emu2.a(7) = 3072;
	
	EXPECT( emu2.step() );  // Privilege Violation
	
	EXPECT( emu2.pc() == 2048 );
}

static void trap()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ] = { 0 };
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	vectors[32 + 2] = big_longword( 2048 );  // TRAP  #2
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x4E42 );  // TRAP  #2
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.step() );  // Trap 0
	
	EXPECT( emu.pc() == 2048 );
	
	vectors[32] = 0xFFFFFFFF;  // TRAP  #0
	
	vectors[47] = big_longword( 2048 );  // TRAP  #15
	
	code[ 0 ] = big_word( 0x4E4F );  // TRAP  #15
	
	emu.reset();
	
	EXPECT( emu.step() );  // Trap  15
	
	EXPECT( emu.pc() == 2048 );
}

static void line_A_emulator()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ] = { 0 };
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	vectors[10] = big_longword( 2048 );  // Line A Emulator
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xA000 );  // unimplemented A-line trap
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.step() );  // unimplemented A-line trap
	
	EXPECT( emu.pc() == 2048 );
}

static void line_F_emulator()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ] = { 0 };
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	vectors[11] = big_longword( 2048 );  // Line F Emulator
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0xF000 );  // unimplemented F-line trap
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.step() );  // unimplemented F-line trap
	
	EXPECT( emu.pc() == 2048 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-exceptions", n_tests );
	
	illegal_instruction();
	
	trapv();
	
	privilege_violation();
	
	trap();
	
	line_A_emulator();
	
	line_F_emulator();
	
	return 0;
}
