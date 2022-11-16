/*
	DisposeToken.cc
	---------------
*/

#include "AEObjectModel/DisposeToken.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.hh"


namespace Nitrogen
{
	
	void DisposeToken( nucleus::owned< Mac::AEDesc_Data > token )
	{
		return TheGlobalTokenDisposer().DisposeToken( token );
	}
	
	TokenDisposer::TokenDisposer()
	{
		Register( Mac::typeAEList, DisposeTokenList );
	}
	
	void TokenDisposer::DisposeToken( nucleus::owned< Mac::AEDesc_Data > token )
	{
		Map::const_iterator found = map.find( Mac::DescType( token.get().descriptorType ) );
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
	
	void DisposeTokenList( nucleus::owned< Mac::AEDescList_Data > listOfTokens )
	{
		// listOfTokens is owned< AEDescList_Data > for this reason:
		// We're already inside AEDisposeToken() for the list, so
		// disposer< AEDesc_Token > isn't appropriate (since running it would
		// yield recursion).  But we haven't yet called AEDisposeToken() on the
		// individual list elements, so they still have token-nature.
		// Since we've deliberately made it hard to accidentally pull tokens out
		// of a non-token list, here we'll have to jump through our own hoop.
		
		// Get each token from the list (which allocates a new AEDesc),
		// and call AEDisposeToken on it, which disposes both
		// any token-related resources and the newly allocated AEDesc itself.
		// The copy of the token AEDesc remaining in the list descriptor will go 
		// when the list goes.
		
		long count = AECountItems( listOfTokens );
		
		for ( long i = 1;  i <= count;  ++i )
		{
			Mac::AEDesc_Data desc = AEGetNthDesc( listOfTokens, i ).release();
			
			AEDisposeToken( &desc );
		}
		
		// Implicit
		//AEDisposeDesc( listOfTokens );
	};
	
}
