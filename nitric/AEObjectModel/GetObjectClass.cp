/*	=================
 *	GetObjectClass.cp
 *	=================
 */

#ifndef AEOBJECTMODEL_GETOBJECTCLASS_H
#include "AEObjectModel/GetObjectClass.h"
#endif


namespace Nitrogen
{
	
	AEObjectClass GetObjectClass( const AEDesc_Token& obj )
	{
		try
		{
			return TheGlobalObjectClassMap().GetObjectClass( DescType( obj.descriptorType ) );
		}
		catch ( const OSStatus& err )
		{
			if ( err != errAEEventNotHandled )
			{
				throw;
			}
		}
		
		try
		{
			return TheGlobalObjectClassGetter().GetObjectClass( obj );
		}
		catch ( const OSStatus& err )
		{
			if ( err != errAEEventNotHandled )
			{
				throw;
			}
		}
		
		return AEObjectClass( obj.descriptorType );
	}
	
	#pragma mark -
	#pragma mark ** ObjectClassMap **
	
	ObjectClassMap::ObjectClassMap()
	{
		Register< typeNull >();
	}
	
	AEObjectClass ObjectClassMap::GetObjectClass( DescType tokenType )
	{
		Map::const_iterator found = map.find( tokenType );
		
		if ( found == map.end() )
		{
			ThrowOSStatus( errAEEventNotHandled );
		}
		
		return found->second;
	}
	
	ObjectClassMap& TheGlobalObjectClassMap()
	{
		static ObjectClassMap theGlobalObjectClassMap;
		
		return theGlobalObjectClassMap;
	}
	
	#pragma mark -
	#pragma mark ** ObjectClassGetter **
	
	ObjectClassGetter::ObjectClassGetter()
	{
	}
	
	AEObjectClass ObjectClassGetter::GetObjectClass( const AEDesc_Token& obj )
	{
		Map::const_iterator found = map.find( DescType( obj.descriptorType ) );
		
		if ( found == map.end() )
		{
			//throw ErrAEEventNotHandled();
			return AEObjectClass( obj.descriptorType );
		}
		
		return found->second( obj );
	}
	
	ObjectClassGetter& TheGlobalObjectClassGetter()
	{
		static ObjectClassGetter theGlobalObjectClassGetter;
		
		return theGlobalObjectClassGetter;
	}
	
}

