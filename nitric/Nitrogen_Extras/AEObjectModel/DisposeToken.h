/*	==============
 *	DisposeToken.h
 *	==============
 */

#ifndef AEOBJECTMODEL_DISPOSETOKEN_H
#define AEOBJECTMODEL_DISPOSETOKEN_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	void DisposeToken( Owned< AEToken > token );
	
	template < ::DescType tokenType > struct DisposeToken_Traits;
	
	template <> struct DisposeToken_Traits< typeAEList >
	{
		static void DisposeToken( Owned< AETokenList > tokenList )
		{
			std::size_t count = AECountItems( tokenList );
			for ( std::size_t i = 1;  i <= count;  ++i )
			{
				// Get the token from the list (which allocates a new AEDesc),
				// and call AEDisposeToken on it, which disposes both
				// any token-related resources and the newly allocated AEDesc itself.
				// The copy of the token AEDesc remaining in the list descriptor will go 
				// when the list goes.
				// We have to call AEGetNthToken() because AEGetNthDesc() returns 
				// Owned< AEDesc > but AEDisposeToken accepts Owned< AEDesc, AETokenDisposer >.
				// Alternately, we could write AETokenList_Items() or somesuch.
				AEDisposeToken( AEGetNthToken( tokenList, i ) );
			}
			
			// Optional
			AEDisposeDesc( tokenList );
		}
	};
	
	class TokenDisposer
	{
		public:
			typedef void (*Callback)( Owned< AEToken > );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			TokenDisposer( const TokenDisposer& );
			TokenDisposer& operator=( const TokenDisposer& );
		
		public:
			TokenDisposer();
			
			void Register( DescType tokenType, TokenDisposer::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, DisposeToken_Traits< tokenType >::DisposeToken );
			}
			
			void DisposeToken( Owned< AEToken > token );
	};
	
	TokenDisposer& TheGlobalTokenDisposer();
	
	inline void RegisterTokenDisposer( DescType tokenType, TokenDisposer::Callback callback )
	{
		TheGlobalTokenDisposer().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	void RegisterTokenDisposer()
	{
		TheGlobalTokenDisposer().template Register< tokenType >();
	}
	
}

#endif
