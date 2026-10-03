/*
	screen_spec.cc
	--------------
*/

#include "frend/screen_spec.hh"

// plus
#include "plus/string.hh"

// frontend-common
#include "frend/load_file.hh"


namespace frend
{

raster_spec screen_spec;

long load_screen_spec( int bindir_fd )
{
	const char* spec_path = "../Resources/screen.txt";
	
	const plus::string data = load_file( bindir_fd, spec_path );
	
	return parse_raster_spec( screen_spec, data.c_str() );
}

}
