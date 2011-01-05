/*	==========
 *	SetData.cp
 *	==========
 */

#ifndef AEOBJECTMODEL_SETDATA_H
#include "AEObjectModel/SetData.h"
#endif

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	void SetData( const Mac::AEDesc_Token& obj, const Mac::AEDesc_Data& data )
	{
		return TheGlobalDataSetter().SetData( obj, data );
	}
	
	DataSetter::DataSetter()
	{
	}
	
	void DataSetter::SetData( const Mac::AEDesc_Token& obj, const Mac::AEDesc_Data& data )
	{
		Map::const_iterator found = map.find( Mac::DescType( obj.descriptorType ) );
		
		if ( found == map.end() )
		{
			Mac::ThrowOSStatus( errAEEventNotHandled );
		}
		
		return found->second( obj, data );
	}
	
	DataSetter& TheGlobalDataSetter()
	{
		static DataSetter theGlobalDataSetter;
		
		return theGlobalDataSetter;
	}
	
}

