/*
	command.hh
	----------
*/

#ifndef MACADB_COMMAND_HH
#define MACADB_COMMAND_HH


namespace mac {
namespace adb {
	
	typedef unsigned char Byte;
	typedef short OSErr;
	typedef char* Ptr;
	
	OSErr send_command( Ptr buffer, short command );
	
	inline
	OSErr send_command( Byte* buffer, short command )
	{
		return send_command( (Ptr) buffer, command );
	}
	
}
}

#endif
