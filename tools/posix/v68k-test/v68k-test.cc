/*
	v68k-test.cc
	------------
*/

// Standard C
#include <stdio.h>
#include <string.h>

// POSIX
#include <unistd.h>

// v68k
#include "v68k/endian.hh"
#include "v68k/emulator.hh"


#pragma exceptions off


using v68k::big_word;
using v68k::big_longword;


static void dump( const v68k::emulator& emu )
{
	const v68k::registers& regs = emu.regs;
	
	printf( "\n" );
	
	for ( int i = 0;  i < 8;  ++i )
	{
		printf( "D%d: %.8x   A%d: %.8x\n", i, regs.d[i], i, regs.a[i] );
	}
	
	printf( "\n" );
	
	printf( "Alt SP:  %.8x\n", regs.alt_sp  );
	printf( "Alt SSP: %.8x\n", regs.alt_ssp );
	
	printf( "\n" );
	
	printf( "PC: %.8x\n", regs.pc );
	printf( "SR: %.4x\n", emu.get_SR() );
}


const uint32_t data_address  = 1024;
const uint32_t stack_address = 2048;
const uint32_t code_address  = 2048;

static const uint16_t program[] =
{
	0x7000,  // MOVEQ  #00,D0
	
	0x41F8,  // LEA  (1024).W, A0
	0x0400,
	
	0x6002,  // BRA.S *+4
	
	0x4848,  // BKPT  #0
	
	0x1018,  // MOVE.B  (A0)+,D0
	
	0x66FA,  // BNE.S *-4
	
	0x4E72,  // STOP #FFFF  ; finish
	0xFFFF
};

static void load_vectors( uint8_t* mem )
{
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( stack_address );  // isp
	vectors[1] = big_longword( code_address  );  // pc
}

static void load_data( uint8_t* mem )
{
	strcpy( (char*) mem + data_address, "Hello world\n" );
}

static void load_code( uint8_t* mem )
{
	uint16_t* code = (uint16_t*) (mem + code_address);
	
	for ( int i = 0;  i < sizeof program / sizeof program[0];  ++i )
	{
		*code++ = big_word( program[i] );
	}
}

static void emulator_test()
{
	uint8_t mem[ 4096 ];
	
	load_vectors( mem );
	load_data   ( mem );
	load_code   ( mem );
	
	v68k::emulator emu( v68k::mc68000, mem, sizeof mem );
	
	emu.reset();
	
step_loop:
	
	while ( emu.step() )
	{
		continue;
	}
	
	if ( emu.condition == v68k::bkpt_0 )
	{
		const uint32_t c = emu.regs.d[0];
		
		if ( c <= 0x7F )
		{
			putchar( c );
			
			emu.acknowledge_breakpoint( 0x4E71 );  // NOP
		}
		
		goto step_loop;
	}
	
	if ( emu.condition == v68k::finished )
	{
		return;
	}
	
	putchar( '\n' );
	
	const char* condition;
	
	switch ( emu.condition )
	{
		using namespace v68k;
		
		case halted:
			condition = "halted";
			break;
		
		case stopped:
			condition = "stopped";
			break;
		
		case bkpt_0:
		case bkpt_1:
		case bkpt_2:
		case bkpt_3:
		case bkpt_4:
		case bkpt_5:
		case bkpt_6:
		case bkpt_7:
			condition = NULL;
			
			printf( "Breakpoint %d\n", emu.condition - bkpt_0 );
			break;
		
		default:
			condition = "???";
			break;
	}
	
	if ( condition )
	{
		printf( "Processor %s\n", condition );
	}
	
	dump( emu );
}

int main( int argc, char** argv )
{
	emulator_test();
	
	return 0;
}

