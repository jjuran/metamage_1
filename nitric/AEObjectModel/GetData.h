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
	
	inline nucleus::owned< Mac::AEDesc_ObjectSpecifier > GetRootObjectSpecifier()
	{
		return AEInitializeDesc< Mac::AEDesc_Data >();
	}
	
	nucleus::owned< Mac::AEDesc_ObjectSpecifier >
	//
	AECreateObjectSpecifier( Mac::AEObjectClass                  objectClass,
	                         const Mac::AEDesc_ObjectSpecifier&  container,
	                         Mac::AEKeyForm                      keyForm,
	                         const Mac::AEDesc_Data&             keyData );
	
	nucleus::owned< Mac::AEDesc_Data >
	//
	GetData( const Mac::AEDesc_Token&  obj,
	         Mac::DescType             desiredType = Mac::typeWildCard );
	
	class DataGetter
	{
		public:
			typedef nucleus::owned< Mac::AEDesc_Data > (*Callback)( const Mac::AEDesc_Token&, Mac::DescType );
		
		private:
			typedef std::map< Mac::DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			DataGetter( const DataGetter& );
			DataGetter& operator=( const DataGetter& );
		
		public:
			DataGetter();
			
			void Register( Mac::DescType tokenType, DataGetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			nucleus::owned< Mac::AEDesc_Data > GetData( const Mac::AEDesc_Token& obj, Mac::DescType desiredType );
	};
	
	DataGetter& TheGlobalDataGetter();
	
	inline void RegisterDataGetter( Mac::DescType tokenType, DataGetter::Callback callback )
	{
		TheGlobalDataGetter().Register( tokenType, callback );
	}
	
}

#endif
