/*
	device_handler_ID.hh
	--------------------
*/

#ifndef MACADB_DEVICEHANDLERID_HH
#define MACADB_DEVICEHANDLERID_HH

// mac-adb-utils
#include "mac_adb/register.hh"


namespace mac {
namespace adb {
	
	enum
	{
		kDeviceHandlerIDRegisterIndex = 3,
		kDeviceHandlerIDByteIndex     = 2,
	};
	
	inline
	OSErr get_device_handler_ID( address_t address )
	{
		OSErr err;
		reg_buffer reg;
		
		err = get_register( reg, address, kDeviceHandlerIDRegisterIndex );
		
		return err ? err : reg.data[ kDeviceHandlerIDByteIndex ];
	}
	
	inline
	OSErr set_device_handler_ID( address_t address, Byte id )
	{
		OSErr err;
		reg_buffer reg;
		
		err = get_register( reg, address, kDeviceHandlerIDRegisterIndex );
		
		if ( err == 0 )  // noErr
		{
			reg.data[ kDeviceHandlerIDByteIndex ] = id;
		
			err = set_register( reg, address, kDeviceHandlerIDRegisterIndex );
		}
		
		return err;
	}
	
}
}

#endif
