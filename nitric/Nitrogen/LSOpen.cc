// Nitrogen/LSOpen.cc
// ------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifdef __MACH__

#include "Nitrogen/LSOpen.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	LaunchServicesErrorsRegistrationDependency::LaunchServicesErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterLaunchServicesErrors();
	
	
	class LaunchServicesErrorsRegistration
	{
		public:
			LaunchServicesErrorsRegistration()  { RegisterLaunchServicesErrors(); }
	};
	
	static LaunchServicesErrorsRegistration theRegistration;
	
	
	void RegisterLaunchServicesErrors()
	{
		RegisterOSStatus< kLSAppInTrashErr                   >();
		RegisterOSStatus< kLSUnknownErr                      >();
		RegisterOSStatus< kLSNotAnApplicationErr             >();
		RegisterOSStatus< kLSNotInitializedErr               >();
		RegisterOSStatus< kLSDataUnavailableErr              >();
		RegisterOSStatus< kLSApplicationNotFoundErr          >();
		RegisterOSStatus< kLSUnknownTypeErr                  >();
		RegisterOSStatus< kLSDataTooOldErr                   >();
		RegisterOSStatus< kLSDataErr                         >();
		RegisterOSStatus< kLSLaunchInProgressErr             >();
		RegisterOSStatus< kLSNotRegisteredErr                >();
		RegisterOSStatus< kLSAppDoesNotClaimTypeErr          >();
		RegisterOSStatus< kLSAppDoesNotSupportSchemeWarning  >();
		RegisterOSStatus< kLSServerCommunicationErr          >();
		RegisterOSStatus< kLSCannotSetInfoErr                >();
		RegisterOSStatus< kLSNoRegistrationInfoErr           >();
		RegisterOSStatus< kLSIncompatibleSystemVersionErr    >();
		RegisterOSStatus< kLSNoLaunchPermissionErr           >();
		RegisterOSStatus< kLSNoExecutableErr                 >();
		RegisterOSStatus< kLSNoClassicEnvironmentErr         >();
		RegisterOSStatus< kLSMultipleSessionsNotSupportedErr >();
	}
	
}

#endif
