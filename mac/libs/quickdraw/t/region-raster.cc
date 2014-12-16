/*
	region-raster.cc
	----------------
*/

// quickdraw
#include "qd/region_detail.hh"
#include "qd/region_raster.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 5 + 16 + 12;


#define ARRAYLEN( a )  (sizeof a / sizeof a[0])

#define ARRAY_LEN( a ) (a), ARRAYLEN( a )


const short End = quickdraw::Region_end;

using quickdraw::region_raster;


struct test_case
{
	short     bbox[ 4 ];
	unsigned  mask_size;
};

static const test_case test_cases[] =
{
	{ { 2,  1, 8, 32 }, 4 },
	{ { 2,  1, 8, 33 }, 5 },
	{ { 2,  0, 8, 33 }, 5 },
	{ { 2, -1, 8, 33 }, 6 },
	{ { 2, -32767, 8, 32767 }, 8192 },
};

struct result_set
{
	unsigned char mask[ 8 ];
};

static void mask_sizes()
{
	for ( int i = 0;  i < ARRAYLEN( test_cases );  ++i )
	{
		const test_case& t = test_cases[ i ];
		
		EXPECT( region_raster::mask_size( t.bbox ) == t.mask_size );
	}
}

static void run_test_case( const test_case&   test,
                           const short*       extent,
                           const result_set*  result,
                           unsigned           n )
{
	const unsigned mask_size = region_raster::mask_size( test.bbox );
	
	void* mask = ::operator new( mask_size );
	
	region_raster raster( test.bbox, extent, mask, mask_size );
	
	for ( int i = 0;  i < n;  ++i )
	{
		raster.load_mask( i * i );
		
		EXPECT_CMP( mask, mask_size, result[ i ].mask, mask_size );
	}
	
	::operator delete( mask );
}

static void disjoint()
{
	const short extent[] =
	{
		0,  1, 4,          End,
		1,  1, 4,          End,
		4,        16, 32,  End,
		9,        16, 32,  End,
		
		End
	};
	
	const result_set result_1[] = 
	{
		{ 0x70 },
		{ 0 },
		{ 0x00, 0x00, 0xFF, 0xFF },
		{ 0 },
	};
	
	const result_set result_2[] = 
	{
		{ 0x00, 0x70 },
		{ 0 },
		{ 0x00, 0x00, 0x00, 0xFF, 0xFF },
		{ 0 },
	};
	
	run_test_case( test_cases[ 0 ], extent, ARRAY_LEN( result_1 ) );
	run_test_case( test_cases[ 1 ], extent, ARRAY_LEN( result_1 ) );
	run_test_case( test_cases[ 2 ], extent, ARRAY_LEN( result_1 ) );
	run_test_case( test_cases[ 3 ], extent, ARRAY_LEN( result_2 ) );
}

static void tangent()
{
	const short bbox[] = { 2, 1, 5, 32 };
	
	const short extent[] =
	{
		0,  1, 4,          End,
		1,  1, 4, 16, 32,  End,
		4,        16, 32,  End,
		
		End
	};
	
	const result_set result_1[] = 
	{
		{ 0x70 },
		{ 0x00, 0x00, 0xFF, 0xFF },
		{ 0 },
	};
	
	const result_set result_2[] = 
	{
		{ 0x00, 0x70 },
		{ 0x00, 0x00, 0x00, 0xFF, 0xFF },
		{ 0 },
	};
	
	run_test_case( test_cases[ 0 ], extent, ARRAY_LEN( result_1 ) );
	run_test_case( test_cases[ 1 ], extent, ARRAY_LEN( result_1 ) );
	run_test_case( test_cases[ 2 ], extent, ARRAY_LEN( result_1 ) );
	run_test_case( test_cases[ 3 ], extent, ARRAY_LEN( result_2 ) );
}


int main( int argc, char** argv )
{
	tap::start( "region-raster", n_tests );
	
	mask_sizes();
	
	disjoint();
	tangent();
	
	return 0;
}
