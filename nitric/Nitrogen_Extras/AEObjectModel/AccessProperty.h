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

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ¥ Property traits ¥
	
	template < AEPropertyID propertyID, DescType tokenType >  struct Property_Traits;
	
	template < AEPropertyID propertyID, DescType tokenType >
	struct Basic_PropertyAccessor
	{
		typedef typename DescType_Traits< tokenType >::Result RecordPtr;
		typedef Property_Traits< propertyID, tokenType > PropertyTraits;
		typedef typename PropertyTraits::Record Record;
		typedef typename PropertyTraits::Field  Field;
		typedef Field Record::* Member;
		
		enum { descType = PropertyTraits::descType };
		
		typedef typename DescType_Traits< descType >::Result Result;
		
		static Nucleus::Owned< AEDesc_Token >
		AccessProperty( AEPropertyID         /* propertyID */,
	                    const AEDesc_Token&  containerToken,
	                    AEObjectClass        /* containerClass */ )
		{
			RecordPtr record = AEGetDescData< tokenType >( containerToken );
			Member member = PropertyTraits::Member();
			Field fieldValue = record->*member;
			Result result = Nucleus::Convert< Result >( fieldValue );
			
			return AECreateDesc< AEDesc_Token, descType >( result );
		}
	};
	
	#pragma mark -
	#pragma mark ¥ AccessProperty ¥
	
	Nucleus::Owned< AEDesc_Token > AccessProperty( AEPropertyID         propertyID,
	                                               const AEDesc_Token&  containerToken,
	                                               AEObjectClass        containerClass );
	
	class PropertyAccessor
	{
		public:
			typedef Nucleus::Owned< AEDesc_Token > (*Callback)( AEPropertyID, const AEDesc_Token&, AEObjectClass );
		
		private:
			typedef std::map< AEPropertyID, Callback >  PropertyMap;
			typedef std::map< DescType, PropertyMap >  Map;
			
			Map map;
			
			// not implemented:
			PropertyAccessor( const PropertyAccessor& );
			PropertyAccessor& operator=( const PropertyAccessor& );
		
		public:
			PropertyAccessor();
			
			void Register( AEPropertyID propertyID, DescType containerType, PropertyAccessor::Callback callback )
			{
				map[ containerType ][ propertyID ] = callback;
			}
			
			template < AEPropertyID propertyID, DescType containerType >
			void Register()
			{
				Register( propertyID, containerType, Basic_PropertyAccessor< propertyID, containerType >::AccessProperty );
			}
			
			Callback FindAccessor( AEPropertyID  propertyID, DescType tokenType );
			
			Nucleus::Owned< AEDesc_Token > AccessProperty( AEPropertyID         propertyID,
			                                               const AEDesc_Token&  containerToken,
			                                               AEObjectClass        containerClass );
			
			Nucleus::Owned< AEDesc_Token > AccessAll( const AEDesc_Token&  containerToken,
			                                          AEObjectClass        containerClass );
	};
	
	PropertyAccessor& TheGlobalPropertyAccessor();
	
	inline void RegisterPropertyAccessor( AEPropertyID propertyID, DescType containerType, PropertyAccessor::Callback callback )
	{
		TheGlobalPropertyAccessor().Register( propertyID, containerType, callback );
	}
	
	template < AEPropertyID propertyID, DescType containerType >
	inline void RegisterPropertyAccessor()
	{
		TheGlobalPropertyAccessor().template Register< propertyID, containerType >();
	}
	
	Nucleus::Owned< AEDesc_Token > AccessClassProperty( AEPropertyID         /* propertyID */,
	                                                    const AEDesc_Token&  /* containerToken */,
	                                                    AEObjectClass        containerClass );
	
	Nucleus::Owned< AEDesc_Token > AccessAllProperties( AEPropertyID         propertyID,
	                                                    const AEDesc_Token&  containerToken,
	                                                    AEObjectClass        containerClass );
	
	inline Nucleus::Owned< AEDesc_Token > AccessAllProperties( const AEDesc_Token&  containerToken,
	                                                           AEObjectClass        containerClass )
	{
		return AccessAllProperties( AEPropertyID(), containerToken, containerClass );
	}
	
}

#endif
