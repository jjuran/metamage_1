/*	================
 *	GetObjectClass.h
 *	================
 */

#ifndef AEOBJECTMODEL_GETOBJECTCLASS_H
#define AEOBJECTMODEL_GETOBJECTCLASS_H

#include <map>

#include <AERegistry.h>

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	AEObjectClass GetObjectClass( const AEToken&  obj );
	
	#pragma mark -
	#pragma mark ¥ ObjectClassMap ¥
	
	template < DescType tokenType > struct ObjectClass_Traits;
	
	template < AEObjectClass objectClass > struct Basic_ObjectClass_Traits
	{
		static AEObjectClass ObjectClass()  { return objectClass; }
	};
	
	template <> struct ObjectClass_Traits< typeNull > : Basic_ObjectClass_Traits< AEObjectClass( cApplication ) > {};
	
	class ObjectClassMap
	{
		private:
			typedef std::map< DescType, AEObjectClass >  Map;
			
			Map map;
			
			// not implemented:
			ObjectClassMap( const ObjectClassMap& );
			ObjectClassMap& operator=( const ObjectClassMap& );
		
		public:
			ObjectClassMap();
			
			void Register( DescType tokenType, AEObjectClass objectClass )
			{
				map[ tokenType ] = objectClass;
			}
			
			template < DescType tokenType >
			void Register()
			{
				Register( tokenType, ObjectClass_Traits< tokenType >::ObjectClass() );
			}
			
			AEObjectClass GetObjectClass( DescType tokenType );
	};
	
	ObjectClassMap& TheGlobalObjectClassMap();
	
	inline void RegisterObjectClass( DescType tokenType, AEObjectClass objectClass )
	{
		TheGlobalObjectClassMap().Register( tokenType, objectClass );
	}
	
	template < DescType tokenType >
	void RegisterObjectClass()
	{
		TheGlobalObjectClassMap().template Register< tokenType >();
	}
	
	#pragma mark -
	#pragma mark ¥ ObjectClassGetter ¥
	
	template < DescType tokenType > struct GetObjectClass_Traits;
	
	class ObjectClassGetter
	{
		public:
			typedef AEObjectClass (*Callback)( const AEToken& );
		
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
			
			AEObjectClass GetObjectClass( const AEToken&  obj );
	};
	
	ObjectClassGetter& TheGlobalObjectClassGetter();
	
	inline void RegisterObjectClassGetter( DescType tokenType, ObjectClassGetter::Callback callback )
	{
		TheGlobalObjectClassGetter().Register( tokenType, callback );
	}
	
	template < DescType tokenType >
	void RegisterObjectClassGetter()
	{
		TheGlobalObjectClassGetter().template Register< tokenType >();
	}
	
}

#endif
