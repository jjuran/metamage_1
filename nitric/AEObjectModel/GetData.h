/*	=========
 *	GetData.h
 *	=========
 */

#ifndef AEOBJECTMODEL_GETDATA_H
#define AEOBJECTMODEL_GETDATA_H

#include <map>

// Nitrogen
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSEDESC_HH
#include "Mac/AppleEvents/Functions/AEDisposeDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYFORM_HH
#include "Mac/AppleEvents/Types/AEKeyForm.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEOBJECTCLASS_HH
#include "Mac/AppleEvents/Types/AEObjectClass.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH
#include "Mac/AppleEvents/Types/DescType_scribe.hh"
#endif


namespace Nitrogen
{
	
	inline nucleus::owned< Mac::AEDesc_ObjectSpecifier > GetRootObjectSpecifier()
	{
		return nucleus::owned< Mac::AEDesc_ObjectSpecifier >();
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
