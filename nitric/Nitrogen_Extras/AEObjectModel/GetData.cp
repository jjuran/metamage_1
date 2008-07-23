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

// Nitrogen Extras / Operators
#include "Operators/AEDataModel.h"


namespace Nitrogen
{
	
	static Nucleus::Owned< AEDesc > GetDataFromAppToken( const AEToken&, DescType )
	{
		return GetRootObjectSpecifier();
	}
	
	static Nucleus::Owned< AEDesc > GetDataFromTokenList( const AEToken& obj, DescType desiredType )
	{
		Nucleus::Owned< AEDescList > list = AECreateList( false );
		
		UInt32 count = AECountItems( obj );
		
		for ( UInt32 i = 1;  i <= count;  ++i )
		{
			AEPutDesc( list,
			           0,
			           GetData( AEGetNthDesc( obj, i ),
			                    desiredType ) );
		}
		
		return list;
	}
	
	
	Nucleus::Owned< AEObjectSpecifier > AECreateObjectSpecifier( AEObjectClass             objectClass,
	                                                             const AEObjectSpecifier&  container,
	                                                             AEEnumeration             keyForm,
	                                                             const AEDesc&             keyData )
	{
		using namespace Nucleus::Operators;
		
		return AECoerceDesc( AECreateList( true ) << AEPutKeyPtr< keyAEDesiredClass >( objectClass )
		                                          << AEPutKeyPtr< keyAEKeyForm      >( keyForm     )
		                                          << AEPutKeyDesc( keyAEKeyData,   keyData   )
		                                          << AEPutKeyDesc( keyAEContainer, container ),
		                     typeObjectSpecifier );
	}
	
	Nucleus::Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType )
	{
		return TheGlobalDataGetter().GetData( obj, desiredType );
	}
	
	DataGetter::DataGetter()
	{
		Register( typeNull,   GetDataFromAppToken  );
		Register( typeAEList, GetDataFromTokenList );
	}
	
	Nucleus::Owned< AEDesc > DataGetter::GetData( const AEToken& obj, DescType desiredType )
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

