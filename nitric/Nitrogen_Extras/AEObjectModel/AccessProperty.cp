/*	=================
 *	AccessProperty.cp
 *	=================
 */

#ifndef AEOBJECTMODEL_ACCESSPROPERTY_H
#include "AEObjectModel/AccessProperty.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace Nitrogen
{
	
	
	Owned< AEToken, AETokenDisposer > AccessProperty( AEPropertyID    propertyID,
	                                                  const AEToken&  containerToken,
	                                                  AEObjectClass   containerClass )
	{
		return TheGlobalPropertyAccessor().AccessProperty( propertyID, containerToken, containerClass );
	}
	
	PropertyAccessor::PropertyAccessor()
	{
		Register( pClass, typeWildCard, AccessClassProperty );
		Register( kAEAll, typeWildCard, AccessAllProperties );
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
			
			foundProp = foundType->second.find( typeWildCard );
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
			
			foundProp = foundType->second.find( typeWildCard );
			if ( foundProp != foundType->second.end() )
			{
				return foundProp->second;
			}
		}
		
		throw ErrAEEventNotHandled();
	}
	
	Owned< AEToken, AETokenDisposer > PropertyAccessor::AccessProperty( AEPropertyID    propertyID,
	                                                                    const AEToken&  containerToken,
	                                                                    AEObjectClass   containerClass )
	{
		Callback accessor = FindAccessor( propertyID, containerToken.descriptorType );
		return accessor( propertyID, containerToken, containerClass );
	}
	
	Owned< AEToken, AETokenDisposer > PropertyAccessor::AccessAll( const AEToken&  containerToken,
	                                                               AEObjectClass   containerClass )
	{
		Map::const_iterator foundType = map.find( containerToken.descriptorType );
		if ( foundType == map.end() )
		{
			throw ErrAEEventNotHandled();
		}
		
		Owned< AEToken, AETokenDisposer > result = AECreateTokenList< true >();
		
		for ( PropertyMap::const_iterator itProp = foundType->second.begin();
		      itProp != foundType->second.end();
		      ++itProp )
		{
			AEPropertyID propertyID = itProp->first;
			AEPutKeyDesc( result,
			              AEKeyword( propertyID ),
			              itProp->second( propertyID,
			                              containerToken,
			                              containerClass ) );
		}
		
		return result;
	}
	
	PropertyAccessor& TheGlobalPropertyAccessor()
	{
		static PropertyAccessor theGlobalPropertyAccessor;
		return theGlobalPropertyAccessor;
	}
	
	Owned< AEToken, AETokenDisposer > AccessClassProperty( AEPropertyID    /* propertyID */,
	                                                       const AEToken&  /* containerToken */,
	                                                       AEObjectClass   containerClass )
	{
		return AECreateToken< typeType >( containerClass );
	}
	
	Owned< AEToken, AETokenDisposer > AccessAllProperties( AEPropertyID    propertyID,
	                                                       const AEToken&  containerToken,
	                                                       AEObjectClass   containerClass )
	{
		return TheGlobalPropertyAccessor().AccessAll( containerToken, containerClass );
	}
	
}

