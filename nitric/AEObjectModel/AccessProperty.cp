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
#ifndef NITROGEN_ASREGISTRY_HH
#include "Nitrogen/ASRegistry.hh"
#endif


namespace Nitrogen
{
	
	nucleus::owned< AEDesc_Token > AccessProperty( AEPropertyID         propertyID,
	                                               const AEDesc_Token&  containerToken,
	                                               AEObjectClass        containerClass )
	{
		return TheGlobalPropertyAccessor().AccessProperty( propertyID, containerToken, containerClass );
	}
	
	PropertyAccessor::PropertyAccessor()
	{
		Register( pClass, typeWildCard, AccessClassProperty );
		Register( pAll,   typeWildCard, AccessAllProperties );
	}
	
	PropertyAccessor::Callback PropertyAccessor::FindAccessor( AEPropertyID  propertyID,
	                                                           DescType      tokenType )
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
			
			foundProp = foundType->second.find( AEPropertyID( typeWildCard ) );
			
			if ( foundProp != foundType->second.end() )
			{
				return foundProp->second;
			}
		}
		
		foundType = map.find( typeWildCard );
		
		if ( foundType != map.end() )
		{
			foundProp = foundType->second.find( propertyID );
			
			if ( foundProp != foundType->second.end() )
			{
				return foundProp->second;
			}
			
			foundProp = foundType->second.find( AEPropertyID( typeWildCard ) );
			
			if ( foundProp != foundType->second.end() )
			{
				return foundProp->second;
			}
		}
		
		ThrowOSStatus( errAEEventNotHandled );
		
		// Not reached
		return NULL;
	}
	
	nucleus::owned< AEDesc_Token > PropertyAccessor::AccessProperty( AEPropertyID         propertyID,
	                                                                 const AEDesc_Token&  containerToken,
	                                                                 AEObjectClass        containerClass )
	{
		Callback accessor = FindAccessor( propertyID, DescType( containerToken.descriptorType ) );
		
		return accessor( propertyID, containerToken, containerClass );
	}
	
	nucleus::owned< AEDesc_Token > PropertyAccessor::AccessAll( const AEDesc_Token&  containerToken,
	                                                            AEObjectClass        containerClass )
	{
		Map::const_iterator foundType = map.find( DescType( containerToken.descriptorType ) );
		
		if ( foundType == map.end() )
		{
			ThrowOSStatus( errAEEventNotHandled );
		}
		
		nucleus::owned< AEDesc_Token > result = AECreateList< AEDesc_Token >( true );
		
		for ( PropertyMap::const_iterator itProp = foundType->second.begin();
		      itProp != foundType->second.end();
		      ++itProp )
		{
			AEPropertyID propertyID = itProp->first;
			
			nucleus::owned< AEDesc_Token > propertyToken;
			
			try
			{
				propertyToken = itProp->second( propertyID,
				                                containerToken,
				                                containerClass );
			}
			catch ( const OSStatus& err )
			{
				if ( err != errAENoSuchObject )
				{
					throw;
				}
				
				propertyToken = MissingValue();
			}
			
			AEPutKeyDesc( result,
			              AEKeyword( ::FourCharCode( propertyID ) ),
			              propertyToken );
			
			// Dispose the token's AEDesc, but don't call the token disposal function.
			AEDesc tokenDesc = propertyToken.release();
			
			nucleus::owned< AEDesc_Data >::seize( static_cast< const AEDesc_Data& >( tokenDesc ) );
		}
		
		return result;
	}
	
	PropertyAccessor& TheGlobalPropertyAccessor()
	{
		static PropertyAccessor theGlobalPropertyAccessor;
		return theGlobalPropertyAccessor;
	}
	
	nucleus::owned< AEDesc_Token > AccessClassProperty( AEPropertyID         /* propertyID */,
	                                                    const AEDesc_Token&  /* containerToken */,
	                                                    AEObjectClass        containerClass )
	{
		//return AECreateDesc< typeObjectClass, AEDesc_Token >( containerClass );
		return AECreateDesc< typeType, AEDesc_Token >( DescType( ::FourCharCode( containerClass ) ) );
	}
	
	nucleus::owned< AEDesc_Token > AccessAllProperties( AEPropertyID         /*propertyID*/,
	                                                    const AEDesc_Token&  containerToken,
	                                                    AEObjectClass        containerClass )
	{
		return TheGlobalPropertyAccessor().AccessAll( containerToken, containerClass );
	}
	
}

