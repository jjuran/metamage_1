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
	
	void DisposeToken( Nucleus::Owned< AEDesc_Data > token );
	
	template < ::DescType tokenType > struct DisposeToken_Traits;
	
	class TokenDisposer
	{
		public:
			typedef void (*Callback)( Nucleus::Owned< AEDesc_Data > );
		
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
			
			void DisposeToken( Nucleus::Owned< AEDesc_Data > token );
	};
	
	TokenDisposer& TheGlobalTokenDisposer();
	
	inline void RegisterTokenDisposer( DescType tokenType, TokenDisposer::Callback callback )
	{
		TheGlobalTokenDisposer().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	inline void RegisterTokenDisposer()
	{
		TheGlobalTokenDisposer().template Register< tokenType >();
	}
	
	void DisposeTokenList( Nucleus::Owned< AEDescList_Data > tokenList );
	
}

#endif
