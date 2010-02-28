/*	=========
 *	GetData.h
 *	=========
 */

#ifndef AEOBJECTMODEL_GETDATA_H
#define AEOBJECTMODEL_GETDATA_H

#include <map>

#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	inline Nucleus::Owned< AEDesc_ObjectSpecifier > GetRootObjectSpecifier()
	{
		return AEInitializeDesc< AEDesc_Data >();
	}
	
	Nucleus::Owned< AEDesc_ObjectSpecifier > AECreateObjectSpecifier( AEObjectClass                  objectClass,
	                                                                  const AEDesc_ObjectSpecifier&  container,
	                                                                  AEEnumeration                  keyForm,
	                                                                  const AEDesc_Data&             keyData );
	
	Nucleus::Owned< AEDesc_Data > GetData( const AEDesc_Token& obj, DescType desiredType = typeWildCard );
	
	class DataGetter
	{
		public:
			typedef Nucleus::Owned< AEDesc_Data > (*Callback)( const AEDesc_Token&, DescType );
		
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
			
			Nucleus::Owned< AEDesc_Data > GetData( const AEDesc_Token& obj, DescType desiredType );
	};
	
	DataGetter& TheGlobalDataGetter();
	
	inline void RegisterDataGetter( DescType tokenType, DataGetter::Callback callback )
	{
		TheGlobalDataGetter().Register( tokenType, callback );
	}
	
}

#endif
