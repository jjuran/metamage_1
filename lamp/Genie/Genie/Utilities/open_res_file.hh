/*
	open_res_file.hh
	----------------
*/

#ifndef GENIE_UTILITIES_OPENRESFILE_HH
#define GENIE_UTILITIES_OPENRESFILE_HH


struct FSSpec;


namespace Genie
{
	
	typedef short ResFileRefNum;
	
	ResFileRefNum open_res_file( const FSSpec& file, signed char perm );
	
}

#endif
