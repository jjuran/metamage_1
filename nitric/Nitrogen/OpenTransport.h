// ===============
// OpenTransport.h
// ===============

#ifndef NITROGEN_OPENTRANSPORT_H
#define NITROGEN_OPENTRANSPORT_H

// Universal Interfaces
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif

// Nitrogen core
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif

// Nitrogen Carbon support
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif


#ifdef XTI2OSStatus
#undef XTI2OSStatus

inline OSStatus XTI2OSStatus( OTXTIErr x )
{
	return -3149 - x;
}

#endif

#ifdef E2OSStatus
#undef E2OSStatus

inline OSStatus E2OSStatus( OTUnixErr e )
{
	return -3199 - e;
}

#endif

#ifdef OSStatus2XTI
#undef OSStatus2XTI

inline OTXTIErr OSStatus2XTI( OSStatus status )
{
	return -3149 - status;
}

#endif

#ifdef OSStatus2E
#undef OSStatus2E

inline OTUnixErr OSStatus2E( OSStatus status )
{
	return -3199 - status;
}

#endif

#ifdef IsXTIError
#undef IsXTIError

#if OTUNIXERRORS
inline bool IsXTIError( OSStatus status )
{
	OTXTIErr x = OSStatus2XTI( status );
	
	return x > 0  &&  x <= TLASTXTIERROR;
}
#endif

#endif

#ifdef IsEError
#undef IsEError

#if OTUNIXERRORS
inline bool IsEError( OSStatus status )
{
	OTUnixErr x = OSStatus2E( status );
	
	return x > 0  &&  x <= ELASTERRNO;
}
#endif

#endif

#ifdef InitOpenTransport
#undef InitOpenTransport
#endif

#ifdef CloseOpenTransport
#undef CloseOpenTransport
#endif

#ifdef OTIsNonBlocking
#undef OTIsNonBlocking

inline bool OTIsNonBlocking( ProviderRef ref )  { return !OTIsBlocking( ref ); }

#endif

#ifdef OTIsAsynchronous
#undef OTIsAsynchronous

inline bool OTIsAsynchronous( ProviderRef ref )  { return !OTIsSynchronous( ref ); }

#endif

#ifdef OTOpenEndpoint
#undef OTOpenEndpoint
#endif


namespace Nitrogen
{
	
	void RegisterOpenTransportErrors();
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	class OTSequence_Tag {};
	typedef IDType< OTSequence_Tag, ::OTSequence > OTSequence;
	
	class OTNameID_Tag {};
	typedef IDType< OTNameID_Tag, ::OTNameID > OTNameID;
	
	class OTReason_Tag {};
	typedef SelectorType< OTReason_Tag, ::OTReason, 0 > OTReason;
	
	class OTCommand_Tag {};
	typedef SelectorType< OTCommand_Tag, ::OTCommand, 0 > OTCommand;
	
	class OTOpenFlags_Tag {};
	typedef FlagType< OTOpenFlags_Tag, ::OTOpenFlags > OTOpenFlags;
	
	class OTUnixErr_Tag {};
	typedef SelectorType< OTUnixErr_Tag, ::OTUnixErr, 0 > OTUnixErr;
	
	class OTXTIErr_Tag {};
	typedef SelectorType< OTXTIErr_Tag, ::OTXTIErr, 0 > OTXTIErr;
	
	using ::ProviderRef;
	using ::EndpointRef;
	using ::MapperRef;
	
	template <>
	struct Disposer< ProviderRef > : std::unary_function< ProviderRef, void >,
	                                 private DefaultDestructionOSStatusPolicy
	{
		void operator()( ProviderRef provider ) const
		{
			HandleDestructionOSStatus( ::OTCloseProvider( provider ) );
		}
	};
	
	template <> struct OwnedDefaults< EndpointRef > : OwnedDefaults< ProviderRef > {};
	template <> struct OwnedDefaults< MapperRef   > : OwnedDefaults< ProviderRef > {};
	
	class OTEventCode_Tag {};
	typedef SelectorType< OTEventCode_Tag, ::OTEventCode > OTEventCode;
	
	class OTXTIStates_Tag {};
	typedef SelectorType< OTXTIStates_Tag, ::OTXTIStates, T_UNINIT > OTXTIStates;
	typedef OTXTIStates OTXTIState;
	
	// ...
	
	using ::OTConfigurationRef;
	
	template <>
	struct Disposer< OTConfigurationRef > : std::unary_function< OTConfigurationRef, void >
	{
		void operator()( OTConfigurationRef config ) const
		{
			::OTDestroyConfiguration( config );
		}
	};
	
