/*	=========
 *	GetData.h
 *	=========
 */

#ifndef AEOBJECTMODEL_GETDATA_H
#define AEOBJECTMODEL_GETDATA_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	inline Owned< AEObjectSpecifier > GetRootObjectSpecifier()
	{
		return AEInitializeDesc();
	}
	
	Owned< AEObjectSpecifier > AECreateObjectSpecifier( AEObjectClass             objectClass,
	                                                    const AEObjectSpecifier&  container,
	                                                    AEEnumeration             keyForm,
	                                                    const AEDesc&             keyData );
	
	Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType = typeWildCard );
	
	template < ::DescType tokenType > struct GetData_Traits;
	
	template <> struct GetData_Traits< typeNull >
	{
		static Owned< AEDesc > GetData( const AEToken&, DescType )  { return GetRootObjectSpecifier(); }
	};
	
	class DataGetter
	{
		public:
			typedef Owned< AEDesc > (*Callback)( const AEToken&, DescType );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			DataGetter( const DataGetter& );
			DataGetter& operator=( const DataGetter& );
		
		public:
			DataGetter();
			
			void Register( DescType tokenType, DataGetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < ::DescType tokenType >
			void Register()
			{
				Register( tokenType, GetData_Traits< tokenType >::GetData );
			}
			
			Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType );
	};
	
	DataGetter& TheGlobalDataGetter();
	
	inline void RegisterDataGetter( DescType tokenType, DataGetter::Callback callback )
	{
		TheGlobalDataGetter().Register( tokenType, callback );
	}
	
	template < ::DescType tokenType >
	void RegisterDataGetter()
	{
		TheGlobalDataGetter().template Register< tokenType >();
	}
	
}

#endif
