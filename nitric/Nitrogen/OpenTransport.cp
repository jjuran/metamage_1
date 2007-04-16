// ================
// OpenTransport.cp
// ================

#ifndef NITROGEN_OPENTRANSPORT_H
#include "Nitrogen/OpenTransport.h"
#endif


namespace Nitrogen
{
	
	OpenTransportErrorsRegistrationDependency::OpenTransportErrorsRegistrationDependency()
	{
		// does nothing, but guarantees construction of theRegistration
	}
	
	
	static void RegisterOpenTransportErrors();
	
	
	class OpenTransportErrorsRegistration
	{
		public:
			OpenTransportErrorsRegistration()  { RegisterOpenTransportErrors(); }
	};
	
	static OpenTransportErrorsRegistration theRegistration;
	
	
	Nucleus::Owned< OTClientContextPtr > InitOpenTransportInContext( OTInitializationFlags flags )
	{
		OTClientContextPtr result;
		
		ThrowOSStatus( ::InitOpenTransportInContext( flags, &result ) );
		
		return Nucleus::Owned< OTClientContextPtr >::Seize( result );
	}
	
	void InitOpenTransport()
	{
	#if TARGET_API_MAC_CARBON
		
		ThrowOSStatus( ::InitOpenTransportInContext( kInitOTForApplicationMask, NULL ) );
		
	#else
		
		ThrowOSStatus( ::InitOpenTransport() );
		
	#endif
	}
	
	void CloseOpenTransport()
	{
	#if TARGET_API_MAC_CARBON
		
		::CloseOpenTransportInContext( NULL );
		
	#else
		
		::CloseOpenTransport();
		
	#endif
	}
	
	void OTCloseProvider( Nucleus::Owned< ProviderRef > provider )
	{
		ThrowOSStatus( ::OTCloseProvider( provider.Release() ) );
	}
	
	void OTSetSynchronous( ProviderRef ref )
	{
		ThrowOSStatus( ::OTSetSynchronous( ref ) );
	}
	
	void OTSetAsynchronous( ProviderRef ref )
	{
		ThrowOSStatus( ::OTSetAsynchronous( ref ) );
	}
	
	void OTSetBlocking( ProviderRef ref )
	{
		ThrowOSStatus( ::OTSetBlocking( ref ) );
	}
	
	void OTSetNonBlocking( ProviderRef ref )
	{
		ThrowOSStatus( ::OTSetNonBlocking( ref ) );
	}
	
	void OTInstallNotifier( ProviderRef  ref,
	                        OTNotifyUPP  proc,
	                        void*        contextPtr )
	{
		ThrowOSStatus( ::OTInstallNotifier( ref, proc, contextPtr ) );
	}
	
	void OTUseSyncIdleEvents( ProviderRef  ref,
	                          bool         useEvents )
	{
		ThrowOSStatus( ::OTUseSyncIdleEvents( ref, useEvents ) );
	}
	
	Nucleus::Owned< EndpointRef > OTOpenEndpointInContext( Nucleus::Owned< OTConfigurationRef >  config,
	                                                       TEndpointInfo*                        info,
	                                                       OTClientContextPtr                    clientContext )
	{
		::OSStatus err;
		
	#if TARGET_API_MAC_CARBON
		
		EndpointRef result = ::OTOpenEndpointInContext( config.Release(),
		                                                OTOpenFlags( 0 ),
		                                                info,
		                                                &err,
		                                                clientContext );
		
	#else
		
		EndpointRef result = ::OTOpenEndpoint( config.Release(),
		                                       OTOpenFlags( 0 ),
		                                       info,
		                                       &err );
		
		(void) clientContext;
		
	#endif
		
		ThrowOSStatus( err );
		
		return Nucleus::Owned< EndpointRef >::Seize( result );
	}
	
	static OTResult ThrowOTResult( OTResult result )
	{
		if ( result < 0 )
		{
			ThrowOSStatus( result );
		}
		
		return result;
	}
	
	OTResult OTLook( EndpointRef ref )
	{
		return ThrowOTResult( ::OTLook( ref ) );
	}
	
	void OTBind( EndpointRef  ref,
	             TBind*       reqAddr,
	             TBind*       retAddr )
	{
		ThrowOSStatus( ::OTBind( ref, reqAddr, retAddr ) );
	}
	
	void OTUnbind( EndpointRef ref )
	{
		ThrowOSStatus( ::OTUnbind( ref ) );
	}
	
