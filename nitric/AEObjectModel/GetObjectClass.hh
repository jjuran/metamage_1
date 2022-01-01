/*
	GetObjectClass.hh
	-----------------
*/

#ifndef AEOBJECTMODEL_GETOBJECTCLASS_HH
#define AEOBJECTMODEL_GETOBJECTCLASS_HH

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

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEDESC_HH
#include "Mac/AppleEvents/Types/AEDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEOBJECTCLASS_HH
#include "Mac/AppleEvents/Types/AEObjectClass.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif


namespace Nitrogen
{
	
	Mac::AEObjectClass GetObjectClass( const Mac::AEDesc_Token& obj );
	
	#pragma mark -
	#pragma mark ** ObjectClassMap **
	
	template < Mac::DescType tokenType > struct ObjectClass_Traits;
	
	template < Mac::AEObjectClass objectClass > struct Basic_ObjectClass_Traits
	{
		static Mac::AEObjectClass ObjectClass()  { return objectClass; }
	};
	
	template <> struct ObjectClass_Traits< Mac::typeNull > : Basic_ObjectClass_Traits< Mac::cApplication > {};
	
	class ObjectClassMap
	{
		private:
			typedef std::map< Mac::DescType, Mac::AEObjectClass >  Map;
			
			Map map;
			
			// not implemented:
			ObjectClassMap( const ObjectClassMap& );
			ObjectClassMap& operator=( const ObjectClassMap& );
		
		public:
			ObjectClassMap();
			
			void Register( Mac::DescType tokenType, Mac::AEObjectClass objectClass )
			{
				map[ tokenType ] = objectClass;
			}
			
			template < Mac::DescType tokenType >
			void Register()
			{
				Register( tokenType, ObjectClass_Traits< tokenType >::ObjectClass() );
			}
			
			Mac::AEObjectClass GetObjectClass( Mac::DescType tokenType );
	};
	
	ObjectClassMap& TheGlobalObjectClassMap();
	
	inline void RegisterObjectClass( Mac::DescType tokenType, Mac::AEObjectClass objectClass )
	{
		TheGlobalObjectClassMap().Register( tokenType, objectClass );
	}
	
	template < Mac::DescType tokenType >
	inline void RegisterObjectClass()
	{
		TheGlobalObjectClassMap().template Register< tokenType >();
	}
	
	#pragma mark -
	#pragma mark ** ObjectClassGetter **
	
	class ObjectClassGetter
	{
		public:
			typedef Mac::AEObjectClass (*Callback)( const Mac::AEDesc_Token& );
		
		private:
			typedef std::map< Mac::DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			ObjectClassGetter( const ObjectClassGetter& );
			ObjectClassGetter& operator=( const ObjectClassGetter& );
		
		public:
			ObjectClassGetter();
			
			void Register( Mac::DescType tokenType, ObjectClassGetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			Mac::AEObjectClass GetObjectClass( const Mac::AEDesc_Token& obj );
	};
	
	ObjectClassGetter& TheGlobalObjectClassGetter();
	
	inline void RegisterObjectClassGetter( Mac::DescType tokenType, ObjectClassGetter::Callback callback )
	{
		TheGlobalObjectClassGetter().Register( tokenType, callback );
	}
	
}

#endif
