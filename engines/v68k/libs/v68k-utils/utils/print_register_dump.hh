/*
	print_register_dump.hh
	----------------------
*/

#ifndef UTILS_PRINTREGISTERDUMP_HH
#define UTILS_PRINTREGISTERDUMP_HH

// Standard C
#include <stdint.h>


namespace v68k
{
	
	struct registers;
	
}

namespace v68k  {
namespace utils {

void print_register_dump( const uint32_t  d[],
                          const uint32_t  a[],
                          uint32_t        alt_sp,
                          uint32_t        alt_ssp,
                          uint32_t        pc,
                          uint16_t        sr );

void print_register_dump( const registers& regs );

}  // namespace utils
}  // namespace v68k

#endif

