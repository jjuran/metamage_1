// ===============
// OpenTransport.h
// ===============

#ifndef NITROGEN_OPENTRANSPORT_H
#define NITROGEN_OPENTRANSPORT_H

// Mac OS Universal Interfaces
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif

// Nucleus
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

// Nitrogen
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
	
	class OpenTransportErrorsRegistrationDependency
	{
		public:
			OpenTransportErrorsRegistrationDependency();
	};
 	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	typedef Nucleus::ID< class OTSequence_Tag, ::OTSequence >::Type OTSequence;
	
	typedef Nucleus::ID< class OTNameID_Tag, ::OTNameID >::Type OTNameID;
	
	typedef Nucleus::Selector< class OTReason_Tag, ::OTReason >::Type OTReason;
	
	typedef Nucleus::Selector< class OTCommand_Tag, ::OTCommand >::Type OTCommand;
	
	typedef Nucleus::Flag< class OTOpenFlags_Tag, ::OTOpenFlags >::Type OTOpenFlags;
	
	typedef Nucleus::Selector< class OTUnixErr_Tag, ::OTUnixErr >::Type OTUnixErr;
	
	typedef Nucleus::Selector< class OTXTIErr_Tag, ::OTXTIErr >::Type OTXTIErr;
	
	// ...
	
	using ::OTConfigurationRef;
	using ::OTClientContextPtr;
	using ::EndpointRef;		
    using ::ProviderRef;
    
}

namespace Nucleus
{
	
	template <>
	struct Disposer< Nitrogen::OTConfigurationRef > : public std::unary_function< Nitrogen::OTConfigurationRef, void >
	{
		void operator()( Nitrogen::OTConfigurationRef config ) const
		{
			::OTDestroyConfiguration( config );
		}
	};
	
	template <>
	struct Disposer< Nitrogen::OTClientContextPtr > : public std::unary_function< Nitrogen::OTClientContextPtr, void >
	{
		void operator()( Nitrogen::OTClientContextPtr context ) const
		{
			::CloseOpenTransportInContext( context );
		}
	};
	
	template <>
	struct Disposer< Nitrogen::ProviderRef > : public std::unary_function< Nitrogen::ProviderRef, void >,
											   private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::ProviderRef provider ) const
		{
			(void) Nitrogen::OpenTransportErrorsRegistrationDependency();
			HandleDestructionOSStatus( ::OTCloseProvider( provider ) );
		}
	};

	template <> struct OwnedDefaults< Nitrogen::EndpointRef > : OwnedDefaults< Nitrogen::ProviderRef > {};
	
}

namespace Nitrogen
{	
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	Nucleus::Owned< OTClientContextPtr > InitOpenTransportInContext( OTInitializationFlags flags );
	
	inline void CloseOpenTransportInContext( Nucleus::Owned< OTClientContextPtr > )  {}
	
	void InitOpenTransport();
	void CloseOpenTransport();
	
	void OTCloseProvider( Nucleus::Owned< ProviderRef > provider );
	
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
	
	Nucleus::Owned< EndpointRef > OTOpenEndpointInContext( Nucleus::Owned< OTConfigurationRef >  config,
	                                                       TEndpointInfo*                        info          = NULL,
	                                                       OTClientContextPtr                    clientContext = NULL );
	
	inline Nucleus::Owned< EndpointRef > OTOpenEndpointInContext( Nucleus::Owned< OTConfigurationRef >  config,
	                                                              OTClientContextPtr                    clientContext )
	{
		return OTOpenEndpointInContext( config, NULL, clientContext );
	}
	
	inline Nucleus::Owned< EndpointRef > OTOpenEndpoint( Nucleus::Owned< OTConfigurationRef >  config,
	                                                     TEndpointInfo*                        info = NULL )
	{
		return OTOpenEndpointInContext( config, info );
	}
	
	TEndpointInfo OTGetEndpointInfo( EndpointRef ref );
	
	OTResult OTGetEndpointState( EndpointRef ref );
	
	OTResult OTLook( EndpointRef ref );
	
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
	
	OTResult OTRcv( EndpointRef  ref,
	                void*        buf,
	                OTByteCount  nbytes,
	                OTFlags*     flags );
	
	OTResult OTRcv( EndpointRef  ref,
	                void*        buf,
	                OTByteCount  nbytes );
	
	OTResult OTSnd( EndpointRef  ref,
	                const void*  buf,
	                OTByteCount  nbytes,
	                OTFlags      flags = OTFlags( 0 ) );
	
	Nucleus::Owned< OTConfigurationRef > OTCreateConfiguration( const char* path );
	
	Nucleus::Owned< OTConfigurationRef > OTCloneConfiguration( OTConfigurationRef config );
	
	inline void OTDestroyConfiguration( Nucleus::Owned< OTConfigurationRef > )  {}
	
}

#endif

