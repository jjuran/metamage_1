/*	==============
 *	ADBProtocol.cc
 *	==============
 */

#if !TARGET_API_MAC_CARBON

#include "ADB/Protocol.hh"

// mac-config
#include "mac_config/upp-macros.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace N = Nitrogen;


const UInt8 kADBListenCommand =  8;
const UInt8 kADBTalkCommand   = 12;

const UInt8 kMaxRegisterOffset = 3;

const UInt8 kDeviceHandlerIDRegisterOffset = 3;
const UInt8 kDeviceHandlerIDByteIndex      = 2;


#if TARGET_CPU_68K

static
asm pascal void ADBCompletion()
{
	// buffer  in A0
	// refCon  in A2 (we always pass a non-NULL pointer)
	// command in D0 (byte, assured to be non-zero)
	
	MOVE.B   D0,(A2)  // The command is a byte, assured to be non-zero
	RTS
}

#else

static
pascal void ADBCompletion( ::Ptr buffer, ::Ptr refCon, long command )
{
	*refCon = true;
}

#endif

static void SendADBCommandSync( char* buffer, UInt8 command )
{
	DEFINE_UPP( ADBCompletion, ADBCompletion )
	
	char refCon = false;
	
	OSErr err = ::ADBOp( &refCon, UPP_ARG( ADBCompletion ), buffer, command );
	
	Mac::ThrowOSStatus( err );
	
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
