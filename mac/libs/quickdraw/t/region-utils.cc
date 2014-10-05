/*
	region-utils.cc
	---------------
*/

// quickdraw
#include "qd/regions.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 4;


typedef quickdraw::region_geometry_t  geometry_t;


static void geometry()
{
	geometry_t g;
	
	const short disjoint[] =
	{
		2,  1, 4,          0x7FFF,
		3,  1, 4,          0x7FFF,
		5,        16, 64,  0x7FFF,
		8,        16, 64,  0x7FFF,
		
		0x7FFF
	};
	
	g = quickdraw::region_geometry( disjoint );
	
	EXPECT( g.n_v_coords == 4 );
	EXPECT( g.n_h_coords == 2 );
	
	const short tangent[] =
	{
		2,  1, 4,          0x7FFF,
		3,  1, 4, 16, 64,  0x7FFF,
		5,        16, 64,  0x7FFF,
		
		0x7FFF
	};
	
	g = quickdraw::region_geometry( tangent );
	
	EXPECT( g.n_v_coords == 3 );
	EXPECT( g.n_h_coords == 4 );
}


int main( int argc, char** argv )
{
	tap::start( "region-utils", n_tests );
	
	geometry();
	
	return 0;
}
