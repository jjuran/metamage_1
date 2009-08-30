#ifndef NITROGEN_KEYCHAINCORE_H
#define	NITROGEN_KEYCHAINCORE_H

#if	!TARGET_RT_MAC_MACHO
#error "These routines are only directly callable from MachO"
#endif

#ifndef __KEYCHAINCORE__
#include <SecurityCore/KeychainCore.h>
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen {

	void RegisterKeychainCoreErrors ();
	
   template <> struct Disposer< KCRef >: public std::unary_function< KCRef, void >
     {
      void operator()( KCRef kc ) const
        {
         (void) ::KCReleaseKeychain ( &kc );
        }
     };

   template <> struct Disposer< KCItemRef >: public std::unary_function< KCItemRef, void >
     {
      void operator()( KCItemRef kci ) const
        {
         (void) ::KCReleaseItem ( &kci );
        }
     };

   template <> struct Disposer< KCSearchRef >: public std::unary_function< KCSearchRef, void >
     {
      void operator()( KCSearchRef kcs ) const
        {
         (void) ::KCReleaseSearch ( &kcs );
        }
     };


	inline void KCSetInteractionAllowed ( Boolean state ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		ThrowOSStatus ( ::KCSetInteractionAllowed ( state ));
		}
		
//	Boolean KCIsInteractionAllowed ( void );
	using ::KCIsInteractionAllowed;
	
	inline Owned<KCRef> KCMakeKCRefFromFSSpec ( const FSSpec &spec ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		KCRef	result;
		ThrowOSStatus ( ::KCMakeKCRefFromFSSpec ( const_cast <FSSpec *> ( &spec ), &result ));
  	    return Owned<KCRef>::Seize( result );
		}
	
	inline Owned<KCRef> KCMakeKCRefFromAlias ( AliasHandle keychainAlias ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		KCRef	result;
		ThrowOSStatus ( ::KCMakeKCRefFromAlias ( keychainAlias, &result ));
  	    return Owned<KCRef>::Seize( result );
		}
	
	inline Owned<AliasHandle> KCMakeAliasFromKCRef ( KCRef keychain ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		AliasHandle	result;
		ThrowOSStatus ( ::KCMakeAliasFromKCRef ( keychain, &result ));
  	    return Owned<AliasHandle>::Seize( result );
		}

//	extern OSStatus KCReleaseKeychain(KCRef * keychain);
	inline Owned<KCRef> KCGetDefaultKeychain () {
		OnlyOnce<RegisterKeychainCoreErrors>();
		KCRef	result;
		ThrowOSStatus ( ::KCGetDefaultKeychain ( &result ));
  	    return Owned<KCRef>::Seize( result );
		}
	
	inline void KCSetDefaultKeychain ( KCRef keychain ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		ThrowOSStatus ( ::KCSetDefaultKeychain ( keychain ));
		}
	
	inline UInt32 KCGetStatus ( KCRef keychain ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		UInt32	result;
		ThrowOSStatus ( ::KCGetStatus ( keychain, &result ));
  	    return result;
		}
	
	inline Owned<KCRef> KCGetKeychain ( KCItemRef item ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		KCRef	result;
		ThrowOSStatus ( ::KCGetKeychain ( item, &result ));
  	    return Owned<KCRef>::Seize( result );
		}


#if 0
/*
 *  KCGetKeychainName()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 */
extern OSStatus 
KCGetKeychainName(
  KCRef       keychain,
  StringPtr   keychainName)                                   AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif

//	extern UInt16 KCCountKeychains(void)
	using ::KCCountKeychains;

	inline Owned<KCRef> KCGetIndKeychain ( UInt16 idx ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		KCRef	result;
		ThrowOSStatus ( ::KCGetIndKeychain ( idx, &result ));
  	    return Owned<KCRef>::Seize( result );
		}


#if 0
typedef CALLBACK_API( OSStatus , KCCallbackProcPtr )(KCEvent keychainEvent, KCCallbackInfo *info, void *userContext);
typedef STACK_UPP_TYPE(KCCallbackProcPtr)                       KCCallbackUPP;
/*
 *  NewKCCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern KCCallbackUPP
NewKCCallbackUPP(KCCallbackProcPtr userRoutine)               AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  DisposeKCCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern void
DisposeKCCallbackUPP(KCCallbackUPP userUPP)                   AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

/*
 *  InvokeKCCallbackUPP()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   available as macro/inline
 */
extern OSStatus
InvokeKCCallbackUPP(
  KCEvent           keychainEvent,
  KCCallbackInfo *  info,
  void *            userContext,
  KCCallbackUPP     userUPP)                                  AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

#endif

#if 0
/* High-level interface for retrieving passwords */
/*
 *  KCFindAppleSharePassword()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCFindAppleSharePassword(
  AFPServerSignature *  serverSignature,       /* can be NULL */
  StringPtr             serverAddress,         /* can be NULL */
  StringPtr             serverName,            /* can be NULL */
  StringPtr             volumeName,            /* can be NULL */
  StringPtr             accountName,           /* can be NULL */
  UInt32                maxLength,
  void *                passwordData,
  UInt32 *              actualLength,
  KCItemRef *           item)                  /* can be NULL */ AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  KCFindInternetPassword()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCFindInternetPassword(
  StringPtr    serverName,           /* can be NULL */
  StringPtr    securityDomain,       /* can be NULL */
  StringPtr    accountName,          /* can be NULL */
  UInt16       port,
  OSType       protocol,
  OSType       authType,
  UInt32       maxLength,
  void *       passwordData,
  UInt32 *     actualLength,
  KCItemRef *  item)                 /* can be NULL */        AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  KCFindInternetPasswordWithPath()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 */
