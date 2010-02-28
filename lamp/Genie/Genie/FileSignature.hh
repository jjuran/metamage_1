/*	======================
 *	Genie/FileSignature.hh
 *	======================
 */

#ifndef GENIE_FILESIGNATURE_HH
#define GENIE_FILESIGNATURE_HH

// Standard C++
#include <string>

// Nitrogen
#include "Nitrogen/Files.hh"


namespace Genie
{
	
	Nitrogen::OSType TextFileCreator();
	
	Nitrogen::FileSignature PickFileSignatureForName( const std::string& name );
	
}

#endif

