/*	==========
 *	GetData.cp
 *	==========
 */

#ifndef AEOBJECTMODEL_GETDATA_H
#include "AEObjectModel/GetData.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace Nitrogen
{
	
	static Nucleus::Owned< AEDesc_Data > GetDataFromAppToken( const AEDesc_Token&, DescType )
	{
		return GetRootObjectSpecifier();
	}
	
	static Nucleus::Owned< AEDesc_Data > GetDataFromTokenList( const AEDescList_Token& obj, DescType desiredType )
	{
		Nucleus::Owned< AEDescList_Data > list = AECreateList< AEDescList_Data >( false );
		
		UInt32 count = AECountItems( obj );
		
		for ( UInt32 i = 1;  i <= count;  ++i )
		{
			AEPutDesc( list,
			           0,
			           GetData( AEGetNthDesc< AEDesc_Token >( obj, i ),
			                    desiredType ).Get() );
		}
		
		return list;
	}
	
	
	Nucleus::Owned< AEDesc_ObjectSpecifier > AECreateObjectSpecifier( AEObjectClass                  objectClass,
	                                                                  const AEDesc_ObjectSpecifier&  container,
	                                                                  AEEnumeration                  keyForm,
	                                                                  const AEDesc_Data&             keyData )
	{
		Nucleus::Owned< AERecord_Data > record = AECreateList< AERecord_Data >( true );
		
		AEPutKeyPtr< AERecord_Data, keyAEDesiredClass >( record, objectClass );
		AEPutKeyPtr< AERecord_Data, keyAEKeyForm      >( record, keyForm     );
		
		AEPutKeyDesc( record, keyAEKeyData,   keyData   );
		AEPutKeyDesc( record, keyAEContainer, container );
		
		return AECoerceDesc( record, typeObjectSpecifier );
	}
	
	Nucleus::Owned< AEDesc_Data > GetData( const AEDesc_Token& obj, DescType desiredType )
	{
		return TheGlobalDataGetter().GetData( obj, desiredType );
	}
	
	DataGetter::DataGetter()
	{
		Register( typeNull,   GetDataFromAppToken  );
		Register( typeAEList, GetDataFromTokenList );
	}
	
	Nucleus::Owned< AEDesc_Data > DataGetter::GetData( const AEDesc_Token& obj, DescType desiredType )
	{
		Map::const_iterator found = map.find( DescType( obj.descriptorType ) );
		
		if ( found == map.end() )
		{
			throw ErrAEEventNotHandled();
		}
		
		return found->second( obj, desiredType );
	}
	
	DataGetter& TheGlobalDataGetter()
	{
		static DataGetter theGlobalDataGetter;
		
		return theGlobalDataGetter;
	}
	
}