extern OSStatus 
KCFindInternetPasswordWithPath(
  StringPtr    serverName,           /* can be NULL */
  StringPtr    securityDomain,       /* can be NULL */
  StringPtr    accountName,          /* can be NULL */
  StringPtr    path,                 /* can be NULL */
  UInt16       port,
  OSType       protocol,
  OSType       authType,
  UInt32       maxLength,
  void *       passwordData,
  UInt32 *     actualLength,
  KCItemRef *  item)                 /* can be NULL */        AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  KCFindGenericPassword()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCFindGenericPassword(
  StringPtr    serviceName,        /* can be NULL */
  StringPtr    accountName,        /* can be NULL */
  UInt32       maxLength,
  void *       passwordData,
  UInt32 *     actualLength,
  KCItemRef *  item)               /* can be NULL */          AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif


#if 0
/* Keychain Manager callbacks */
/*
 *  KCAddCallback()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCAddCallback(
  KCCallbackUPP   callbackProc,
  KCEventMask     eventMask,
  void *          userContext)                                AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;


/*
 *  KCRemoveCallback()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCRemoveCallback(KCCallbackUPP callbackProc)                  AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif

	inline Owned<KCItemRef> KCNewItem ( KCItemClass itemClass, OSType itemCreator,
								UInt32 length, const void *data ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		KCItemRef	result;
		ThrowOSStatus ( ::KCNewItem ( itemClass, itemCreator, length, data, &result ));
  	    return Owned<KCItemRef>::Seize( result );
		}

	inline void KCSetAttribute ( KCItemRef item, SecKeychainAttrType tag, UInt32 length, const void *data ) {
		OnlyOnce<RegisterKeychainCoreErrors>();

		SecKeychainAttribute attr;
		attr.tag	= tag;
		attr.length = length;
		attr.data	= const_cast<void *> ( data );
		ThrowOSStatus ( ::KCSetAttribute ( item, &attr ));
		}
	

#if 0
<http://developer.apple.com/documentation/Carbon/Reference/Keychain_Manager/keychain_manager/function_group_7.html#//apple_ref/c/func/KCSetAttribute>
/*
 *  KCGetAttribute()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCGetAttribute(
  KCItemRef      item,
  KCAttribute *  attr,
  UInt32 *       actualLength)                                AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
#endif


	inline void KCSetData ( KCItemRef item, UInt32 length, const void *data ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		ThrowOSStatus ( ::KCSetData ( item, length, data ));
		}

	inline UInt32 KCGetData ( KCItemRef item, UInt32 maxLength, void *data ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		UInt32 result;
		ThrowOSStatus ( ::KCGetData ( item, maxLength, data, &result ));
		return result;
		}

	inline void KCUpdateItem ( KCItemRef item ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		ThrowOSStatus ( ::KCUpdateItem ( item ));
		}

//	extern OSStatus KCReleaseItem(KCItemRef * item);

	inline Owned<KCItemRef> KCCopyItem ( KCItemRef item, KCRef destKeychain ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		KCItemRef	result;
		ThrowOSStatus ( ::KCCopyItem ( item, destKeychain, &result ));
  	    return Owned<KCItemRef>::Seize( result );
		}


#if 0
/* Searching and enumerating keychain items */
/*
 *  KCFindFirstItem()
 *  
 *  Availability:
 *    Mac OS X:         in version 10.0 and later in CoreServices.framework
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 */
extern OSStatus 
KCFindFirstItem(
  KCRef                    keychain,       /* can be NULL */
  const KCAttributeList *  attrList,       /* can be NULL */
  KCSearchRef *            search,
  KCItemRef *              item)                              AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;

Marshall sez: this is icky because it returns two things that need to be owned.
#endif

	inline Owned<KCItemRef> KCFindNextItem ( KCSearchRef search ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		KCItemRef	result;
		ThrowOSStatus ( ::KCFindNextItem ( search, &result ));
  	    return Owned<KCItemRef>::Seize( result );
		}

//	extern OSStatus KCReleaseSearch(KCSearchRef * search);

//	!!! Should this be an Owned<KCItemRef> ???
	inline void KCDeleteItem ( KCItemRef item ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		ThrowOSStatus ( ::KCDeleteItem ( item ));
		}

	inline void KCLock ( KCRef keychain ) {
		OnlyOnce<RegisterKeychainCoreErrors>();
		ThrowOSStatus ( ::KCLock ( keychain ));
		}

//	KCUnlock is in KeychainHI.h

	}

#endif

