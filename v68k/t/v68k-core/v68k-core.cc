/*
	v68k-core.cc
	------------
*/

// v68k
#include "v68k/emulator.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 2 + 1 + 4 + 4 + 2 + 2 + 4 + 4 + 6;


static void no_mem()
{
	using namespace v68k;
	
	const memory_region memory( NULL, 0 );
	
	emulator emu( mc68000, memory );
	
	EXPECT( emu.condition == startup );
	
	emu.reset();
	
	EXPECT( emu.condition == halted );
}

static void only_7_bytes()
{
	using namespace v68k;
	
	uint8_t mem[ 7 ] = { 0 };
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.condition == halted );
}

static void only_8_bytes()
{
	using namespace v68k;
	
	uint8_t mem[ 8 ] = { 0 };
	
	const low_memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.condition == halted );
	
	EXPECT( (emu.get_SR() & 0xFFE0) == 0x2700 );
	
	EXPECT( emu.a(7) == 0 );
	EXPECT( emu.pc()   == 0 );
}

static void only_1026_bytes()
{
	using namespace v68k;
	
	uint8_t mem[ 1026 ] = { 0, 0, 0, 0,  0, 0, 4, 0 };  // PC = 1024
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.condition == normal );
	
	EXPECT( (emu.get_SR() & 0xFFE0) == 0x2700 );
	
	EXPECT( emu.a(7) ==    0 );
	EXPECT( emu.pc()   == 1024 );
}

static void bad_SP_on_reset()
{
	using namespace v68k;
	
	uint8_t mem[ 1026 ] = { 0xFF, 0xFF, 0xFF, 0xFF,  0, 0, 4, 0 };  // PC = 1024
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.condition == normal );
	
	EXPECT( emu.a(7) == 0xFFFFFFFF );
}

static void bad_PC_on_reset()
{
	using namespace v68k;
	
	uint8_t mem[ 1026 ] = { 0, 0, 0, 0,  0xFF, 0xFF, 0xFF, 0xFF };
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( emu.condition == halted );
	
	EXPECT( emu.pc() == 0xFFFFFFFF );
}

static void stop_FFFF()
{
	using namespace v68k;
	
	uint8_t mem[ 1030 ] =
	{
		0, 0, 0, 0,
		0, 0, 4, 0
	};
	
	mem[1024] = 0x4E;  // STOP  #FFFF
	mem[1025] = 0x72;
	mem[1026] = 0xFF;
	mem[1027] = 0xFF;
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	emu.set_CCR( 0 );
	
	EXPECT( !emu.step() );
	
	EXPECT( emu.condition == finished );
	
	EXPECT( emu.get_SR() == 0x2700 );
	
	EXPECT( emu.pc() == 1028 );
}

static void stop_2EFF()
{
	using namespace v68k;
	
	uint8_t mem[ 1030 ] =
	{
		0, 0, 0, 0,
		0, 0, 4, 0
	};
	
	mem[1024] = 0x4E;  // STOP  #2EFF
	mem[1025] = 0x72;
	mem[1026] = 0x2E;
	mem[1027] = 0xFF;
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory );
	
	emu.reset();
	
	EXPECT( !emu.step() );
	
	EXPECT( emu.condition == stopped );
	
	EXPECT( emu.get_SR() == 0x261F );
	
	EXPECT( emu.pc() == 1028 );
}

static int bkpt_vector = -1;

static uint16_t opcode = 0x4AFC;

static
v68k::op_result bkpt_handler( v68k::processor_state& s, int vector )
{
	bkpt_vector = vector;
	
	s.acknowledge_breakpoint( opcode );
	
	return v68k::Ok;
}

static void bkpt()
{
	using namespace v68k;
	
	uint8_t mem[ 1028 ] =
	{
		0, 0, 0, 0,
		0, 0, 4, 0
	};
	
	mem[1024] = 0x48;  // BKPT  #0
	mem[1025] = 0x48;
	mem[1026] = 0xA1;
	mem[1027] = 0x23;
	
	const memory_region memory( mem, sizeof mem );
	
	emulator emu( mc68000, memory, &::bkpt_handler );
	
	emu.reset();
	
	EXPECT( !emu.step() );
	
	EXPECT( bkpt_vector == 0 );
	
	EXPECT( emu.pc() == 1024 );
	
	emu.opcode = 0x484F;
	
	emu.condition = normal;
	
	opcode = 0x4E71;
	
	emu.step();
	
	EXPECT( bkpt_vector == 7 );
	
	EXPECT( emu.condition == normal );
	
	EXPECT( emu.opcode == 0xA123 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-core", n_tests );
	
	no_mem();
	
	only_7_bytes();
	
	only_8_bytes();
	
	only_1026_bytes();
	
	bad_SP_on_reset();
	
	bad_PC_on_reset();
	
	stop_FFFF();
	
	stop_2EFF();
	
	bkpt();
	
	return 0;
}
