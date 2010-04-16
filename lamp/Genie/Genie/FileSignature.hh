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


namespace Genie
{
	
	extern ::OSType gTextFileCreator;
	
	inline Mac::FSCreator TextFileCreator()
	{
		return Mac::FSCreator( gTextFileCreator );
	}
	
	Mac::FSSignature PickFileSignatureForName( const char*  name,
	                                           unsigned     length );
	
}

#endif

