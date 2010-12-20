/*	================
 *	AccessProperty.h
 *	================
 */

#ifndef AEOBJECTMODEL_ACCESSPROPERTY_H
#define AEOBJECTMODEL_ACCESSPROPERTY_H

#include <map>

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	nucleus::owned< AEDesc_Token > AccessProperty( Mac::AEPropertyID    propertyID,
	                                               const AEDesc_Token&  containerToken,
	                                               Mac::AEObjectClass   containerClass );
	
	class PropertyAccessor
	{
		public:
			typedef nucleus::owned< AEDesc_Token > (*Callback)( Mac::AEPropertyID, const AEDesc_Token&, Mac::AEObjectClass );
		
		private:
			typedef std::map< Mac::AEPropertyID, Callback >  PropertyMap;
			typedef std::map< DescType, PropertyMap >  Map;
			
			Map map;
			
			// not implemented:
			PropertyAccessor( const PropertyAccessor& );
			PropertyAccessor& operator=( const PropertyAccessor& );
		
		public:
			PropertyAccessor();
			
			void Register( Mac::AEPropertyID propertyID, DescType containerType, PropertyAccessor::Callback callback )
			{
				map[ containerType ][ propertyID ] = callback;
			}
			
			Callback FindAccessor( Mac::AEPropertyID propertyID, DescType tokenType );
			
			nucleus::owned< AEDesc_Token > AccessProperty( Mac::AEPropertyID    propertyID,
			                                               const AEDesc_Token&  containerToken,
			                                               Mac::AEObjectClass   containerClass );
			
			nucleus::owned< AEDesc_Token > AccessAll( const AEDesc_Token&  containerToken,
			                                          Mac::AEObjectClass   containerClass );
	};
	
	PropertyAccessor& TheGlobalPropertyAccessor();
	
	inline void RegisterPropertyAccessor( Mac::AEPropertyID propertyID, DescType containerType, PropertyAccessor::Callback callback )
	{
		TheGlobalPropertyAccessor().Register( propertyID, containerType, callback );
	}
	
	nucleus::owned< AEDesc_Token > AccessClassProperty( Mac::AEPropertyID    propertyID,
	                                                    const AEDesc_Token&  /* containerToken */,
	                                                    Mac::AEObjectClass   containerClass );
	
	nucleus::owned< AEDesc_Token > AccessAllProperties( Mac::AEPropertyID    propertyID,
	                                                    const AEDesc_Token&  containerToken,
	                                                    Mac::AEObjectClass   containerClass );
	
	inline nucleus::owned< AEDesc_Token > AccessAllProperties( const AEDesc_Token&  containerToken,
	                                                           Mac::AEObjectClass   containerClass )
	{
		return AccessAllProperties( Mac::AEPropertyID(), containerToken, containerClass );
	}
	
}

#endif
