/*
	...-icongen.cc
	--------------
*/

// icongen
#include "icongen/icongen.hh"

// ...-icongen
#include "icon.hh"


int main( int argc, char** argv )
{
	return icongen_main( argc, argv, &draw_the_icon );
}
