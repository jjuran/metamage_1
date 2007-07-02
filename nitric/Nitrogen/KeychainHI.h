// Nitrogen/KeychainHI.h
// ---------------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Marshall Clow and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_KEYCHAINHI_H
#define	NITROGEN_KEYCHAINHI_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __KEYCHAINHI__
#include <SecurityHI/KeychainHI.h>
#endif

#ifndef NITROGEN_KEYCHAINCORE_H
#include "Nitrogen/KeychainCore.h"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen {

	inline Nucleus::Owned<KCItemRef> KCAddAppleSharePassword (
				AFPServerSignature *serverSignature, StringPtr serverAddress,
				StringPtr serverName, StringPtr volumeName, StringPtr accountName,
				UInt32 passwordLength, const void *passwordData ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddAppleSharePassword ( serverSignature, serverAddress, 
				serverName, volumeName, accountName, passwordLength, passwordData, &result ));
  	    return Nucleus::Owned<KCItemRef>::Seize( result );
		}
		
	inline Nucleus::Owned<KCItemRef> KCAddInternetPassword (
				StringPtr serverName, StringPtr securityDomain, StringPtr accountName,
				UInt16 port, OSType protocol, OSType authType,
				UInt32 passwordLength, const void *passwordData ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddInternetPassword ( serverName, securityDomain, accountName, 
				port, protocol, authType, passwordLength, passwordData, &result ));
  	    return Nucleus::Owned<KCItemRef>::Seize( result );
		}
	
	inline Nucleus::Owned<KCItemRef> KCAddGenericPassword ( StringPtr serviceName, 
				StringPtr accountName, UInt32 passwordLength, const void *passwordData ) {
		(void) KeychainErrorsRegistrationDependency();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddGenericPassword ( serviceName, accountName, 
				passwordLength, passwordData, &result ));
  	    return Nucleus::Owned<KCItemRef>::Seize( result );
		}
	
	inline void KCAddItem ( KCItemRef item ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCAddItem ( item ));
		}

	inline void KCUnlock ( KCRef keychain, ConstStr255Param password = NULL ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCUnlock ( keychain, const_cast<StringPtr> ( password )));
		}
	
	inline Nucleus::Owned<KCRef> KCCreateKeychain ( ConstStr255Param password = NULL ) {
		(void) KeychainErrorsRegistrationDependency();
		KCRef	result;
		ThrowOSStatus ( ::KCCreateKeychain ( const_cast<StringPtr> ( password ), &result ));
  	    return Nucleus::Owned<KCRef>::Seize( result );
		}

	inline void KCChangeSettings ( KCRef keychain ) {
		(void) KeychainErrorsRegistrationDependency();
		ThrowOSStatus ( ::KCChangeSettings ( keychain ));
		}
	}

#endif