	void OTConnect( EndpointRef  ref,
	                TCall&       sndCall,
	                TCall*       rcvCall )
	{
		ThrowOSStatus( ::OTConnect( ref, &sndCall, rcvCall ) );
	}
	
	void OTListen( EndpointRef  ref,
	               TCall*       call )
	{
		ThrowOSStatus( ::OTListen( ref, call ) );
	}
	
	void OTAccept( EndpointRef  listener,
	               EndpointRef  worker,
	               TCall*       call )
	{
		ThrowOSStatus( ::OTAccept( listener, worker, call ) );
	}
	
	void OTSndDisconnect( EndpointRef  ref,
	                      TCall*       call )
	{
		ThrowOSStatus( ::OTSndDisconnect( ref, call ) );
	}
	
	void OTSndOrderlyDisconnect( EndpointRef ref )
	{
		ThrowOSStatus( ::OTSndOrderlyDisconnect( ref ) );
	}
	
	void OTRcvDisconnect( EndpointRef  ref,
	                      TDiscon*     discon )
	{
		ThrowOSStatus( ::OTRcvDisconnect( ref, discon ) );
	}
	
	void OTRcvOrderlyDisconnect( EndpointRef ref )
	{
		ThrowOSStatus( ::OTRcvOrderlyDisconnect( ref ) );
	}
	
	OTResult OTRcv( EndpointRef  ref,
	                void*        buf,
	                OTByteCount  nbytes,
	                OTFlags*     flags )
	{
		return ThrowOTResult( ::OTRcv( ref, buf, nbytes, flags ) );
	}
	
	OTResult OTRcv( EndpointRef  ref,
	                void*        buf,
	                OTByteCount  nbytes )
	{
		OTFlags flags;
		
		return Nitrogen::OTRcv( ref, buf, nbytes, &flags );
	}
	
	OTResult OTSnd( EndpointRef  ref,
	                const void*  buf,
	                OTByteCount  nbytes,
	                OTFlags      flags )
	{
		return ThrowOTResult( ::OTSnd( ref, (void*)buf, nbytes, flags ) );
	}
	
	class OTCreateConfiguration_Failed {};
	
	Nucleus::Owned< OTConfigurationRef > OTCreateConfiguration( const char* path )
	{
		OTConfigurationRef result = ::OTCreateConfiguration( path );
		
		if ( result == NULL )
		{
			throw OTCreateConfiguration_Failed();
		}
		
		return Nucleus::Owned< OTConfigurationRef >::Seize( result );
	}
	
	Nucleus::Owned< OTConfigurationRef > OTCloneConfiguration( OTConfigurationRef config )
	{
		return Nucleus::Owned< OTConfigurationRef >::Seize( ::OTCloneConfiguration( config ) );
	}
	
