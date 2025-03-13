/*
	default_clut.cc
	---------------
*/

#include "raster/default_clut.hh"

// rasterlib
#include "raster/clut_detail.hh"


namespace raster
{

void load_default_clut( clut_data& clut, short n_colors )
{
	clut.seed  = 0;
	clut.flags = 0;
	clut.max   = n_colors - 1;
	
	// If these are uint16_t instead, MWC68K pessimizes the code.
	
	int increment = n_colors ==  2 ? 0xFFFF
	              : n_colors ==  4 ? 0x5555
	              : n_colors == 16 ? 0x1111
	              : /* 256 colors */ 0x0101;
	
	int intensity = 0;
	
	for ( int i = 0;  i < n_colors;  ++i )
	{
		/*
			Spike the intensity values with a "seasoning" that
			will make any endianness errors painfully obvious.
		*/
		
		clut.palette[ i ].value = 0;
		clut.palette[ i ].red   = intensity ^ 0x1040;
		clut.palette[ i ].green = intensity ^ 0x0820;
		clut.palette[ i ].blue  = intensity ^ 0x2080;
		
		intensity += increment;
	}
	
	if ( n_colors == 2 )
	{
		/*
			Override the 1-bit case to make it more colorful.
		*/
		
		clut.palette[ 0 ].red   = 0x0000 ^ 0x2040;
		clut.palette[ 0 ].green = 0x0000 ^ 0x0020;
		clut.palette[ 0 ].blue  = 0x0000 ^ 0x4080;
		
		clut.palette[ 1 ].red   = 0xFFFF ^ 0x2040;
		clut.palette[ 1 ].green = 0xFFFF ^ 0x4020;
		clut.palette[ 1 ].blue  = 0xFFFF ^ 0x0080;
	}
}

}
