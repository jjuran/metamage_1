/*	==============
 *	Subprojects.hh
 *	==============
 */

#ifndef COMPILEDRIVER_SUBPROJECTS_HH
#define COMPILEDRIVER_SUBPROJECTS_HH

// Standard C++
#include <vector>

// Nitrogen
#include "Nitrogen/Files.h"


namespace CompileDriver
{
	
	namespace N = Nitrogen;
	
	void AddSubproject( const N::FSDirSpec& location );
	
	std::vector< N::FSDirSpec >& Subprojects();
	
}

#endif