	void RegisterOpenTransportErrors()
	{
	//	RegisterOSStatus< kOTOutOfMemoryErr          >();  // Duplicates kENOMEMErr
	//	RegisterOSStatus< kOTNotFoundErr             >();  // Duplicates kENOENTErr
	//	RegisterOSStatus< kOTDuplicateFoundErr       >();  // Duplicates kEEXISTErr
		RegisterOSStatus< kOTBadAddressErr           >();
		RegisterOSStatus< kOTBadOptionErr            >();
		RegisterOSStatus< kOTAccessErr               >();
		RegisterOSStatus< kOTBadReferenceErr         >();
		RegisterOSStatus< kOTNoAddressErr            >();
		RegisterOSStatus< kOTOutStateErr             >();
		RegisterOSStatus< kOTBadSequenceErr          >();
		RegisterOSStatus< kOTSysErrorErr             >();
		RegisterOSStatus< kOTLookErr                 >();
		RegisterOSStatus< kOTBadDataErr              >();
		RegisterOSStatus< kOTBufferOverflowErr       >();
		RegisterOSStatus< kOTFlowErr                 >();
		RegisterOSStatus< kOTNoDataErr               >();
		RegisterOSStatus< kOTNoDisconnectErr         >();
		RegisterOSStatus< kOTNoUDErrErr              >();
		RegisterOSStatus< kOTBadFlagErr              >();
		RegisterOSStatus< kOTNoReleaseErr            >();
		RegisterOSStatus< kOTNotSupportedErr         >();
		RegisterOSStatus< kOTStateChangeErr          >();
		RegisterOSStatus< kOTNoStructureTypeErr      >();
		RegisterOSStatus< kOTBadNameErr              >();
		RegisterOSStatus< kOTBadQLenErr              >();
		RegisterOSStatus< kOTAddressBusyErr          >();
		RegisterOSStatus< kOTIndOutErr               >();
		RegisterOSStatus< kOTProviderMismatchErr     >();
		RegisterOSStatus< kOTResQLenErr              >();
		RegisterOSStatus< kOTResAddressErr           >();
		RegisterOSStatus< kOTQFullErr                >();
		RegisterOSStatus< kOTProtocolErr             >();
		RegisterOSStatus< kOTBadSyncErr              >();
		RegisterOSStatus< kOTCanceledErr             >();
		RegisterOSStatus< kEPERMErr                  >();
		RegisterOSStatus< kENOENTErr                 >();
		RegisterOSStatus< kENORSRCErr                >();
		RegisterOSStatus< kEINTRErr                  >();
		RegisterOSStatus< kEIOErr                    >();
		RegisterOSStatus< kENXIOErr                  >();
		RegisterOSStatus< kEBADFErr                  >();
		RegisterOSStatus< kEAGAINErr                 >();
		RegisterOSStatus< kENOMEMErr                 >();
		RegisterOSStatus< kEACCESErr                 >();
		RegisterOSStatus< kEFAULTErr                 >();
		RegisterOSStatus< kEBUSYErr                  >();
		RegisterOSStatus< kEEXISTErr                 >();
		RegisterOSStatus< kENODEVErr                 >();
		RegisterOSStatus< kEINVALErr                 >();
		RegisterOSStatus< kENOTTYErr                 >();
		RegisterOSStatus< kEPIPEErr                  >();
		RegisterOSStatus< kERANGEErr                 >();
		RegisterOSStatus< kEWOULDBLOCKErr            >();
		RegisterOSStatus< kEDEADLKErr                >();
		RegisterOSStatus< kEALREADYErr               >();
		RegisterOSStatus< kENOTSOCKErr               >();
		RegisterOSStatus< kEDESTADDRREQErr           >();
		RegisterOSStatus< kEMSGSIZEErr               >();
		RegisterOSStatus< kEPROTOTYPEErr             >();
		RegisterOSStatus< kENOPROTOOPTErr            >();
		RegisterOSStatus< kEPROTONOSUPPORTErr        >();
		RegisterOSStatus< kESOCKTNOSUPPORTErr        >();
		RegisterOSStatus< kEOPNOTSUPPErr             >();
		RegisterOSStatus< kEADDRINUSEErr             >();
		RegisterOSStatus< kEADDRNOTAVAILErr          >();
		RegisterOSStatus< kENETDOWNErr               >();
		RegisterOSStatus< kENETUNREACHErr            >();
		RegisterOSStatus< kENETRESETErr              >();
		RegisterOSStatus< kECONNABORTEDErr           >();
		RegisterOSStatus< kECONNRESETErr             >();
		RegisterOSStatus< kENOBUFSErr                >();
		RegisterOSStatus< kEISCONNErr                >();
		RegisterOSStatus< kENOTCONNErr               >();
		RegisterOSStatus< kESHUTDOWNErr              >();
		RegisterOSStatus< kETOOMANYREFSErr           >();
		RegisterOSStatus< kETIMEDOUTErr              >();
		RegisterOSStatus< kECONNREFUSEDErr           >();
		RegisterOSStatus< kEHOSTDOWNErr              >();
		RegisterOSStatus< kEHOSTUNREACHErr           >();
		RegisterOSStatus< kEPROTOErr                 >();
		RegisterOSStatus< kETIMEErr                  >();
		RegisterOSStatus< kENOSRErr                  >();
		RegisterOSStatus< kEBADMSGErr                >();
		RegisterOSStatus< kECANCELErr                >();
		RegisterOSStatus< kENOSTRErr                 >();
		RegisterOSStatus< kENODATAErr                >();
		RegisterOSStatus< kEINPROGRESSErr            >();
		RegisterOSStatus< kESRCHErr                  >();
		RegisterOSStatus< kENOMSGErr                 >();
		RegisterOSStatus< kOTClientNotInittedErr     >();
		RegisterOSStatus< kOTPortHasDiedErr          >();
		RegisterOSStatus< kOTPortWasEjectedErr       >();
		RegisterOSStatus< kOTBadConfigurationErr     >();
		RegisterOSStatus< kOTConfigurationChangedErr >();
		RegisterOSStatus< kOTUserRequestedErr        >();
		RegisterOSStatus< kOTPortLostConnection      >();
	}
	
}

