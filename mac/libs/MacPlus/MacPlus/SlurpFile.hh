/*
	SlurpFile.hh
	------------
*/

#ifndef MACPLUS_SLURPFILE_HH
#define MACPLUS_SLURPFILE_HH

// plus
#include "plus/string.hh"

struct FSSpec;


namespace MacPlus
{
	
	plus::string SlurpFile( const FSSpec& file );
	
}

#endif
