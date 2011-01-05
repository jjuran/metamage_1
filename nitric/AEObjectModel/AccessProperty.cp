/*	=================
 *	AccessProperty.cp
 *	=================
 */

#ifndef AEOBJECTMODEL_ACCESSPROPERTY_H
#include "AEObjectModel/AccessProperty.h"
#endif

// Mac OS
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_ASREGISTRY_HH
#include "Nitrogen/ASRegistry.hh"
#endif


namespace Nitrogen
{
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AccessProperty( Mac::AEPropertyID         propertyID,
	                const Mac::AEDesc_Token&  containerToken,
	                Mac::AEObjectClass        containerClass )
	{
		return TheGlobalPropertyAccessor().AccessProperty( propertyID, containerToken, containerClass );
	}
	
	PropertyAccessor::PropertyAccessor()
	{
		Register( Mac::pClass, Mac::typeWildCard, AccessClassProperty );
		Register( Mac::pAll,   Mac::typeWildCard, AccessAllProperties );
	}
	
	PropertyAccessor::Callback PropertyAccessor::FindAccessor( Mac::AEPropertyID  propertyID,
	                                                           Mac::DescType      tokenType )
	{
		Map        ::const_iterator foundType;
		PropertyMap::const_iterator foundProp;
		
		foundType = map.find( tokenType );
		
		if ( foundType != map.end() )
		{
			foundProp = foundType->second.find( propertyID );
			
			if ( foundProp != foundType->second.end() )
			{
				return foundProp->second;
			}
			
			foundProp = foundType->second.find( Mac::AEPropertyID( Mac::typeWildCard ) );
			
			if ( foundProp != foundType->second.end() )
			{
				return foundProp->second;
			}
		}
		
		foundType = map.find( Mac::typeWildCard );
		
		if ( foundType != map.end() )
		{
			foundProp = foundType->second.find( propertyID );
			
			if ( foundProp != foundType->second.end() )
			{
				return foundProp->second;
			}
			
			foundProp = foundType->second.find( Mac::AEPropertyID( Mac::typeWildCard ) );
			
			if ( foundProp != foundType->second.end() )
			{
				return foundProp->second;
			}
		}
		
		Mac::ThrowOSStatus( errAEEventNotHandled );
		
		// Not reached
		return NULL;
	}
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	PropertyAccessor::AccessProperty( Mac::AEPropertyID         propertyID,
	                                  const Mac::AEDesc_Token&  containerToken,
	                                  Mac::AEObjectClass        containerClass )
	{
		Callback accessor = FindAccessor( propertyID, Mac::DescType( containerToken.descriptorType ) );
		
		return accessor( propertyID, containerToken, containerClass );
	}
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	PropertyAccessor::AccessAll( const Mac::AEDesc_Token&  containerToken,
	                             Mac::AEObjectClass        containerClass )
	{
		Map::const_iterator foundType = map.find( Mac::DescType( containerToken.descriptorType ) );
		
		if ( foundType == map.end() )
		{
			Mac::ThrowOSStatus( errAEEventNotHandled );
		}
		
		nucleus::owned< Mac::AEDesc_Token > result = AECreateList< Mac::AEDesc_Token >( true );
		
		for ( PropertyMap::const_iterator itProp = foundType->second.begin();
		      itProp != foundType->second.end();
		      ++itProp )
		{
			Mac::AEPropertyID propertyID = itProp->first;
			
			nucleus::owned< Mac::AEDesc_Token > propertyToken;
			
			try
			{
				propertyToken = itProp->second( propertyID,
				                                containerToken,
				                                containerClass );
			}
			catch ( const Mac::OSStatus& err )
			{
				if ( err != errAENoSuchObject )
				{
					throw;
				}
				
				propertyToken = MissingValue();
			}
			
			AEPutKeyDesc( result,
			              Mac::AEKeyword( propertyID + 0 ),
			              propertyToken );
			
			// Dispose the token's AEDesc, but don't call the token disposal function.
			AEDesc tokenDesc = propertyToken.release();
			
			nucleus::owned< Mac::AEDesc_Data >::seize( static_cast< const Mac::AEDesc_Data& >( tokenDesc ) );
		}
		
		return result;
	}
	
	PropertyAccessor& TheGlobalPropertyAccessor()
	{
		static PropertyAccessor theGlobalPropertyAccessor;
		return theGlobalPropertyAccessor;
	}
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AccessClassProperty( Mac::AEPropertyID         propertyID,
	                     const Mac::AEDesc_Token&  containerToken,
	                     Mac::AEObjectClass        containerClass )
	{
		return AECreateDesc< Mac::typeType, Mac::AEDesc_Token >( Mac::DescType( containerClass + 0 ) );
	}
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AccessAllProperties( Mac::AEPropertyID         propertyID,
	                     const Mac::AEDesc_Token&  containerToken,
	                     Mac::AEObjectClass        containerClass )
	{
		return TheGlobalPropertyAccessor().AccessAll( containerToken, containerClass );
	}
	
}

