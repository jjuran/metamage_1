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
	
	Owned< AEObjectSpecifier > AECreateObjectSpecifier( AEObjectClass             objectClass,
	                                                    const AEObjectSpecifier&  container,
	                                                    AEEnumeration             keyForm,
	                                                    const AEDesc&             keyData )
	{
		return AECoerceDesc
		(
			AECreateList< true >()
				<< keyAEDesiredClass + AECreateDesc< typeType       >( DescType( objectClass ) )
				<< keyAEKeyForm      + AECreateDesc< typeEnumerated >(              keyForm    )
				<< keyAEKeyData      + keyData
				<< keyAEContainer    + container,
				
			typeObjectSpecifier
		);
	}
	
	Owned< AEDesc > GetData( const AEToken& obj, DescType desiredType )
	{
		return TheGlobalDataGetter().GetData( obj, desiredType );
	}
	
	DataGetter::DataGetter()
	{
		Register< typeNull >();
	}
	
	Owned< AEDesc > DataGetter::GetData( const AEToken& obj, DescType desiredType )
	{
		Map::const_iterator found = map.find( obj.descriptorType );
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

