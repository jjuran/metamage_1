/*	======================
 *	Genie/FileSignature.hh
 *	======================
 */

#ifndef GENIE_FILESIGNATURE_HH
#define GENIE_FILESIGNATURE_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSSIGNATURE_HH
#include "Mac/Files/Types/FSSignature.hh"
#endif

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
	
	inline Mac::FSCreator TextFileCreator()
	{
		return Mac::FSCreator( gTextFileCreator );
	}
	
	Mac::FSSignature PickFileSignatureForName( const plus::string& name );
	
}

#endif

