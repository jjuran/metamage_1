/*	===============
 *	DisposeToken.cp
 *	===============
 */

#ifndef AEOBJECTMODEL_DISPOSETOKEN_H
#include "AEObjectModel/DisposeToken.h"
#endif


namespace Nitrogen
{
	
	void DisposeToken( Owned< AEToken > token )
	{
		return TheGlobalTokenDisposer().DisposeToken( token );
	}
	
	TokenDisposer::TokenDisposer()
	{
		Register< typeAEList >();
	}
	
	void TokenDisposer::DisposeToken( Owned< AEToken > token )
	{
		Map::const_iterator found = map.find( token.Get().descriptorType );
		if ( found == map.end() )
		{
			// If we omitted this, the descriptor would still be disposed
			// at function exit.
			
			AEDisposeDesc( token );
			
			return;  // FIXME:  Check for typeWildCard
		}
		
		return found->second( token );
	}
	
	TokenDisposer& TheGlobalTokenDisposer()
	{
		static TokenDisposer theGlobalTokenDisposer;
		return theGlobalTokenDisposer;
	}
	
}

