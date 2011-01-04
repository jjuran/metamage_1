/*	=================
 *	GetObjectClass.cp
 *	=================
 */

#ifndef AEOBJECTMODEL_GETOBJECTCLASS_H
#include "AEObjectModel/GetObjectClass.h"
#endif

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Nitrogen
{
	
	Mac::AEObjectClass GetObjectClass( const Mac::AEDesc_Token& obj )
	{
		try
		{
			return TheGlobalObjectClassMap().GetObjectClass( Mac::DescType( obj.descriptorType ) );
		}
		catch ( const Mac::OSStatus& err )
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
		catch ( const Mac::OSStatus& err )
		{
			if ( err != errAEEventNotHandled )
			{
				throw;
			}
		}
		
		return Mac::AEObjectClass( obj.descriptorType );
	}
	
	#pragma mark -
	#pragma mark ** ObjectClassMap **
	
	ObjectClassMap::ObjectClassMap()
	{
		Register< Mac::typeNull >();
	}
	
	Mac::AEObjectClass ObjectClassMap::GetObjectClass( Mac::DescType tokenType )
	{
		Map::const_iterator found = map.find( tokenType );
		
		if ( found == map.end() )
		{
			Mac::ThrowOSStatus( errAEEventNotHandled );
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
	
	Mac::AEObjectClass ObjectClassGetter::GetObjectClass( const Mac::AEDesc_Token& obj )
	{
		Map::const_iterator found = map.find( Mac::DescType( obj.descriptorType ) );
		
		if ( found == map.end() )
		{
			//throw ErrAEEventNotHandled();
			return Mac::AEObjectClass( obj.descriptorType );
		}
		
		return found->second( obj );
	}
	
	ObjectClassGetter& TheGlobalObjectClassGetter()
	{
		static ObjectClassGetter theGlobalObjectClassGetter;
		
		return theGlobalObjectClassGetter;
	}
	
}

