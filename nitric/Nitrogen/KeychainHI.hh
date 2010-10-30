// Nitrogen/KeychainHI.hh
// ----------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_KEYCHAINHI_HH
#define NITROGEN_KEYCHAINHI_HH

#ifndef __KEYCHAINHI__
#include <KeychainHI.h>
#endif

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

#ifndef NITROGEN_KEYCHAINCORE_HH
#include "Nitrogen/KeychainCore.hh"
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

namespace Nitrogen {

	inline nucleus::owned<KCItemRef> KCAddAppleSharePassword (
				AFPServerSignature *serverSignature, StringPtr serverAddress,
				StringPtr serverName, StringPtr volumeName, StringPtr accountName,
				UInt32 passwordLength, const void *passwordData ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddAppleSharePassword ( serverSignature, serverAddress, 
				serverName, volumeName, accountName, passwordLength, passwordData, &result ));
  	    return nucleus::owned<KCItemRef>::seize( result );
		}
		
	inline nucleus::owned<KCItemRef> KCAddInternetPassword (
				StringPtr serverName, StringPtr securityDomain, StringPtr accountName,
				UInt16 port, OSType protocol, OSType authType,
				UInt32 passwordLength, const void *passwordData ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddInternetPassword ( serverName, securityDomain, accountName, 
				port, protocol, authType, passwordLength, passwordData, &result ));
  	    return nucleus::owned<KCItemRef>::seize( result );
		}
	
	inline nucleus::owned<KCItemRef> KCAddGenericPassword ( StringPtr serviceName, 
				StringPtr accountName, UInt32 passwordLength, const void *passwordData ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddGenericPassword ( serviceName, accountName, 
				passwordLength, passwordData, &result ));
  	    return nucleus::owned<KCItemRef>::seize( result );
		}
	
	inline void KCAddItem ( KCItemRef item ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCAddItem ( item ));
		}

	inline void KCUnlock ( KCRef keychain, ConstStr255Param password = NULL ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCUnlock ( keychain, const_cast<StringPtr> ( password )));
		}
	
	inline nucleus::owned<KCRef> KCCreateKeychain ( ConstStr255Param password = NULL ) {
		(void) KeychainErrorsRegistrationDependency();
		KCRef	result;
		ThrowOSStatus ( ::KCCreateKeychain ( const_cast<StringPtr> ( password ), &result ));
  	    return nucleus::owned<KCRef>::seize( result );
		}

	inline void KCChangeSettings ( KCRef keychain ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCChangeSettings ( keychain ));
		}
	}

#endif

