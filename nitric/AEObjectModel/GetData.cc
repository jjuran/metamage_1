/*
	GetData.cc
	----------
*/

#include "AEObjectModel/GetData.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.hh"


namespace Nitrogen
{
	
	static nucleus::owned< Mac::AEDesc_Data > GetDataFromAppToken( const Mac::AEDesc_Token&, Mac::DescType )
	{
		return GetRootObjectSpecifier();
	}
	
	static nucleus::owned< Mac::AEDesc_Data > GetDataFromTokenList( const Mac::AEDescList_Token& obj, Mac::DescType desiredType )
	{
		nucleus::owned< Mac::AEDescList_Data > list = AECreateList( false );
		
		UInt32 count = AECountItems( obj );
		
		for ( UInt32 i = 1;  i <= count;  ++i )
		{
			AEPutDesc( list,
			           0,
			           GetData( AEDesc_Cast< const Mac::AEDesc_Token >( AEGetNthDesc( obj, i ).get() ),
			                    desiredType ).get() );
		}
		
		return list;
	}
	
	
	nucleus::owned< Mac::AEDesc_ObjectSpecifier >
	//
	AECreateObjectSpecifier( Mac::AEObjectClass                  objectClass,
	                         const Mac::AEDesc_ObjectSpecifier&  container,
	                         Mac::AEKeyForm                      keyForm,
	                         const Mac::AEDesc_Data&             keyData )
	{
		nucleus::owned< Mac::AERecord_Data > record = AECreateList( true );
		
		/*
			g++ 3 can't handle this:
			
			AEPutKeyPtr< Mac::keyAEDesiredClass >( record, objectClass );
			AEPutKeyPtr< Mac::keyAEKeyForm      >( record, keyForm     );
		*/
		
		AEPutKeyPtr( record,
		             Mac::keyAEDesiredClass,
		             Mac::typeType,
		             &objectClass,
		             sizeof objectClass );
		
		AEPutKeyPtr( record,
		             Mac::keyAEKeyForm,
		             Mac::typeEnumerated,
		             &keyForm,
		             sizeof keyForm );
		
		AEPutKeyDesc( record, Mac::keyAEKeyData,   keyData   );
		AEPutKeyDesc( record, Mac::keyAEContainer, container );
		
		return AECoerceDesc( record, Mac::typeObjectSpecifier );
	}
	
	nucleus::owned< Mac::AEDesc_Data >
	//
	GetData( const Mac::AEDesc_Token&  obj,
	         Mac::DescType             desiredType )
	{
		return TheGlobalDataGetter().GetData( obj, desiredType );
	}
	
	DataGetter::DataGetter()
	{
		Register( Mac::typeNull,   GetDataFromAppToken  );
		Register( Mac::typeAEList, GetDataFromTokenList );
	}
	
	nucleus::owned< Mac::AEDesc_Data > DataGetter::GetData( const Mac::AEDesc_Token& obj, Mac::DescType desiredType )
	{
		Map::const_iterator found = map.find( Mac::DescType( obj.descriptorType ) );
		
		if ( found == map.end() )
		{
			Mac::ThrowOSStatus( errAEEventNotHandled );
		}
		
		return found->second( obj, desiredType );
	}
	
	DataGetter& TheGlobalDataGetter()
	{
		static DataGetter theGlobalDataGetter;
		
		return theGlobalDataGetter;
	}
	
}
