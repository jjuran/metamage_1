/*
	print_register_dump.hh
	----------------------
*/

#ifndef IOUTILS_PRINTREGISTERDUMP_HH
#define IOUTILS_PRINTREGISTERDUMP_HH

// Standard C
#include <stdint.h>


namespace v68k    {
namespace ioutils {

void print_register_dump( const uint32_t  d[],
                          const uint32_t  a[],
                          uint32_t        usp,
                          uint32_t        isp,
                          uint32_t        msp,
                          uint32_t        pc,
                          uint16_t        sr );

void print_register_dump( const uint32_t regs[], uint16_t sr );

}  // namespace ioutils
}  // namespace v68k

#endif
