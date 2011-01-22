/*
	CreateAlias.hh
	--------------
*/

#ifndef GENIE_UTILITIES_CREATEALIAS_HH
#define GENIE_UTILITIES_CREATEALIAS_HH

// Mac OS
//#include <Files.h>
struct FSSpec;


namespace Genie
{
	
	void CreateAlias( const FSSpec& aliasFile, const FSSpec& target );
	
}

#endif

