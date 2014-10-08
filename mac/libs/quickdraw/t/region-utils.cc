/*
	region-utils.cc
	---------------
*/

// quickdraw
#include "qd/regions.hh"
#include "qd/region_detail.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 4 + 2;


typedef quickdraw::region_geometry_t  geometry_t;


const short End = quickdraw::Region_end;


static void geometry()
{
	geometry_t g;
	
	const short disjoint[] =
	{
		2,  1, 4,          End,
		3,  1, 4,          End,
		5,        16, 64,  End,
		8,        16, 64,  End,
		
		End
	};
	
	g = quickdraw::region_geometry( disjoint );
	
	EXPECT( g.n_v_coords == 4 );
	EXPECT( g.n_h_coords == 2 );
	
	const short tangent[] =
	{
		2,  1, 4,          End,
		3,  1, 4, 16, 64,  End,
		5,        16, 64,  End,
		
		End
	};
	
	g = quickdraw::region_geometry( tangent );
	
	EXPECT( g.n_v_coords == 3 );
	EXPECT( g.n_h_coords == 4 );
}

static void bbox()
{
	short bbox[ 4 ];
	
	const short dexter[] =
	{
		2, 1, 8, 64,
		
		2,  1, 4,          End,
		3,  1, 4,          End,
		5,        16, 64,  End,
		8,        16, 64,  End,
		
		End
	};
	
	quickdraw::set_region_bbox( bbox, dexter + 4 );
	
	EXPECT_CMP( &bbox, sizeof bbox, dexter, sizeof bbox );
	
	const short sinister[] =
	{
		2, 1, 8, 64,
		
		2,        16, 64,  End,
		3,        16, 64,  End,
		5,  1, 4,          End,
		8,  1, 4,          End,
		
		End
	};
	
	quickdraw::set_region_bbox( bbox, sinister + 4 );
	
	EXPECT_CMP( &bbox, sizeof bbox, sinister, sizeof bbox );
}


int main( int argc, char** argv )
{
	tap::start( "region-utils", n_tests );
	
	geometry();
	
	bbox();
	
	return 0;
}
