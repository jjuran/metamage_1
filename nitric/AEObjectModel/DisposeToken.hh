/*
	DisposeToken.hh
	---------------
*/

#ifndef AEOBJECTMODEL_DISPOSETOKEN_HH
#define AEOBJECTMODEL_DISPOSETOKEN_HH

// Nitrogen
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSEDESC_HH
#include "Mac/AppleEvents/Functions/AEDisposeDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif


namespace Nitrogen
{
	
	void DisposeToken( nucleus::owned< Mac::AEDesc_Data > token );
	
	void DisposeTokenList( nucleus::owned< Mac::AEDescList_Data > tokenList );
	
}

#endif
