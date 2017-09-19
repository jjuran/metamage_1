/*
	QDUtils.cc
	----------
*/

#include "QDUtils.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// ams-common
#include "QDGlobals.hh"


pascal short Random_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	qd.randSeed = qd.randSeed * 16807 % 0x7FFFFFFF;
	
	return qd.randSeed;
}

static const char hex_table[ 32 ] =
{
	0,  10, 11, 12, 13, 14, 15, 0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  1,  2,  3,  4,  5,  6,  7,
	8,  9,  0   // more zeros follow
};

const char xdigit_mask = 0x1F;

static inline 
unsigned char unhex( char xdigit )
{
	return hex_table[ xdigit & xdigit_mask ];
}

pascal void StuffHex_patch( char* dst, const unsigned char* srcHex )
{
	uint8_t len = *srcHex++;
	
	unsigned char c;
	
	while ( (len -= 2) > 0 )
	{
		c  = unhex( *srcHex++ ) << 4;
		c |= unhex( *srcHex++ );
		
		*dst++ = c;
	}
}

pascal PatHandle GetPattern_patch( short id )
{
	return (PatHandle) GetResource( 'PAT ', id );
}

pascal CursHandle GetCursor_patch( short id )
{
	return (CursHandle) GetResource( 'CURS', id );
}
