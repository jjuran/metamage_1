/*
	print_register_dump.cc
	----------------------
*/

#include "print_register_dump.hh"

// POSIX
#include <unistd.h>

// v68k
#include "v68k/registers.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k  {
namespace utils {


static char hexify( unsigned x )
{
	x &= 0x0F;
	
	return (x <= 9 ? '0' : 'a' - 10) + x;
}

static void inscribe_16( char* p, uint16_t x )
{
	p[0] = hexify( x >> 12 );
	p[1] = hexify( x >>  8 );
	p[2] = hexify( x >>  4 );
	p[3] = hexify( x >>  0 );
}

static void inscribe_32( char* p, uint32_t x )
{
	p[0] = hexify( x >> 28 );
	p[1] = hexify( x >> 24 );
	p[2] = hexify( x >> 20 );
	p[3] = hexify( x >> 16 );
	p[4] = hexify( x >> 12 );
	p[5] = hexify( x >>  8 );
	p[6] = hexify( x >>  4 );
	p[7] = hexify( x >>  0 );
}

#define DUMP_TEMPLATE             "\n" \
	"D0: 12345678   A0: 12345678" "\n" \
	"D1: 12345678   A1: 12345678" "\n" \
	"D2: 12345678   A2: 12345678" "\n" \
	"D3: 12345678   A3: 12345678" "\n" \
	"D4: 12345678   A4: 12345678" "\n" \
	"D5: 12345678   A5: 12345678" "\n" \
	"D6: 12345678   A6: 12345678" "\n" \
	"D7: 12345678   A7: 12345678" "\n" \
	                              "\n" \
	"USP: 12345678"               "\n" \
	"ISP: 12345678"               "\n" \
	"MSP: 12345678"               "\n" \
	                              "\n" \
	"PC:  12345678"               "\n" \
	"SR:  1234"                   "\n" \
	                              "\n"

const size_t dump_size = sizeof DUMP_TEMPLATE - 1;

void print_register_dump( const uint32_t  d[],
                          const uint32_t  a[],
                          uint32_t        usp,
                          uint32_t        isp,
                          uint32_t        msp,
                          uint32_t        pc,
                          uint16_t        sr )
{
	char dump[] = DUMP_TEMPLATE;
	
	char* p = dump;
	
	++p;  // skip newline
	
	for ( int i = 0;  i < 8;  ++i )
	{
		p += sizeof "Di: " - 1;
		
		inscribe_32( p, d[i] );
		
		p += sizeof "12345678   Ai: " - 1;
		
		inscribe_32( p, a[i] );
		
		p += sizeof "12345678" "\n" - 1;
	}
	
	++p;  // skip newline
	
	p += sizeof "USP: " - 1;
	
	inscribe_32( p, usp );
	
	p += sizeof "12345678" "\n" - 1;
	
	p += sizeof "ISP: " - 1;
	
	inscribe_32( p, isp );
	
	p += sizeof "12345678" "\n" - 1;
	
	p += sizeof "MSP: " - 1;
	
	inscribe_32( p, msp );
	
	p += sizeof "12345678" "\n" - 1;
	
	++p;  // skip newline
	
	p += sizeof "PC:  " - 1;
	
	inscribe_32( p, pc );
	
	p += sizeof "12345678" "\n" - 1;
	
	p += sizeof "SR:  " - 1;
	
	inscribe_16( p, sr );
	
	write( STDERR_FILENO, dump, dump_size );
}

void print_register_dump( const uint32_t regs[], uint16_t sr )
{
	print_register_dump( regs + D0,
	                     regs + A0,
	                     regs[ USP ],
	                     regs[ ISP ],
	                     regs[ MSP ],
	                     regs[ PC  ],
	                     sr );
}

}  // namespace utils
}  // namespace v68k
