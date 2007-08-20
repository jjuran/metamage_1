/*	==============
 *	ADBProtocol.hh
 *	==============
 */

#ifndef ADBPROTOCOL_HH
#define ADBPROTOCOL_HH

// ClassicToolbox
#include "ClassicToolbox/DeskBus.h"


static const std::size_t kMaxADBRegisterLength = 8;

static const std::size_t kADBRegisterBufferSize = 1 + kMaxADBRegisterLength;


struct ADBRegister
{
	char buffer[ kADBRegisterBufferSize ];
	
	ADBRegister()  { buffer[0] = 0; }
};


ADBRegister GetADBRegister( Nitrogen::ADBAddress address, UInt8 index );

void SetADBRegister( Nitrogen::ADBAddress address, UInt8 index, const ADBRegister& value );


UInt8 GetDeviceHandlerID( Nitrogen::ADBAddress address );

void SetDeviceHandlerID( Nitrogen::ADBAddress address, UInt8 id );

#endif

