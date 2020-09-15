/*
	print_registers.hh
	------------------
*/

#ifndef PRINTREGISTERS_HH
#define PRINTREGISTERS_HH

// Standard C
#include <stdint.h>


struct registers
{
	uint32_t d[8];  // D0-D7
	uint32_t a[8];  // A0-A7
	
	uint16_t sr;
	uint32_t pc;
	uint16_t fv;
	uint32_t pc0;
};

void print_registers( const uint32_t  d[],
                      const uint32_t  a[],
                      uint32_t        pc,
                      uint16_t        sr );

void print_registers( const registers& regs );

#endif
