/*
	UnpackBits.mac.cc
	-----------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 3 * 10;


struct test_case
{
	unsigned char*  src;
	unsigned char*  dst;
};

static
void run_test( Ptr origin, Size src_len, Ptr target, Size dst_len )
{
	char buffer[ 256 ];
	
	Ptr src = origin;
	Ptr dst = buffer;
	
	UnpackBits( &src, &dst, dst_len );
	
	EXPECT_EQ( src - origin, src_len );
	EXPECT_EQ( dst - buffer, dst_len );
	
	EXPECT_CMP( buffer, dst - buffer, target, dst_len );
}

static inline
void run_test( const test_case& test )
{
	Ptr origin = (Ptr) test.src + 1;
	Ptr target = (Ptr) test.dst + 1;
	
	run_test( origin, test.src[ 0 ],
	          target, test.dst[ 0 ] );
}

#define X5  "XXXXX"

#define  X25  X5  X5  X5  X5  X5
#define X125 X25 X25 X25 X25 X25

#define S127 "(" X125 ")"

static const test_case test_cases[] =
{
	{ "\p", "\p" },
	{ "\p\x00-", "\p-" },
//	{ "\p\x03abcd", "\pabcd" },  // "illegal character constant"!?
	{ "\p\x03" "abcd", "\pabcd" },
	{ "\p\x7C"  X125, "\p"  X125 },  // 124 -> 125
	{ "\p\x7D:" X125, "\p:" X125 },  // 125 -> 126
	{ "\p\x7E"  S127, "\p"  S127 },  // 126 -> 127
	{ "\p\x7F:" S127, "\p:" S127 },  // 127 -> 128
	{ "\p\xFF=", "\p==" },   // -1 -> 2
	{ "\p\xFE.", "\p..." },  // -2 -> 3
	{ "\p\x82X", "\pXX" X125 },  // -126 -> 127
//	{ "\p\x81X", "\pXXX" X125 },  // -127 -> 128?  (In Mac OS, 127)
};

static
void unpack()
{
	for ( int i = 0;  i < sizeof test_cases / sizeof *test_cases;  ++i )
	{
		run_test( test_cases[ i ] );
	}
}

int main( int argc, char** argv )
{
	tap::start( "UnpackBits.mac", n_tests );
	
	unpack();
	
	return 0;
}
