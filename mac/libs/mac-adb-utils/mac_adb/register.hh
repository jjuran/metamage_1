/*
	register.hh
	-----------
*/

#ifndef MACADB_REGISTER_HH
#define MACADB_REGISTER_HH

// mac-adb-utils
#include "mac_adb/command.hh"


namespace mac {
namespace adb {
	
	enum
	{
		kRegisterMaxLength  = 8,
		kRegisterBufferSize = 1 + kRegisterMaxLength,
		
		kADBListenCommand =  8,
		kADBTalkCommand   = 12,
	};
	
	typedef Byte address_t;
	
	struct reg_buffer
	{
		Byte data[ kRegisterBufferSize ];
	};
	
	inline
	OSErr get_register( Ptr buffer, address_t address, int index )
	{
		short command = address << 4 | kADBTalkCommand | index;
		
		return send_command( buffer, command );
	}
	
	inline
	OSErr get_register( Byte* buffer, address_t address, int index )
	{
		return get_register( (Ptr) buffer, address, index );
	}
	
	inline
	OSErr get_register( reg_buffer& buffer, address_t address, int index )
	{
		return get_register( buffer.data, address, index );
	}
	
	inline
	OSErr set_register( Ptr buffer, address_t address, int index )
	{
		short command = address << 4 | kADBListenCommand | index;
		
		return send_command( buffer, command );
	}
	
	inline
	OSErr set_register( Byte* buffer, address_t address, int index )
	{
		return set_register( (Ptr) buffer, address, index );
	}
	
	inline
	OSErr set_register( reg_buffer& buffer, address_t address, int index )
	{
		return set_register( buffer.data, address, index );
	}
	
}
}

#endif
