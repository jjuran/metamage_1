/*
	region-sect.cc
	--------------
*/

// quickdraw
#include "qd/regions.hh"
#include "qd/region_detail.hh"
#include "qd/sect_region.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 4 + 3 + 1;


#define ARRAY_LEN( a )  (sizeof a / sizeof a[0])


const short End = quickdraw::Region_end;


static const short rgn_empty[] =
{
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

static const short rgn_2_1_5_16_xor_3_4_8_64[] =
{
	2,  1,    16,      End,
	3,     4,     64,  End,
	5,  1,    16,      End,
	8,     4,     64,  End,
	
	End
};

static const short rgn_3_1_5_16_xor_3_4_8_64[] =
{
	3,  1, 4, 16, 64,  End,
	5,  1,    16,      End,
	8,     4,     64,  End,
	
	End
};

static const short rgn_2_1_5_16_xor_3_4_5_64[] =
{
	2,  1,    16,      End,
	3,     4,     64,  End,
	5,  1, 4, 16, 64,  End,
	
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

static const short rgn_3_1_5_16_xor_3_1_8_64[] =
{
	3,        16, 64,  End,
	
	5,  1,    16,      End,
	8,  1,        64,  End,
	
	End
};

static const short rgn_2_3_5_16_xor_2_3_8_64[] =
{
	2,        16, 64,  End,
	
	5,  3,    16,      End,
	8,  3,        64,  End,
	
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

static const short rect_2_1_5_64[] = { 2, 1, 5, 64 };
static const short rect_2_1_8_64[] = { 2, 1, 8, 64 };
static const short rect_2_3_8_64[] = { 2, 3, 8, 64 };
static const short rect_3_1_8_64[] = { 3, 1, 8, 64 };

#define rgn_2_3_8_64_sect_2_1_5_16_xor_2_1_8_64 rgn_2_3_5_16_xor_2_3_8_64
#define rgn_3_1_8_64_sect_2_1_5_16_xor_2_1_8_64 rgn_3_1_5_16_xor_3_1_8_64
#define rgn_3_1_8_64_sect_2_1_5_16_xor_3_4_8_64 rgn_3_1_5_16_xor_3_4_8_64
#define rgn_2_1_5_64_sect_2_1_5_16_xor_3_4_8_64 rgn_2_1_5_16_xor_3_4_5_64

#define rgn_2_1_3_4_xor_5_16_8_64_sect_2_1_3_4_xor_5_16_8_64 rgn_2_1_3_4_xor_5_16_8_64
#define rgn_2_1_3_4_xor_2_1_8_64_sect_2_1_3_4_xor_2_1_5_16_xor_2_1_8_64 rgn_2_1_5_16_xor_2_1_8_64
#define rgn_2_1_3_4_xor_2_1_5_16_sect_2_1_3_4_xor_2_1_5_16_xor_2_1_8_64 rgn_empty

struct rgn_test_case
{
	const short*  a;
	const short*  b;
	const short*  c;
	unsigned      c_len;
};

#define SECT_RECT( a, b )  { rect ## a, rgn ## b, rgn ## a ## _sect ## b, sizeof rgn ## a ## _sect ## b }
#define SECT( a, b )       { rgn  ## a, rgn ## b, rgn ## a ## _sect ## b, sizeof rgn ## a ## _sect ## b }

static const rgn_test_case sect_rect_tests[] =
{
	SECT_RECT( _2_3_8_64,  _2_1_5_16_xor_2_1_8_64 ),
	SECT_RECT( _3_1_8_64,  _2_1_5_16_xor_2_1_8_64 ),
	SECT_RECT( _3_1_8_64,  _2_1_5_16_xor_3_4_8_64 ),
	SECT_RECT( _2_1_5_64,  _2_1_5_16_xor_3_4_8_64 ),
};

static const rgn_test_case sect_rgns_tests[] =
{
	SECT( _2_1_3_4_xor_5_16_8_64,  _2_1_3_4_xor_5_16_8_64 ),
	SECT( _2_1_3_4_xor_2_1_8_64,  _2_1_3_4_xor_2_1_5_16_xor_2_1_8_64   ),
	SECT( _2_1_3_4_xor_2_1_5_16,  _2_1_3_4_xor_2_1_5_16_xor_2_1_8_64   ),
};

static void sect_rect_rgn()
{
	for ( int i = 0;  i < ARRAY_LEN( sect_rect_tests );  ++i )
	{
		short r[ 64 ] = { 0 };  // more than enough
		
		const rgn_test_case& test = sect_rect_tests[ i ];
		
		short bbox[ 4 ];
		
		quickdraw::set_region_bbox( bbox, test.b );
		
		quickdraw::sect_rect_region( test.a, bbox, test.b, sizeof r, r );
		
		EXPECT_CMP( r, test.c_len, test.c, test.c_len );
	}
}

static void sect_rgns()
{
	for ( int i = 0;  i < ARRAY_LEN( sect_rgns_tests );  ++i )
	{
		short r[ 64 ] = { 0 };  // more than enough
		
		const rgn_test_case& test = sect_rgns_tests[ i ];
		
		const size_t max = sizeof r;
		
		quickdraw::sect_regions( rect_2_1_8_64, test.a, max, test.b, max, r );
		
		EXPECT_CMP( r, test.c_len, test.c, test.c_len );
	}
}

static void complex()
{
	const short a_data[] =
	{
		0x002C,
		0x0078, 0x002D, 0x0123, 0x00F8,
		0x0078, 0x002D, 0x00F7, 0x7FFF,
		0x0079, 0x00F7, 0x00F8, 0x7FFF,
		0x0122, 0x002D, 0x002E, 0x7FFF,
		0x0123, 0x002E, 0x00F8, 0x7FFF,
		0x7FFF
	};
	
	const short b_data[] =
	{
		0x002C,
		0x0078, 0x002D, 0x0123, 0x00F8,
		0x0078, 0x002D, 0x0082, 0x7FFF,
		0x00D7, 0x0082, 0x0083, 0x7FFF,
		0x00D8, 0x0083, 0x00F8, 0x7FFF,
		0x0123, 0x002D, 0x00F8, 0x7FFF,
		0x7FFF
	};
	
	const short c_data[] =
	{
		0x0034,
		0x0078, 0x002D, 0x0123, 0x00F8,
		0x0078, 0x002D, 0x0082, 0x7FFF,
		0x00D7, 0x0082, 0x0083, 0x7FFF,
		0x00D8, 0x0083, 0x00F8, 0x7FFF,
		0x0122, 0x002D, 0x002E, 0x7FFF,
		0x0123, 0x002E, 0x00F8, 0x7FFF,
		0x7FFF
	};
	
	short result[ 0x0034 / 2 ];
	
	using quickdraw::sect_regions;
	
	sect_regions( a_data + 1,
	              a_data + 5,
	              sizeof a_data,
	              b_data + 5,
	              sizeof b_data,
	              result );
	
	const unsigned len = c_data[ 0 ] - 5 * sizeof (short);
	
	EXPECT_CMP( result, len, c_data + 5, len );
}


int main( int argc, char** argv )
{
	tap::start( "region-sect", n_tests );
	
	sect_rect_rgn();
	sect_rgns();
	
	complex();
	
	return 0;
}
