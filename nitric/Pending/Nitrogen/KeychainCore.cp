// KeychainCore.cp

#ifndef NITROGEN_KEYCHAINCORE_H
#include "Nitrogen/KeychainCore.h"
#endif

namespace Nitrogen {

	void RegisterKeychainCoreErrors () {
		RegisterOSStatus< errKCNotAvailable          >();
		RegisterOSStatus< errKCReadOnly              >();
		RegisterOSStatus< errKCAuthFailed            >();
		RegisterOSStatus< errKCNoSuchKeychain        >();
		RegisterOSStatus< errKCInvalidKeychain       >();
		RegisterOSStatus< errKCDuplicateKeychain     >();
		RegisterOSStatus< errKCDuplicateCallback     >();
		RegisterOSStatus< errKCInvalidCallback       >();
		RegisterOSStatus< errKCDuplicateItem         >();
		RegisterOSStatus< errKCItemNotFound          >();
		RegisterOSStatus< errKCBufferTooSmall        >();
		RegisterOSStatus< errKCDataTooLarge          >();
		RegisterOSStatus< errKCNoSuchAttr            >();
		RegisterOSStatus< errKCInvalidItemRef        >();
		RegisterOSStatus< errKCInvalidSearchRef      >();
		RegisterOSStatus< errKCNoSuchClass           >();
		RegisterOSStatus< errKCNoDefaultKeychain     >();
		RegisterOSStatus< errKCInteractionNotAllowed >();
		RegisterOSStatus< errKCReadOnlyAttr          >();
		RegisterOSStatus< errKCWrongKCVersion        >();
		RegisterOSStatus< errKCKeySizeNotAllowed     >();
		RegisterOSStatus< errKCNoStorageModule       >();
		RegisterOSStatus< errKCNoCertificateModule   >();
		RegisterOSStatus< errKCNoPolicyModule        >();
		RegisterOSStatus< errKCInteractionRequired   >();
		RegisterOSStatus< errKCDataNotAvailable      >();
		RegisterOSStatus< errKCDataNotModifiable     >();
		RegisterOSStatus< errKCCreateChainFailed     >();
		}

	}