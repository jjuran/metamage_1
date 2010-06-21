/*
	Mac/Files/Types/FSSignature.hh
	------------------------------
*/

#ifndef MAC_FILES_TYPES_FSSIGNATURE_HH
#define MAC_FILES_TYPES_FSSIGNATURE_HH

#ifndef __FILES__
#include <Files.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"

// Nitrogen
#include "Mac/Files/Types/FSCreator.hh"
#include "Mac/Files/Types/FSType.hh"


namespace Mac
{
	
	struct FSSignature
	{
		FSCreator  creator;
		FSType     type;
		
		FSSignature() : creator(), type()
		{
		}
		
		FSSignature( FSCreator  c,
		             FSType     t ) : creator( c ),
		                              type   ( t )
		{
		}
		
		FSSignature( const FInfo& fInfo ) : creator( FSCreator( fInfo.fdCreator ) ),
		                                    type   ( FSType   ( fInfo.fdType    ) )
		{
		}
	};
	
}

#endif

