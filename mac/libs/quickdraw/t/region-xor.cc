/*
	region-xor.cc
	-------------
*/

// quickdraw
#include "qd/region_detail.hh"
#include "qd/xor_region.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 10;


#define ARRAY_LEN( a )  (sizeof a / sizeof a[0])


const short End = quickdraw::Region_end;


static const short rgn_empty[] =
{
	End
};

static const short rgn_2_1_3_4[] =
{
	2,  1, 4,  End,
	3,  1, 4,  End,
	
	End
};

static const short rgn_3_1_5_4[] =
{
	3,  1, 4,  End,
	5,  1, 4,  End,
	
	End
};

static const short rgn_2_1_5_4[] =
{
	2,  1, 4,  End,
	5,  1, 4,  End,
	
	End
};

static const short rgn_2_1_8_64[] =
{
	2,  1, 64,  End,
	8,  1, 64,  End,
	
	End
};

static const short rgn_3_4_5_16[] =
{
	3,  4, 16,  End,
	5,  4, 16,  End,
	
	End
};

static const short rgn_5_16_8_64[] =
{
	5,  16, 64,  End,
	8,  16, 64,  End,
	
	End
};

static const short rgn_2_1_5_16[] =
{
	2,  1, 16,  End,
	5,  1, 16,  End,
	
	End
};

static const short rgn_3_4_8_64[] =
{
	3,  4, 64,  End,
	8,  4, 64,  End,
	
	End
};

static const short rgn_2_1_3_4_xor_5_16_8_64[] =
{
	2,  1, 4,          End,
	3,  1, 4,          End,
	5,        16, 64,  End,
	8,        16, 64,  End,
	
	End
};

static const short rgn_3_1_5_4_xor_5_16_8_64[] =
{
	3,  1, 4,          End,
	5,  1, 4, 16, 64,  End,
	8,        16, 64,  End,
	
	End
};

static const short rgn_2_1_8_64_xor_3_4_5_16[] =
{
	2,  1,        64,  End,
	3,     4, 16,      End,
	5,     4, 16,      End,
	8,  1,        64,  End,
	
	End
};

static const short rgn_2_1_5_16_xor_3_4_8_64[] =
{
	2,  1,    16,      End,
	3,     4,     64,  End,
	5,  1,    16,      End,
	8,     4,     64,  End,
	
	End
};

static const short rgn_2_1_3_4_xor_2_1_5_16[] =
{
	2,     4, 16,  End,
	3,  1, 4,      End,
	5,  1,    16,  End,
	
	End
};

static const short rgn_2_1_3_4_xor_2_1_8_64[] =
{
	2,     4,     64,  End,
	3,  1, 4,          End,
	
	8,  1,        64,  End,
	
	End
};

static const short rgn_2_1_5_16_xor_2_1_8_64[] =
{
	2,        16, 64,  End,
	
	5,  1,    16,      End,
	8,  1,        64,  End,
	
	End
};

static const short rgn_2_1_3_4_xor_2_1_5_16_xor_2_1_8_64[] =
{
	2,  1, 4, 16, 64,  End,
	3,  1, 4,          End,
	5,  1,    16,      End,
	8,  1,        64,  End,
	
	End
};

#define rgn_2_1_3_4_xor_3_1_5_4 rgn_2_1_5_4
#define rgn_2_1_3_4_xor_2_1_3_4 rgn_empty

struct rgn_test_case
{
	const short*  a;
	const short*  b;
	const short*  c;
	unsigned      c_len;
};

#define XOR( a, b )  { rgn ## a, rgn ## b, rgn ## a ## _xor ## b, sizeof rgn ## a ## _xor ## b }

static const rgn_test_case xor_tests[] =
{
	XOR( _2_1_3_4,  _5_16_8_64 ),
	XOR( _3_1_5_4,  _5_16_8_64 ),
	XOR( _2_1_3_4,  _3_1_5_4   ),
	XOR( _2_1_3_4,  _2_1_3_4   ),
	XOR( _2_1_8_64, _3_4_5_16  ),
	XOR( _2_1_5_16, _3_4_8_64  ),
	XOR( _2_1_3_4,  _2_1_5_16  ),
	XOR( _2_1_3_4,  _2_1_8_64  ),
	XOR( _2_1_5_16, _2_1_8_64  ),
	
	XOR( _2_1_3_4, _2_1_5_16_xor_2_1_8_64  ),
};

static void xor_rgn()
{
	for ( int i = 0;  i < ARRAY_LEN( xor_tests );  ++i )
	{
		short r[ 64 ] = { 0 };  // more than enough
		
		const rgn_test_case& test = xor_tests[ i ];
		
		quickdraw::xor_region( test.a, test.b, r );
		
		EXPECT_CMP( r, test.c_len, test.c, test.c_len );
	}
}


int main( int argc, char** argv )
{
	tap::start( "region-xor", n_tests );
	
	xor_rgn();
	
	return 0;
}
