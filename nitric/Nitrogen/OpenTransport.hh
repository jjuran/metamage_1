// Nitrogen/OpenTransport.hh
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_OPENTRANSPORT_HH
#define NITROGEN_OPENTRANSPORT_HH

// Mac OS
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/errors_registered.hh"
#include "nucleus/owned.hh"

// Nitrogen
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
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
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( OpenTransport );
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	enum OTSequence
	{
		kOTSequence_Max = nucleus::enumeration_traits< ::OTSequence >::max
	};
	
	enum OTNameID
	{
		kOTNameID_Max = nucleus::enumeration_traits< ::OTNameID >::max
	};
	
	enum OTReason
	{
		kOTReason_Max = nucleus::enumeration_traits< ::OTReason >::max
	};
	
	enum OTCommand
	{
		kOTCommand_Max = nucleus::enumeration_traits< ::OTCommand >::max
	};
	
	enum OTOpenFlags
	{
		kOTOpenFlags_Max = nucleus::enumeration_traits< ::OTOpenFlags >::max
	};
	
	enum OTUnixErr
	{
		kOTUnixErr_Max = nucleus::enumeration_traits< ::OTUnixErr >::max
	};
	
	enum OTXTIErr
	{
		kOTXTIErr_Max = nucleus::enumeration_traits< ::OTXTIErr >::max
	};
	
	// ...
	
	using ::OTConfigurationRef;
	using ::OTClientContextPtr;
	using ::EndpointRef;		
    using ::ProviderRef;
    
}

namespace nucleus
{
	
	template <>
	struct disposer< Nitrogen::OTConfigurationRef > : public std::unary_function< Nitrogen::OTConfigurationRef, void >
	{
		void operator()( Nitrogen::OTConfigurationRef config ) const
		{
			::OTDestroyConfiguration( config );
		}
	};
	
	template <>
	struct disposer< Nitrogen::OTClientContextPtr > : public std::unary_function< Nitrogen::OTClientContextPtr, void >
	{
		void operator()( Nitrogen::OTClientContextPtr context ) const
		{
			::CloseOpenTransportInContext( context );
		}
	};
	
	template <>
	struct disposer< Nitrogen::ProviderRef > : public std::unary_function< Nitrogen::ProviderRef, void >
	{
		void operator()( Nitrogen::ProviderRef provider ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::OpenTransport );
			
			::Nitrogen::HandleDestructionOSStatus( ::OTCloseProvider( provider ) );
		}
	};

	template <> struct disposer_traits< Nitrogen::EndpointRef > : disposer_traits< Nitrogen::ProviderRef > {};
	
}

namespace Nitrogen
{	
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	inline OTResult ThrowOTResult( OTResult result )
	{
		if ( result < 0 )
		{
			ThrowOSStatus( result );
		}
		
		return result;
	}
	
	nucleus::owned< OTClientContextPtr > InitOpenTransportInContext( OTInitializationFlags flags );
	
	inline void CloseOpenTransportInContext( nucleus::owned< OTClientContextPtr > )  {}
	
	void OTCloseProvider( nucleus::owned< ProviderRef > provider );
	
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
	
	nucleus::owned< EndpointRef > OTOpenEndpointInContext( nucleus::owned< OTConfigurationRef >  config,
	                                                       TEndpointInfo*                        info          = NULL,
	                                                       OTClientContextPtr                    clientContext = NULL );
	
	inline nucleus::owned< EndpointRef > OTOpenEndpointInContext( nucleus::owned< OTConfigurationRef >  config,
	                                                              OTClientContextPtr                    clientContext )
	{
		return OTOpenEndpointInContext( config, NULL, clientContext );
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
	
	nucleus::owned< OTConfigurationRef > OTCreateConfiguration( const char* path );
	
	nucleus::owned< OTConfigurationRef > OTCloneConfiguration( OTConfigurationRef config );
	
	inline void OTDestroyConfiguration( nucleus::owned< OTConfigurationRef > )  {}
	
}

#endif

