/*	==========
 *	SetData.cp
 *	==========
 */

#ifndef AEOBJECTMODEL_SETDATA_H
#include "AEObjectModel/SetData.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif


namespace Nitrogen
{
	
	void SetData( const AEToken& obj, const AEDesc& data )
	{
		return TheGlobalDataSetter().SetData( obj, data );
	}
	
	DataSetter::DataSetter()
	{
	}
	
	void DataSetter::SetData( const AEToken& obj, const AEDesc& data )
	{
		Map::const_iterator found = map.find( obj.descriptorType );
		if ( found == map.end() )
		{
			throw ErrAEEventNotHandled();
		}
		
		return found->second( obj, data );
	}
	
	DataSetter& TheGlobalDataSetter()
	{
		static DataSetter theGlobalDataSetter;
		return theGlobalDataSetter;
	}
	
}

