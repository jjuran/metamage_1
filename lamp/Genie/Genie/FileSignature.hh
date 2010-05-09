/*	======================
 *	Genie/FileSignature.hh
 *	======================
 */

#ifndef GENIE_FILESIGNATURE_HH
#define GENIE_FILESIGNATURE_HH

// Nitrogen
#include "Nitrogen/Files.hh"


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	Nitrogen::OSType TextFileCreator();
	
	Nitrogen::FileSignature PickFileSignatureForName( const plus::string& name );
	
}

#endif

