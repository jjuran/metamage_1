// LSOpen.cp

#ifndef NITROGEN_LSOPEN_H
#include "Nitrogen/LSOpen.h"
#endif

namespace Nitrogen {

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
	
	
	void RegisterLaunchServicesErrors () {
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