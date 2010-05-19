/*	======================
 *	Genie/FileSignature.hh
 *	======================
 */

#ifndef GENIE_FILESIGNATURE_HH
#define GENIE_FILESIGNATURE_HH

// Nitrogen
#include "Mac/Files/Types/FSSignature.hh"

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	extern ::OSType gTextFileCreator;
	
	inline Nitrogen::OSType TextFileCreator()
	{
		return Nitrogen::OSType( gTextFileCreator );
	}
	
	Mac::FSSignature PickFileSignatureForName( const plus::string& name );
	
}

#endif

