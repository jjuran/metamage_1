/*	==============
 *	ADBProtocol.cc
 *	==============
 */

#include "ADBProtocol.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;


const UInt8 kMaxRegisterIndex = 3;

const UInt8 kADBListenCommand =  8;
const UInt8 kADBTalkCommand   = 12;


static pascal void ADBCompletion( ::Ptr buffer, ::Ptr refCon, long command )
{
	if ( refCon != NULL )
	{
		*refCon = true;
	}
}

static void SendADBCommandSync( char* buffer, UInt8 command )
{
	static NN::Owned< N::ADBCompletionUPP > upp( N::NewADBCompletionUPP( ADBCompletion ) );
	
	char refCon = false;
	
	N::ADBOp( &refCon, upp, buffer, command );
	
	volatile const char& done = refCon;
	
	while ( !done )
	{
		continue;
	}
}

ADBRegister GetADBRegister( N::ADBAddress address, UInt8 index )
{
	ASSERT( index <= kMaxRegisterIndex );
	
	ADBRegister result;
	
	UInt8 command = address << 4 | kADBTalkCommand | index;

	SendADBCommandSync( result.buffer, command );
	
	return result;
}

void SetADBRegister( N::ADBAddress address, UInt8 index, const ADBRegister& value )
{
	ASSERT( index <= kMaxRegisterIndex );
	
	UInt8 command = address << 4 | kADBListenCommand | index;
	
	// Copy probably not necessary to enforce const, but cost is negligible
	ADBRegister reg = value;
	
	SendADBCommandSync( reg.buffer, command );
}

