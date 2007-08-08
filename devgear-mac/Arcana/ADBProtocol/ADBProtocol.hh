/*	==============
 *	ADBProtocol.hh
 *	==============
 */

// ClassicToolbox
#include "ClassicToolbox/DeskBus.h"


const std::size_t kMaxADBRegisterLength = 8;

const std::size_t kADBRegisterBufferSize = 1 + kMaxADBRegisterLength;


struct ADBRegister
{
	char buffer[ kADBRegisterBufferSize ];
	
	ADBRegister()  { buffer[0] = 0; }
};

ADBRegister GetADBRegister( Nitrogen::ADBAddress address, UInt8 index );

void SetADBRegister( Nitrogen::ADBAddress address, UInt8 index, const ADBRegister& value );

