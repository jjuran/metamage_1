/*
	v68k-core.cc
	------------
*/

// v68k
#include "v68k/emulator.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 2 + 1 + 4 + 2 + 2 + 4 + 4 + 6;


using tap::ok_if;


static void no_mem()
{
	using namespace v68k;
	
	emulator emu( mc68000, NULL, 0 );
	
	ok_if( emu.condition == startup );
	
	emu.reset();
	
	ok_if( emu.condition == halted );
}

static void only_7_bytes()
{
	using namespace v68k;
	
	uint8_t mem[ 7 ] = { 0 };
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	ok_if( emu.condition == halted );
}

static void only_8_bytes()
{
	using namespace v68k;
	
	uint8_t mem[ 8 ] = { 0 };
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	ok_if( emu.condition == normal );
	
	ok_if( (emu.get_SR() & 0xFFE0) == 0x2700 );
	
	ok_if( emu.regs.a[7] == 0 );
	ok_if( emu.regs.pc   == 0 );
}

static void bad_SP_on_reset()
{
	using namespace v68k;
	
	uint8_t mem[ 8 ] = { 0xFF, 0xFF, 0xFF, 0xFF };
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	ok_if( emu.condition == normal );
	
	ok_if( emu.regs.a[7] == 0xFFFFFFFF );
}

static void bad_PC_on_reset()
{
	using namespace v68k;
	
	uint8_t mem[ 8 ] = { 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF };
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	ok_if( emu.condition == halted );
	
	ok_if( emu.regs.pc == 0xFFFFFFFF );
}

static void stop_FFFF()
{
	using namespace v68k;
	
	uint8_t mem[ 16 ] =
	{
		0, 0, 0, 0,
		0, 0, 0, 8,
		
		0x4E, 0x72,
		0xFF, 0xFF
	};
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.set_CCR( 0 );
	
	ok_if( !emu.step() );
	
	ok_if( emu.condition == finished );
	
	ok_if( emu.get_SR() == 0x2700 );
	
	ok_if( emu.regs.pc == 12 );
}

static void stop_2EFF()
{
	using namespace v68k;
	
	uint8_t mem[ 16 ] =
	{
		0, 0, 0, 0,
		0, 0, 0, 8,
		
		0x4E, 0x72,
		0x2E, 0xFF
	};
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	ok_if( !emu.step() );
	
	ok_if( emu.condition == stopped );
	
	ok_if( emu.get_SR() == 0x261F );
	
	ok_if( emu.regs.pc == 12 );
}

static void bkpt()
{
	using namespace v68k;
	
	uint8_t mem[ 12 ] =
	{
		0, 0, 0, 0,
		0, 0, 0, 8,
		
		0x48, 0x48
	};
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	ok_if( !emu.step() );
	
	ok_if( emu.condition == bkpt_0 );
	
	ok_if( emu.regs.pc == 8 );
	
	emu.opcode = 0x484F;
	
	emu.condition = normal;
	
	emu.step();
	
	ok_if( emu.condition == bkpt_7 );
	
	emu.acknowledge_breakpoint( 0x4E71 );
	
	ok_if( emu.condition == normal );
	
	ok_if( emu.opcode == 0x4E71 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-core", n_tests );
	
	no_mem();
	
	only_7_bytes();
	
	only_8_bytes();
	
	bad_SP_on_reset();
	
	bad_PC_on_reset();
	
	stop_FFFF();
	
	stop_2EFF();
	
	bkpt();
	
	return 0;
}

