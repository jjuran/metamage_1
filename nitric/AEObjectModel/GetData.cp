/*	==========
 *	GetData.cp
 *	==========
 */

#ifndef AEOBJECTMODEL_GETDATA_H
#include "AEObjectModel/GetData.h"
#endif


namespace Nitrogen
{
	
	static nucleus::owned< AEDesc_Data > GetDataFromAppToken( const AEDesc_Token&, DescType )
	{
		return GetRootObjectSpecifier();
	}
	
	static nucleus::owned< AEDesc_Data > GetDataFromTokenList( const AEDescList_Token& obj, DescType desiredType )
	{
		nucleus::owned< AEDescList_Data > list = AECreateList( false );
		
		UInt32 count = AECountItems( obj );
		
		for ( UInt32 i = 1;  i <= count;  ++i )
		{
			AEPutDesc( list,
			           0,
			           GetData( AEDesc_Cast< const AEDesc_Token >( AEGetNthDesc( obj, i ).get() ),
			                    desiredType ).get() );
		}
		
		return list;
	}
	
	
	nucleus::owned< AEDesc_ObjectSpecifier > AECreateObjectSpecifier( AEObjectClass                  objectClass,
	                                                                  const AEDesc_ObjectSpecifier&  container,
	                                                                  AEEnumeration                  keyForm,
	                                                                  const AEDesc_Data&             keyData )
	{
		nucleus::owned< AERecord_Data > record = AECreateList( true );
		
		AEPutKeyPtr< keyAEDesiredClass >( record, objectClass );
		AEPutKeyPtr< keyAEKeyForm      >( record, keyForm     );
		
		AEPutKeyDesc( record, keyAEKeyData,   keyData   );
		AEPutKeyDesc( record, keyAEContainer, container );
		
		return AECoerceDesc( record, typeObjectSpecifier );
	}
	
	nucleus::owned< AEDesc_Data > GetData( const AEDesc_Token& obj, DescType desiredType )
	{
		return TheGlobalDataGetter().GetData( obj, desiredType );
	}
	
	DataGetter::DataGetter()
	{
		Register( typeNull,   GetDataFromAppToken  );
		Register( typeAEList, GetDataFromTokenList );
	}
	
	nucleus::owned< AEDesc_Data > DataGetter::GetData( const AEDesc_Token& obj, DescType desiredType )
	{
		Map::const_iterator found = map.find( DescType( obj.descriptorType ) );
		
		if ( found == map.end() )
		{
			ThrowOSStatus( errAEEventNotHandled );
		}
		
		return found->second( obj, desiredType );
	}
	
	DataGetter& TheGlobalDataGetter()
	{
		static DataGetter theGlobalDataGetter;
		
		return theGlobalDataGetter;
	}
	
}

