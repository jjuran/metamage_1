/*	=============
 *	PPCToolbox.cc
 *	=============
 */

// *** A-line Processing Instructions ***
// API: ClassicMacToolbox (not Carbon)

#include "ClassicToolbox/PPCToolbox.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __SCRIPT__
#include <Script.h>
#endif

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( PPCToolbox )
	
	
	static void RegisterPPCToolboxErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class PPCToolboxErrorsRegistration
	{
		public:
			PPCToolboxErrorsRegistration()  { RegisterPPCToolboxErrors(); }
	};
	
	static PPCToolboxErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
}

namespace nucleus
{
	
	template <>
	PPCXTIAddress maker< PPCXTIAddress >::operator()( const char* address_data,
	                                                  std::size_t address_size ) const
	{
		if ( address_size > kMaxPPCXTIAddress )
		{
			// FIXME:  Should throw or something
			
			address_size = kMaxPPCXTIAddress;
		}
		
		PPCXTIAddress xtiAddr;
		
		xtiAddr.fAddressType = kDNSAddrType;
		
		void* p = mempcpy( xtiAddr.fAddress, address_data, address_size );
		
		*(char*) p = '\0';
		
		return xtiAddr;
	}
	
	template <>
	PPCAddrRec maker< PPCAddrRec >::operator()( const PPCXTIAddress& xtiAddr ) const
	{
		size_t size = strlen( (const char*) xtiAddr.fAddress ) + 1;
		
		PPCAddrRec xti;
		
		xti.Reserved[0] = 0;
		xti.Reserved[1] = 0;
		xti.Reserved[2] = 0;
		xti.xtiAddrLen = sizeof (::PPCXTIAddressType) + size;
		xti.xtiAddr = xtiAddr;
		
		return xti;
	}
	
	template <>
	PPCPortRec maker< PPCPortRec >::operator()( ConstStr32Param name, ConstStr32Param portTypeStr ) const
	{
		PPCPortRec port;
		
		port.nameScript = smSystemScript;  // FIXME
		
		Nitrogen::CopyToPascalString( name,        port.name,          32 );
		Nitrogen::CopyToPascalString( portTypeStr, port.u.portTypeStr, 32 );
		
		port.portKindSelector = ppcByString;
		
		return port;
	}
	
}

namespace Nitrogen
{
	
	void IPCListPortsSync( IPCListPortsPBRec& pb )
	{
		Mac::ThrowOSStatus( ::IPCListPortsSync( &pb ) );
	}
	
	PortInfoRec IPCListPortsSync( const PPCPortRec& name, const LocationNameRec& location )
	{
		PortInfoRec portInfo;
		
		IPCListPortsPBRec pb = nucleus::make< IPCListPortsPBRec >( &name, &location, &portInfo );
		
		IPCListPortsSync( pb );
		
		return portInfo;
	}
	
	PortInfoRec IPCListPortsSync( const PPCPortRec& name )
	{
		PortInfoRec portInfo;
		
		IPCListPortsPBRec pb = nucleus::make< IPCListPortsPBRec >( &name, &portInfo );
		
		IPCListPortsSync( pb );
		
		return portInfo;
	}
	
	void RegisterPPCToolboxErrors()
	{
		RegisterOSStatus< paramErr            >();
		RegisterOSStatus< memFullErr          >();
		RegisterOSStatus< userCanceledErr     >();
		RegisterOSStatus< notInitErr          >();
		RegisterOSStatus< nameTypeErr         >();
		RegisterOSStatus< noPortErr           >();
		RegisterOSStatus< noGlobalsErr        >();
		RegisterOSStatus< localOnlyErr        >();
		RegisterOSStatus< destPortErr         >();
		RegisterOSStatus< sessTableErr        >();
		RegisterOSStatus< noSessionErr        >();
		RegisterOSStatus< badReqErr           >();
		RegisterOSStatus< portNameExistsErr   >();
		RegisterOSStatus< noUserNameErr       >();
		RegisterOSStatus< userRejectErr       >();
		RegisterOSStatus< noResponseErr       >();
		RegisterOSStatus< portClosedErr       >();
		RegisterOSStatus< sessClosedErr       >();
		RegisterOSStatus< badPortNameErr      >();
		RegisterOSStatus< noDefaultUserErr    >();
		RegisterOSStatus< notLoggedInErr      >();
		RegisterOSStatus< noUserRefErr        >();
		RegisterOSStatus< networkErr          >();
		RegisterOSStatus< noInformErr         >();
		RegisterOSStatus< authFailErr         >();
		RegisterOSStatus< noUserRecErr        >();
		RegisterOSStatus< badServiceMethodErr >();
		RegisterOSStatus< badLocNameErr       >();
		RegisterOSStatus< guestNotAllowedErr  >();
		RegisterOSStatus< nbpDuplicate        >();
	}
	
}
