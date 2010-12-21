/*	==============
 *	DisposeToken.h
 *	==============
 */

#ifndef AEOBJECTMODEL_DISPOSETOKEN_H
#define AEOBJECTMODEL_DISPOSETOKEN_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	void DisposeToken( nucleus::owned< Mac::AEDesc_Data > token );
	
	template < ::DescType tokenType > struct DisposeToken_Traits;
	
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
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, DisposeToken_Traits< tokenType >::DisposeToken );
			}
			
			void DisposeToken( nucleus::owned< Mac::AEDesc_Data > token );
	};
	
	TokenDisposer& TheGlobalTokenDisposer();
	
	inline void RegisterTokenDisposer( Mac::DescType tokenType, TokenDisposer::Callback callback )
	{
		TheGlobalTokenDisposer().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	inline void RegisterTokenDisposer()
	{
		TheGlobalTokenDisposer().template Register< tokenType >();
	}
	
	void DisposeTokenList( nucleus::owned< Mac::AEDescList_Data > tokenList );
	
}

#endif
