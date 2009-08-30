// Nitrogen/Devices.cp
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Devices.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( DeviceManager )
	
	
	static void RegisterDeviceManagerErrors();
	
	
#pragma force_active on
	
	class DeviceManagerErrorsRegistration
	{
		public:
			DeviceManagerErrorsRegistration()  { RegisterDeviceManagerErrors(); }
	};
	
	static DeviceManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
	void RegisterDeviceManagerErrors()
	{
		// System 7.6.1 has been witnessed returning eofErr from PBReadAsync().
		// To be on the safe side, we register the errors returned by PBRead()
		// and PBWrite() for files, in addition to devices.
		
		RegisterOSStatus< controlErr      >();
		RegisterOSStatus< statusErr       >();
		RegisterOSStatus< readErr         >();
		RegisterOSStatus< writErr         >();
		RegisterOSStatus< badUnitErr      >();
		RegisterOSStatus< unitEmptyErr    >();
		RegisterOSStatus< openErr         >();
		RegisterOSStatus< closErr         >();
		RegisterOSStatus< badMDBErr       >();
		RegisterOSStatus< dRemovErr       >();
		RegisterOSStatus< dInstErr        >();
		RegisterOSStatus< abortErr        >();
		RegisterOSStatus< notOpenErr      >();
		RegisterOSStatus< dskFulErr       >();
		RegisterOSStatus< ioErr           >();
		RegisterOSStatus< fnOpnErr        >();
		RegisterOSStatus< eofErr          >();
		RegisterOSStatus< posErr          >();
		RegisterOSStatus< wPrErr          >();
		RegisterOSStatus< fLckdErr        >();
		RegisterOSStatus< vLckdErr        >();
		RegisterOSStatus< paramErr        >();
		RegisterOSStatus< rfNumErr        >();
		RegisterOSStatus< wrPermErr       >();
		RegisterOSStatus< afpAccessDenied >();
	}
	
}

