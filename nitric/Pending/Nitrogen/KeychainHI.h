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

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen {

	void RegisterKeychainHIErrors ();

	inline Owned<KCItemRef> KCAddAppleSharePassword (
				AFPServerSignature *serverSignature, StringPtr serverAddress,
				StringPtr serverName, StringPtr volumeName, StringPtr accountName,
				UInt32 passwordLength, const void *passwordData ) {
		OnlyOnce<RegisterKeychainHIErrors>();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddAppleSharePassword ( serverSignature, serverAddress, 
				serverName, volumeName, accountName, passwordLength, passwordData, &result ));
  	    return Owned<KCItemRef>::Seize( result );
		}
		
	inline Owned<KCItemRef> KCAddInternetPassword (
				StringPtr serverName, StringPtr securityDomain, StringPtr accountName,
				UInt16 port, OSType protocol, OSType authType,
				UInt32 passwordLength, const void *passwordData ) {
		OnlyOnce<RegisterKeychainHIErrors>();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddInternetPassword ( serverName, securityDomain, accountName, 
				port, protocol, authType, passwordLength, passwordData, &result ));
  	    return Owned<KCItemRef>::Seize( result );
		}
	
	inline Owned<KCItemRef> KCAddGenericPassword ( StringPtr serviceName, 
				StringPtr accountName, UInt32 passwordLength, const void *passwordData ) {
		OnlyOnce<RegisterKeychainHIErrors>();
		KCItemRef	result;
		ThrowOSStatus ( ::KCAddGenericPassword ( serviceName, accountName, 
				passwordLength, passwordData, &result ));
  	    return Owned<KCItemRef>::Seize( result );
		}
	
	inline void KCAddItem ( KCItemRef item ) {
		OnlyOnce<RegisterKeychainHIErrors>();
		ThrowOSStatus ( ::KCAddItem ( item ));
		}

	inline void KCUnlock ( KCRef keychain, ConstStr255Param password = NULL ) {
		OnlyOnce<RegisterKeychainHIErrors>();
		ThrowOSStatus ( ::KCUnlock ( keychain, const_cast<StringPtr> ( password )));
		}
	
	inline Owned<KCRef> KCCreateKeychain ( ConstStr255Param password = NULL ) {
		OnlyOnce<RegisterKeychainHIErrors>();
		KCRef	result;
		ThrowOSStatus ( ::KCCreateKeychain ( const_cast<StringPtr> ( password ), &result ));
  	    return Owned<KCRef>::Seize( result );
		}

	inline void KCChangeSettings ( KCRef keychain ) {
		OnlyOnce<RegisterKeychainHIErrors>();
		ThrowOSStatus ( ::KCChangeSettings ( keychain ));
		}
	}

#endif

