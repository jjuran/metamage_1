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
#include "Mac/Toolbox/Types/OSType.hh"


namespace Mac
{
	
	struct FSSignature
	{
		OSType creator;
		OSType type;
		
		FSSignature() : creator(), type()
		{
		}
		
		FSSignature( OSType c,
		             OSType t ) : creator( c ),
		                          type   ( t )
		{
		}
		
		FSSignature( const FInfo& fInfo ) : creator( OSType( fInfo.fdCreator ) ),
		                                    type   ( OSType( fInfo.fdType    ) )
		{
		}
	};
	
}

#endif

