/*
	Mac/Files/Types/FSSignature.hh
	------------------------------
*/

#ifndef MAC_FILES_TYPES_FSSIGNATURE_HH
#define MAC_FILES_TYPES_FSSIGNATURE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSCREATOR_HH
#include "Mac/Files/Types/FSCreator.hh"
#endif
#ifndef MAC_FILES_TYPES_FSTYPE_HH
#include "Mac/Files/Types/FSType.hh"
#endif


namespace Mac
{
	
	struct FSSignature
	{
		FSType     type;
		FSCreator  creator;
		
		FSSignature() : type(), creator()
		{
		}
		
		FSSignature( FSType     t,
		             FSCreator  c ) : type   ( t ),
		                              creator( c )
		{
		}
		
		FSSignature( const FInfo& fInfo ) : type   ( FSType   ( fInfo.fdType    ) ),
		                                    creator( FSCreator( fInfo.fdCreator ) )
		{
		}
	};
	
}

#endif
