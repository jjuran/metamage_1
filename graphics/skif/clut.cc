/*
	clut.cc
	-------
*/

#include "clut.hh"


void load_default_clut( raster::clut_data& clut, short n_colors )
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
		clut.palette[ i ].value = 0;
		clut.palette[ i ].red   = intensity;
		clut.palette[ i ].green = intensity;
		clut.palette[ i ].blue  = intensity;
		
		intensity += increment;
	}
}