	using ::OTClientContextPtr;
	
	template <>
	struct Disposer< OTClientContextPtr > : std::unary_function< OTClientContextPtr, void >
	{
		void operator()( OTClientContextPtr context ) const
		{
			::CloseOpenTransportInContext( context );
		}
	};
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	Owned< OTClientContextPtr > InitOpenTransportInContext( OTInitializationFlags flags );
	
	inline void CloseOpenTransportInContext( Owned< OTClientContextPtr > )  {}
	
	void InitOpenTransport();
	void CloseOpenTransport();
	
	void OTCloseProvider( Owned< ProviderRef > provider );
	
	// ...
	
	void OTSetSynchronous ( ProviderRef ref );
	void OTSetAsynchronous( ProviderRef ref );
	
	inline bool OTIsSynchronous( ProviderRef ref )  { return ::OTIsSynchronous( ref ); }
	
	void OTSetBlocking   ( ProviderRef ref );
	void OTSetNonBlocking( ProviderRef ref );
	
	inline bool OTIsBlocking( ProviderRef ref )  { return ::OTIsBlocking( ref ); }
	
	void OTInstallNotifier( ProviderRef  ref,
	                        OTNotifyUPP  proc,
	                        void*        contextPtr );
	
	void OTUseSyncIdleEvents( ProviderRef  ref,
	                          bool         useEvents );
	
	using ::OTRemoveNotifier;
	using ::OTLeaveNotifier;
	
	inline bool OTEnterNotifier( ProviderRef ref )  { return ::OTEnterNotifier( ref ); }
	
	void OTAckSends    ( ProviderRef ref );
	void OTDontAckSends( ProviderRef ref );
	
	inline bool OTIsAckingSends( ProviderRef ref )  { return ::OTIsAckingSends( ref ); }
	
	void OTCancelSynchronousCalls( ProviderRef  ref,
	                               OSStatus     err );
	
	using ::OTIsNonBlocking;
	using ::OTIsAsynchronous;
	
	Owned< EndpointRef > OTOpenEndpointInContext( Owned< OTConfigurationRef >  config,
	                                              TEndpointInfo*               info          = NULL,
	                                              OTClientContextPtr           clientContext = NULL );
	
	inline Owned< EndpointRef > OTOpenEndpointInContext( Owned< OTConfigurationRef >  config,
	                                                     OTClientContextPtr           clientContext )
	{
		return OTOpenEndpointInContext( config, NULL, clientContext );
	}
	
	inline Owned< EndpointRef > OTOpenEndpoint( Owned< OTConfigurationRef >  config,
	                                            TEndpointInfo*               info = NULL )
	{
		return OTOpenEndpointInContext( config, info );
	}
	
	TEndpointInfo OTGetEndpointInfo( EndpointRef ref );
	
	OTXTIState OTGetEndpointState( EndpointRef ref );
	
	OTEventCode OTLook( EndpointRef ref );
	
	// ...
	
	void OTBind( EndpointRef  ref,
	             TBind*       reqAddr = NULL,
	             TBind*       retAddr = NULL );
	
	void OTUnbind( EndpointRef ref );
	
	void OTConnect( EndpointRef  ref,
	                TCall&       sndCall,
	                TCall*       rcvCall = NULL );
	
	void OTListen( EndpointRef  ref,
	               TCall*       call );
	
	void OTAccept( EndpointRef  listener,
	               EndpointRef  worker,
	               TCall*       call );
	
	void OTSndDisconnect( EndpointRef  ref,
	                      TCall*       call = NULL );
	
	void OTSndOrderlyDisconnect( EndpointRef ref );
	
	void OTRcvDisconnect( EndpointRef  ref,
	                      TDiscon*     discon = NULL );
	
	void OTRcvOrderlyDisconnect( EndpointRef ref );
	
	OTByteCount OTRcv( EndpointRef  ref,
	                   void*        buf,
	                   OTByteCount  nbytes,
	                   OTFlags*     flags );
	
	OTByteCount OTRcv( EndpointRef  ref,
	                   void*        buf,
	                   OTByteCount  nbytes );
	
	OTByteCount OTSnd( EndpointRef  ref,
	                   const void*  buf,
	                   OTByteCount  nbytes,
	                   OTFlags      flags = OTFlags( 0 ) );
	
	Owned< OTConfigurationRef > OTCreateConfiguration( const char* path );
	
	Owned< OTConfigurationRef > OTCloneConfiguration( OTConfigurationRef config );
	
	inline void OTDestroyConfiguration( Owned< OTConfigurationRef > )  {}
	
}

#endif

