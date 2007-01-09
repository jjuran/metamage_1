/*	===============
 *	DisposeToken.cp
 *	===============
 */

#ifndef AEOBJECTMODEL_DISPOSETOKEN_H
#include "AEObjectModel/DisposeToken.h"
#endif

// Nitrogen Extras / Iteration
#ifndef ITERATION_AEDESCLISTITEMS_H
#include "Iteration/AEDescListItems.h"
#endif

// Nitrogen Extras / Templates
#ifndef TEMPLATES_POINTERTOFUNCTION_H
#include "Templates/PointerToFunction.h"
#endif


namespace Nitrogen
{
	
	void DisposeToken( Nucleus::Owned< AEToken > token )
	{
		return TheGlobalTokenDisposer().DisposeToken( token );
	}
	
	TokenDisposer::TokenDisposer()
	{
		Register( typeAEList, DisposeTokenList );
	}
	
	void TokenDisposer::DisposeToken( Nucleus::Owned< AEToken > token )
	{
		Map::const_iterator found = map.find( DescType( token.Get().descriptorType ) );
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
	
	static void AEDisposeTokenFromList( Nucleus::Owned< AEToken > token )
	{
		AEDisposeToken( Nucleus::Owned< AEToken, AETokenDisposer >::Seize( token.Release() ) );
	}
	
	void DisposeTokenList( Nucleus::Owned< AETokenList > tokenList )
	{
		AEDescList_ItemValue_Container tokens = AEDescList_ItemValues( tokenList );
		
		// Get each token from the list (which allocates a new AEDesc),
		// and call AEDisposeToken on it, which disposes both
		// any token-related resources and the newly allocated AEDesc itself.
		// The copy of the token AEDesc remaining in the list descriptor will go 
		// when the list goes.
		// We use AEDisposeTokenFromList() which accepts an Owned< AEDesc >
		// and passes an Owned< AEToken, AETokenDisposer > to AEDisposeToken().
		// This is necessary because AEGetNthDesc() always returns Owned< AEDesc >.
		
		std::for_each( tokens.begin(),
		               tokens.end(),
		               PtrFun( AEDisposeTokenFromList ) );
		
		// Optional
		AEDisposeDesc( tokenList );
	};
	
}

