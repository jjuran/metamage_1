/*
	region-scanner.cc
	-----------------
*/

// Standard C
#include <string.h>

// quickdraw
#include "qd/region_detail.hh"
#include "qd/region_scanner.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 6;


typedef unsigned short uint16_t;

const short End = quickdraw::Region_end;

using quickdraw::region_scanner;


static void scanner()
{
	uint16_t bits[ 4 ] = { 0 };
	uint16_t temp[ 4 ] = { 0 };
	
	short extent[ 9 ] = { 0 };
	
	const short r1[ 9 ] = { 2, 3, 67, End };
	const short r2[ 9 ] = { 2, 3, 66, End };
	const short r3[ 9 ] = { 2, 3, 66, End, 4, 3, 66, End, End };
	
	{
		region_scanner scanner( extent, temp, sizeof temp );
		
		EXPECT( !scanner.scan( 0, 0, bits, temp ) );
		
		memset( bits, 0xFF, sizeof bits );
		
		EXPECT( scanner.scan( 3, 2, bits, temp ) );
		
		EXPECT_CMP( extent, sizeof extent, r1, sizeof r1 );
	}
	
	{
		region_scanner scanner( extent, temp, sizeof temp );
		
		EXPECT( scanner.scan( 3, 2, bits, temp, 1 ) );
		
		EXPECT_CMP( extent, sizeof extent, r2, sizeof r2 );
		
		scanner.finish( 3, 4, bits, 1 );
		
		EXPECT_CMP( extent, sizeof extent, r3, sizeof r3 );
	}
}


int main( int argc, char** argv )
{
	tap::start( "region-scanner", n_tests );
	
	scanner();
	
	return 0;
}
