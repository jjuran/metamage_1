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
	
	
	void IPCListPortsSync( IPCListPortsPBRec& pb )
	{
		ThrowOSStatus( ::IPCListPortsSync( &pb ) );
	}
	
	PortInfoRec IPCListPortsSync( const PPCPortRec& name, const LocationNameRec& location )
	{
		PortInfoRec portInfo;
		
		IPCListPortsPBRec pb = Nucleus::Make< IPCListPortsPBRec >( &name, &location, &portInfo );
		
		IPCListPortsSync( pb );
		
		return portInfo;
	}
	
	PortInfoRec IPCListPortsSync( const PPCPortRec& name )
	{
		PortInfoRec portInfo;
		
		IPCListPortsPBRec pb = Nucleus::Make< IPCListPortsPBRec >( &name, &portInfo );
		
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

