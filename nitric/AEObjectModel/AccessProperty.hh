/*
	AccessProperty.hh
	-----------------
*/

#ifndef AEOBJECTMODEL_ACCESSPROPERTY_HH
#define AEOBJECTMODEL_ACCESSPROPERTY_HH

// Standard C++
#include <map>

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSEDESC_HH
#include "Mac/AppleEvents/Functions/AEDisposeDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSETOKEN_HH
#include "Mac/AppleEvents/Functions/AEDisposeToken.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEOBJECTCLASS_HH
#include "Mac/AppleEvents/Types/AEObjectClass.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEPROPERTYID_HH
#include "Mac/AppleEvents/Types/AEPropertyID.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif


namespace Nitrogen
{
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AccessProperty( Mac::AEPropertyID         propertyID,
	                const Mac::AEDesc_Token&  containerToken,
	                Mac::AEObjectClass        containerClass );
	
	class PropertyAccessor
	{
		public:
			typedef nucleus::owned< Mac::AEDesc_Token >
			        //
			        (*Callback)( Mac::AEPropertyID,
			                     const Mac::AEDesc_Token&,
			                     Mac::AEObjectClass );
		
		private:
			typedef std::map< Mac::AEPropertyID, Callback >  PropertyMap;
			typedef std::map< Mac::DescType, PropertyMap  >  Map;
			
			Map map;
			
			// not implemented:
			PropertyAccessor( const PropertyAccessor& );
			PropertyAccessor& operator=( const PropertyAccessor& );
		
		public:
			PropertyAccessor();
			
			void Register( Mac::AEPropertyID propertyID, Mac::DescType containerType, PropertyAccessor::Callback callback )
			{
				map[ containerType ][ propertyID ] = callback;
			}
			
			Callback FindAccessor( Mac::AEPropertyID propertyID, Mac::DescType tokenType );
			
			nucleus::owned< Mac::AEDesc_Token > AccessProperty( Mac::AEPropertyID         propertyID,
			                                                    const Mac::AEDesc_Token&  containerToken,
			                                                    Mac::AEObjectClass        containerClass );
			
			nucleus::owned< Mac::AEDesc_Token > AccessAll( const Mac::AEDesc_Token&  containerToken,
			                                               Mac::AEObjectClass        containerClass );
	};
	
	PropertyAccessor& TheGlobalPropertyAccessor();
	
	inline void RegisterPropertyAccessor( Mac::AEPropertyID propertyID, Mac::DescType containerType, PropertyAccessor::Callback callback )
	{
		TheGlobalPropertyAccessor().Register( propertyID, containerType, callback );
	}
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AccessClassProperty( Mac::AEPropertyID         propertyID,
	                     const Mac::AEDesc_Token&  containerToken,
	                     Mac::AEObjectClass        containerClass );
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AccessAllProperties( Mac::AEPropertyID         propertyID,
	                     const Mac::AEDesc_Token&  containerToken,
	                     Mac::AEObjectClass        containerClass );
	
	inline nucleus::owned< Mac::AEDesc_Token >
	//
	AccessAllProperties( const Mac::AEDesc_Token&  containerToken,
	                     Mac::AEObjectClass        containerClass )
	{
		return AccessAllProperties( Mac::AEPropertyID(), containerToken, containerClass );
	}
	
}

#endif
