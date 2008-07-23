/*	===============
 *	DisposeToken.cp
 *	===============
 */

#ifndef AEOBJECTMODEL_DISPOSETOKEN_H
#include "AEObjectModel/DisposeToken.h"
#endif

// MoreFunctional
#include "PointerToFunction.hh"

// Nitrogen Extras / Iteration
#ifndef ITERATION_AEDESCLISTITEMS_H
#include "Iteration/AEDescListItems.h"
#endif


namespace Nitrogen
{
	
	void DisposeToken( Nucleus::Owned< AEDesc_Data > token )
	{
		return TheGlobalTokenDisposer().DisposeToken( token );
	}
	
	TokenDisposer::TokenDisposer()
	{
		Register( typeAEList, DisposeTokenList );
	}
	
	void TokenDisposer::DisposeToken( Nucleus::Owned< AEDesc_Data > token )
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
	
	void DisposeTokenList( Nucleus::Owned< AEDescList_Data > listOfTokens )
	{
		// The following casting exercise is necessary for this reason:
		// We're already inside AEDisposeToken() for the list, so
		// Disposer< AEDesc_Token > isn't appropriate (since running it would
		// yield recursion).  But we haven't yet called AEDisposeToken() on the
		// individual list elements, so they still have token-nature.
		// Since we've deliberately made it hard to accidentally pull tokens out
		// of a non-token list, here we'll have to jump through our own hoop.
		
		const AEDesc& list = listOfTokens;
		
		const AEDescList_Token& tokenList = static_cast< const AEDescList_Token& >( list );
		
		AEDescList_Token_ItemValue_Container tokens = AEDescList_ItemValues( tokenList );
		
		// Get each token from the list (which allocates a new AEDesc),
		// and call AEDisposeToken on it, which disposes both
		// any token-related resources and the newly allocated AEDesc itself.
		// The copy of the token AEDesc remaining in the list descriptor will go 
		// when the list goes.
		
		std::for_each( tokens.begin(),
		               tokens.end(),
		               more::ptr_fun( AEDisposeToken ) );
		
		// Implicit
		//AEDisposeDesc( listOfTokens );
	};
	
}

