/*	======================
 *	Genie/FileSignature.hh
 *	======================
 */

#ifndef GENIE_FILESIGNATURE_HH
#define GENIE_FILESIGNATURE_HH

// Standard C++
#include <string>

// Nitrogen
#include "Nitrogen/Files.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	N::OSType TextFileCreator();
	
	N::FileSignature PickFileSignatureForName( const std::string& name );
	
}

#endif

