/*	==============
 *	ADBProtocol.cc
 *	==============
 */

#if !TARGET_API_MAC_CARBON

#include "ADBProtocol.hh"

// Nucleus
#include "Nucleus/NAssert.h"


namespace N = Nitrogen;
namespace NN = Nucleus;


const UInt8 kADBListenCommand =  8;
const UInt8 kADBTalkCommand   = 12;

const UInt8 kMaxRegisterOffset = 3;

const UInt8 kDeviceHandlerIDRegisterOffset = 3;
const UInt8 kDeviceHandlerIDByteIndex      = 2;


namespace
{
	
	pascal void ADBCompletion( ::Ptr buffer, ::Ptr refCon, long command )
	{
		if ( refCon != NULL )
		{
			*refCon = true;
		}
	}
	
}

static void SendADBCommandSync( char* buffer, UInt8 command )
{
	char refCon = false;
	
	N::ADBOp< ADBCompletion >( &refCon, buffer, command );
	
	volatile const char& done = refCon;
	
	while ( !done )
	{
		continue;
	}
}

ADBRegister GetADBRegister( N::ADBAddress address, UInt8 index )
{
	ASSERT( index <= kMaxRegisterOffset );
	
	ADBRegister result;
	
	UInt8 command = address << 4 | kADBTalkCommand | index;

	SendADBCommandSync( result.buffer, command );
	
	return result;
}

void SetADBRegister( N::ADBAddress address, UInt8 index, const ADBRegister& value )
{
	ASSERT( index <= kMaxRegisterOffset );
	
	UInt8 command = address << 4 | kADBListenCommand | index;
	
	// Copy probably not necessary to enforce const, but cost is negligible
	ADBRegister reg = value;
	
	SendADBCommandSync( reg.buffer, command );
}


UInt8 GetDeviceHandlerID( N::ADBAddress address )
{
	UInt8 id = GetADBRegister( address, kDeviceHandlerIDRegisterOffset ).buffer[ kDeviceHandlerIDByteIndex ];
	
	return id;
}

void SetDeviceHandlerID( N::ADBAddress address, UInt8 id )
{
	ADBRegister reg = GetADBRegister( address, kDeviceHandlerIDRegisterOffset );
	
	reg.buffer[ kDeviceHandlerIDByteIndex ] = id;

	SetADBRegister( address, kDeviceHandlerIDRegisterOffset, reg );
}

#endif

