/*
	v68k-test.cc
	------------
*/

// Standard C
#include <stdio.h>

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
	
	uint16_t sr = regs.ttsm << 12
	            | regs. iii <<  8
	            | regs.   x <<  4
	            | regs.nzvc <<  0;
	
	printf( "\n" );
	
	printf( "Alt SP: %.8x\n", regs.alt_sp );
	
	printf( "\n" );
	
	printf( "PC: %.8x\n", regs.pc );
	printf( "SR: %.4x\n", sr );
}


const uint32_t data_address  = 1024;
const uint32_t stack_address = 2048;
const uint32_t code_address  = 2048;

static void load_vectors( uint8_t* mem )
{
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( stack_address );  // isp
	vectors[1] = big_longword( code_address  );  // pc
}

static void load_data( uint8_t* mem )
{
	uint32_t* data = (uint32_t*) (mem + data_address);
	
	data[0] = big_longword( 0xFF000000 );
	data[1] = big_longword( 0x0000FF00 );
	
	data[2] = big_longword( 0x00000F00 );
	data[3] = big_longword( 0xF0000000 );
}

static void load_code( uint8_t* mem )
{
	uint16_t* code = (uint16_t*) (mem + code_address);
	
	int i = 0;
	
	code[ i++ ] = big_word( 0x014F );  // MOVEP.L  (-1016,A7),D0
	code[ i++ ] = big_word( 0xFC08 );
	
	code[ i++ ] = big_word( 0xC189 );  // EXG  D0,A1
	
	code[ i++ ] = big_word( 0x4e61 );  // MOVE A1,USP
	
	code[ i++ ] = big_word( 0x4e6A );  // MOVE USP,A2
	
	code[ i++ ] = big_word( 0x4e6B );  // MOVE USP,A3
	
	code[ i++ ] = big_word( 0x4e6C );  // MOVE USP,A4
	
	code[ i++ ] = big_word( 0x4e6D );  // MOVE USP,A5
	
	code[ i++ ] = big_word( 0x4e6E );  // MOVE USP,A6
	
	code[ i++ ] = big_word( 0x014F );  // MOVEP.L  (-1024,A7),D0
	code[ i++ ] = big_word( 0xFC00 );
	
	code[ i++ ] = big_word( 0x034F );  // MOVEP.L  (-1024,A7),D1
	code[ i++ ] = big_word( 0xFC00 );
	
	code[ i++ ] = big_word( 0x054F );  // MOVEP.L  (-1024,A7),D2
	code[ i++ ] = big_word( 0xFC00 );
	
	code[ i++ ] = big_word( 0x0b4F );  // MOVEP.L  (-1024,A7),D5
	code[ i++ ] = big_word( 0xFC00 );
	
	code[ i++ ] = big_word( 0x0d4F );  // MOVEP.L  (-1024,A7),D6
	code[ i++ ] = big_word( 0xFC00 );
	
	code[ i++ ] = big_word( 0x0f4F );  // MOVEP.L  (-1024,A7),D7
	code[ i++ ] = big_word( 0xFC00 );
	
	code[ i++ ] = big_word( 0x76FF );  // MOVEQ  #FF,D3
	code[ i++ ] = big_word( 0x78FF );  // MOVEQ  #FF,D4
	
	code[ i++ ] = big_word( 0xC788 );  // EXG  D3,A0
	code[ i++ ] = big_word( 0xC98F );  // EXG  D4,A7
	
	code[ i++ ] = big_word( 0x76FF );  // MOVEQ  #FF,D3
	code[ i++ ] = big_word( 0x78FF );  // MOVEQ  #FF,D4
	
	code[ i++ ] = big_word( 0x014F );  // MOVEP.L  (0,A7),D0
	code[ i++ ] = big_word( 0x0000 );
}

static void emulator_test()
{
	uint8_t mem[ 4096 ];
	
	load_vectors( mem );
	load_data   ( mem );
	load_code   ( mem );
	
	v68k::emulator emu( v68k::mc68000, mem, sizeof mem );
	
	emu.reset();
	
	while ( emu.step() )
	{
		continue;
	}
	
	dump( emu );
}

int main( int argc, char** argv )
{
	emulator_test();
	
	return 0;
}

