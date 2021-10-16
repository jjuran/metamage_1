// Nitrogen/OpenTransport.cc
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#if __LP64__
int dummy;
#else

#include "Nitrogen/OpenTransport.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
#ifndef MAC_OS_X_VERSION_10_8
	
	using Mac::ThrowOSStatus;
	
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( OpenTransport )
	
	
	static void RegisterOpenTransportErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class OpenTransportErrorsRegistration
	{
		public:
			OpenTransportErrorsRegistration()  { RegisterOpenTransportErrors(); }
	};
	
	static OpenTransportErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	nucleus::owned< OTClientContextPtr > InitOpenTransportInContext( OTInitializationFlags flags )
	{
		OTClientContextPtr result;
		
		ThrowOSStatus( ::InitOpenTransportInContext( flags, &result ) );
		
		return nucleus::owned< OTClientContextPtr >::seize( result );
	}
	
	void OTCloseProvider( nucleus::owned< ProviderRef > provider )
	{
		ThrowOSStatus( ::OTCloseProvider( provider.release() ) );
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
	
	nucleus::owned< EndpointRef > OTOpenEndpointInContext( nucleus::owned< OTConfigurationRef >  config,
	                                                       TEndpointInfo*                        info,
	                                                       OTClientContextPtr                    clientContext )
	{
		::OSStatus err;
		
		EndpointRef result = ::OTOpenEndpointInContext( config.release(),
		                                                OTOpenFlags( 0 ),
		                                                info,
		                                                &err,
		                                                clientContext );
		
		ThrowOSStatus( err );
		
		return nucleus::owned< EndpointRef >::seize( result );
	}
	
	void
	//
	OTAsyncOpenEndpointInContext( nucleus::owned< OTConfigurationRef >  config,
	                              TEndpointInfo*                        info,
	                              OTNotifyUPP                           notifier,
	                              void*                                 context,
	                              OTClientContextPtr                    clientContext )
	{
		OSStatus err = ::OTAsyncOpenEndpointInContext( config.release(),
		                                               OTOpenFlags( 0 ),
		                                               info,
		                                               notifier,
		                                               context,
		                                               clientContext );
		
		ThrowOSStatus( err );
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
	
	void OTRcvConnect( EndpointRef  ref,
	                   TCall*       call )
	{
		ThrowOSStatus( ::OTRcvConnect( ref, call ) );
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
	
	nucleus::owned< OTConfigurationRef > OTCreateConfiguration( const char* path )
	{
		OTConfigurationRef result = ::OTCreateConfiguration( path );
		
		if ( result == NULL )
		{
			throw OTCreateConfiguration_Failed();
		}
		
		return nucleus::owned< OTConfigurationRef >::seize( result );
	}
	
	nucleus::owned< OTConfigurationRef > OTCloneConfiguration( OTConfigurationRef config )
	{
		return nucleus::owned< OTConfigurationRef >::seize( ::OTCloneConfiguration( config ) );
	}
	
#endif  // #ifndef MAC_OS_X_VERSION_10_8
	
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

#endif  // #if __LP64__
