/*
	v68k-rot13.cc
	-------------
*/

// Standard C
#include <stdio.h>
#include <string.h>

// v68k
#include "v68k/endian.hh"
#include "v68k/emulator.hh"


#pragma exceptions off


using v68k::big_word;
using v68k::big_longword;


const uint32_t initial_SSP  = 4096;
const uint32_t initial_USP  = 3072;
const uint32_t code_address = 2048;
const uint32_t os_address   = 1024;

static const uint16_t os[] =
{
	0x6006,  // BRA.S  *+8
	
	0x4E73,  // RTE
	
	0x4E72,  // STOP #FFFF  ; finish
	0xFFFF,
	
	// start
	0x027C,  // ANDI #DFFF,SR  ; clear S
	0xDFFF,
	
	0x4FF8,  // LEA  (3072).W,A7
	initial_USP,
	
	0x4EB8,  // JSR  0x0800  ; 2048
	0x0800,
	
	0x4e4F   // TRAP  #15
};

const uint32_t rte_addr    = os_address + 2;
const uint32_t finish_addr = os_address + 4;

static const uint16_t program[] =
{
	0x41FA,  // LEA  *+42,(A0)
	0x0028,
	
	0x4849,  // BKPT  #1  ; read a character
	
	0x2200,  // MOVE.L  D0,D1  ; test for EOF
	
	0x6B1C,  // BMI.S *+30
	
	0x0201,  // ANDI.B  #0x80,D1  ; !0 -> non-letter
	0x0080,
	
	0x6612,  // BNE.S *+20
	
	0x1200,  // MOVE.B  D0,D1
	
	0x0201,  // ANDI.B  #0x40,D1  ; 0 -> non-letter
	0x0040,
	
	0x670A,  // BEQ.S *+12
	
	0x1200,  // MOVE.B  D0,D1
	
	0x0241,  // ANDI.W  #0x003F,D1
	0x003F,
	
	0x1030,  // MOVE.B  (A0,D1),D0
	0x1000,
	
	0x4848,  // BKPT  #0  ; print a character
	
	0x60DE,  // BRA.S *-32
	
	0x4E75,  // RTS
	
	0x0040,  // 64 bytes of data follow
	
	0x404E,
	0x4F50,
	0x5152,
	0x5354,
	0x5556,
	0x5758,
	0x595A,
	0x4142,
	0x4344,
	0x4546,
	0x4748,
	0x494A,
	0x4B4C,
	0x4D5B,
	0x5C5D,
	0x5E5F,
	0x606E,
	0x6F70,
	0x7172,
	0x7374,
	0x7576,
	0x7778,
	0x797A,
	0x6162,
	0x6364,
	0x6566,
	0x6768,
	0x696A,
	0x6B6C,
	0x6D7B,
	0x7C7D,
	0x7E7F,
};

static void load_vectors( uint8_t* mem )
{
	uint32_t* vectors = (uint32_t*) mem;
	
	memset( vectors, 0xFF, 1024 );
	
	vectors[0] = big_longword( initial_SSP );  // isp
	vectors[1] = big_longword( os_address  );  // pc
	
	vectors[47] = big_longword( finish_addr );  // Trap 15
	
	vectors[64] = big_longword( rte_addr );  // Vector 64 (user-defined)
}

static void load_code( uint16_t*        dest,
                       const uint16_t*  begin,
                       const uint16_t*  end )
{
	while ( begin < end )
	{
		*dest++ = big_word( *begin++ );
	}
}

static inline void load_n_words( uint8_t*         mem,
                                 uint32_t         offset,
                                 const uint16_t*  begin,
                                 size_t           n_words )
{
	uint16_t* dest = (uint16_t*) (mem + offset);
	
	load_code( dest, begin, begin + n_words );
}

static int emulator_rot13()
{
	uint8_t mem[ 4096 ];
	
	load_vectors( mem );
	load_n_words( mem, code_address, program, sizeof program / 2 );
	load_n_words( mem, os_address,   os,      sizeof os      / 2 );
	
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
		
		putchar( c );
		
		emu.acknowledge_breakpoint( 0x4E71 );  // NOP
		
		goto step_loop;
	}
	else if ( emu.condition == v68k::bkpt_1 )
	{
		int32_t c = getchar();
		
		emu.regs.d[0] = c;
		
		emu.acknowledge_breakpoint( 0x4E71 );  // NOP
		
		goto step_loop;
	}
	
	return emu.condition == v68k::finished ? 0 : 1;
}

int main( int argc, char** argv )
{
	return emulator_rot13();
}

