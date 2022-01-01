/*
	DisposeToken.hh
	---------------
*/

#ifndef AEOBJECTMODEL_DISPOSETOKEN_HH
#define AEOBJECTMODEL_DISPOSETOKEN_HH

// Standard C++
#include <map>

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
	
	class TokenDisposer
	{
		public:
			typedef void (*Callback)( nucleus::owned< Mac::AEDesc_Data > );
		
		private:
			typedef std::map< Mac::DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			TokenDisposer( const TokenDisposer& );
			TokenDisposer& operator=( const TokenDisposer& );
		
		public:
			TokenDisposer();
			
			void Register( Mac::DescType tokenType, TokenDisposer::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			void DisposeToken( nucleus::owned< Mac::AEDesc_Data > token );
	};
	
	TokenDisposer& TheGlobalTokenDisposer();
	
	inline void RegisterTokenDisposer( Mac::DescType tokenType, TokenDisposer::Callback callback )
	{
		TheGlobalTokenDisposer().Register( tokenType, callback );
	}
	
	void DisposeTokenList( nucleus::owned< Mac::AEDescList_Data > tokenList );
	
}

#endif
