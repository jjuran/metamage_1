/*	================
 *	GetObjectClass.h
 *	================
 */

#ifndef AEOBJECTMODEL_GETOBJECTCLASS_H
#define AEOBJECTMODEL_GETOBJECTCLASS_H

#include <map>

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	Mac::AEObjectClass GetObjectClass( const AEDesc_Token& obj );
	
	#pragma mark -
	#pragma mark ** ObjectClassMap **
	
	template < DescType tokenType > struct ObjectClass_Traits;
	
	template < Mac::AEObjectClass objectClass > struct Basic_ObjectClass_Traits
	{
		static Mac::AEObjectClass ObjectClass()  { return objectClass; }
	};
	
	template <> struct ObjectClass_Traits< typeNull > : Basic_ObjectClass_Traits< Mac::cApplication > {};
	
	class ObjectClassMap
	{
		private:
			typedef std::map< DescType, Mac::AEObjectClass >  Map;
			
			Map map;
			
			// not implemented:
			ObjectClassMap( const ObjectClassMap& );
			ObjectClassMap& operator=( const ObjectClassMap& );
		
		public:
			ObjectClassMap();
			
			void Register( DescType tokenType, Mac::AEObjectClass objectClass )
			{
				map[ tokenType ] = objectClass;
			}
			
			template < DescType tokenType >
			void Register()
			{
				Register( tokenType, ObjectClass_Traits< tokenType >::ObjectClass() );
			}
			
			Mac::AEObjectClass GetObjectClass( DescType tokenType );
	};
	
	ObjectClassMap& TheGlobalObjectClassMap();
	
	inline void RegisterObjectClass( DescType tokenType, Mac::AEObjectClass objectClass )
	{
		TheGlobalObjectClassMap().Register( tokenType, objectClass );
	}
	
	template < DescType tokenType >
	inline void RegisterObjectClass()
	{
		TheGlobalObjectClassMap().template Register< tokenType >();
	}
	
	#pragma mark -
	#pragma mark ** ObjectClassGetter **
	
	template < DescType tokenType > struct GetObjectClass_Traits;
	
	class ObjectClassGetter
	{
		public:
			typedef Mac::AEObjectClass (*Callback)( const AEDesc_Token& );
		
		private:
			typedef std::map< DescType, Callback >  Map;
			
			Map map;
			
			// not implemented:
			ObjectClassGetter( const ObjectClassGetter& );
			ObjectClassGetter& operator=( const ObjectClassGetter& );
		
		public:
			ObjectClassGetter();
			
			void Register( DescType tokenType, ObjectClassGetter::Callback callback )
			{
				map[ tokenType ] = callback;
			}
			
			template < DescType tokenType >
			void Register()
			{
				Register( tokenType, GetObjectClass_Traits< tokenType >::GetObjectClass );
			}
			
			Mac::AEObjectClass GetObjectClass( const AEDesc_Token& obj );
	};
	
	ObjectClassGetter& TheGlobalObjectClassGetter();
	
	inline void RegisterObjectClassGetter( DescType tokenType, ObjectClassGetter::Callback callback )
	{
		TheGlobalObjectClassGetter().Register( tokenType, callback );
	}
	
	template < DescType tokenType >
	inline void RegisterObjectClassGetter()
	{
		TheGlobalObjectClassGetter().template Register< tokenType >();
	}
	
}

#endif
