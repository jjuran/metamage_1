// Nitrogen/Movies.cc
// ------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Movies.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( QuickTime )
	
	
	static void RegisterQuickTimeErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class QuickTimeErrorsRegistration
	{
		public:
			QuickTimeErrorsRegistration()  { RegisterQuickTimeErrors(); }
	};
	
	static QuickTimeErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	nucleus::owned< MovieFileRefNum > OpenMovieFile( const FSSpec&  file,
	                                                 FSIOPermssn    permission )
	{
		SInt16 result;
		ThrowOSStatus( ::OpenMovieFile( &file, &result, permission ) );
		
		return nucleus::owned< MovieFileRefNum >::seize( MovieFileRefNum( result ) );
	}
	
	nucleus::owned< Movie > NewMovieFromFile( MovieFileRefNum  refNum,
	                                          ResID            resID,
	                                          NewMovieFlags    flags )
	{
		Movie result;
		::ResID resIDcopy = resID;
		
		ThrowOSStatus( ::NewMovieFromFile( &result, refNum, &resIDcopy, NULL, flags, NULL ) );
		
		return nucleus::owned< Movie >::seize( result );
	}
	
	// Half-way Nitrified call with eraseColor passed by pointer
	
	static nucleus::owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                               short            desiredWidth,
	                                                               short            desiredHeight,
	                                                               ::WindowRef*     newWindow,
	                                                               const RGBColor*  eraseColor,
	                                                               FullScreenFlags  flags )
	{
		FullScreenContextPtr context;
		
		ThrowOSStatus( ::BeginFullScreen( reinterpret_cast< ::Ptr* >( &context ),
		                                  whichGD,
		                                  &desiredWidth,
		                                  &desiredHeight,
		                                  newWindow,
		                                  const_cast< RGBColor* >( eraseColor ),
		                                  flags ) );
		
		return nucleus::owned< FullScreenContextPtr >::seize( context );
	}
	
	// Basic Nitrogen version
	
	nucleus::owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        ::WindowRef*     newWindow,
	                                                        const RGBColor&  eraseColor,
	                                                        FullScreenFlags  flags )
	{
		return BeginFullScreen( whichGD,
		                        desiredWidth,
		                        desiredHeight,
		                        newWindow,
		                        &eraseColor,
		                        flags );
	}
	
	// Nitrogen version with eraseColor omitted
	
	nucleus::owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        ::WindowRef*     newWindow,
	                                                        FullScreenFlags  flags )
	{
		return BeginFullScreen( whichGD,
		                        desiredWidth,
		                        desiredHeight,
		                        newWindow,
		                        NULL,
		                        flags );
	}
	
	// Nitrogen version with newWindow omitted
	
	nucleus::owned< FullScreenContextPtr > BeginFullScreen( GDHandle         whichGD,
	                                                        short            desiredWidth,
	                                                        short            desiredHeight,
	                                                        const RGBColor&  eraseColor,
	                                                        FullScreenFlags  flags )
	{
		return BeginFullScreen( whichGD,
		                        desiredWidth,
		                        desiredHeight,
		                        NULL,
		                        &eraseColor,
		                        flags );
	}
	
	void RegisterQuickTimeErrors()
	{
		RegisterOSStatus< couldNotResolveDataRef           >();
		RegisterOSStatus< badImageDescription              >();
		RegisterOSStatus< badPublicMovieAtom               >();
		RegisterOSStatus< cantFindHandler                  >();
		RegisterOSStatus< cantOpenHandler                  >();
		RegisterOSStatus< badComponentType                 >();
		RegisterOSStatus< noMediaHandler                   >();
		RegisterOSStatus< noDataHandler                    >();
		RegisterOSStatus< invalidMedia                     >();
		RegisterOSStatus< invalidTrack                     >();
		RegisterOSStatus< invalidMovie                     >();
		RegisterOSStatus< invalidSampleTable               >();
		RegisterOSStatus< invalidDataRef                   >();
		RegisterOSStatus< invalidHandler                   >();
		RegisterOSStatus< invalidDuration                  >();
		RegisterOSStatus< invalidTime                      >();
		RegisterOSStatus< cantPutPublicMovieAtom           >();
		RegisterOSStatus< badEditList                      >();
		RegisterOSStatus< mediaTypesDontMatch              >();
		RegisterOSStatus< progressProcAborted              >();
		RegisterOSStatus< movieToolboxUninitialized        >();
		RegisterOSStatus< wfFileNotFound                   >();
		RegisterOSStatus< cantCreateSingleForkFile         >();
		RegisterOSStatus< invalidEditState                 >();
		RegisterOSStatus< nonMatchingEditState             >();
		RegisterOSStatus< staleEditState                   >();
		RegisterOSStatus< userDataItemNotFound             >();
		RegisterOSStatus< maxSizeToGrowTooSmall            >();
		RegisterOSStatus< badTrackIndex                    >();
		RegisterOSStatus< trackIDNotFound                  >();
		RegisterOSStatus< trackNotInMovie                  >();
		RegisterOSStatus< timeNotInTrack                   >();
		RegisterOSStatus< timeNotInMedia                   >();
		RegisterOSStatus< badEditIndex                     >();
		RegisterOSStatus< internalQuickTimeError           >();
		RegisterOSStatus< cantEnableTrack                  >();
		RegisterOSStatus< invalidRect                      >();
		RegisterOSStatus< invalidSampleNum                 >();
		RegisterOSStatus< invalidChunkNum                  >();
		RegisterOSStatus< invalidSampleDescIndex           >();
		RegisterOSStatus< invalidChunkCache                >();
		RegisterOSStatus< invalidSampleDescription         >();
		RegisterOSStatus< dataNotOpenForRead               >();
		RegisterOSStatus< dataNotOpenForWrite              >();
		RegisterOSStatus< dataAlreadyOpenForWrite          >();
		RegisterOSStatus< dataAlreadyClosed                >();
		RegisterOSStatus< endOfDataReached                 >();
		RegisterOSStatus< dataNoDataRef                    >();
		RegisterOSStatus< noMovieFound                     >();
		RegisterOSStatus< invalidDataRefContainer          >();
		RegisterOSStatus< badDataRefIndex                  >();
		RegisterOSStatus< noDefaultDataRef                 >();
		RegisterOSStatus< couldNotUseAnExistingSample      >();
		RegisterOSStatus< movieTextNotFoundErr             >();
		
		// QT for Windows errors
	//	RegisterOSStatus< noVideoTrackInMovieErr           >();
	//	RegisterOSStatus< noSoundTrackInMovieErr           >();
	//	RegisterOSStatus< soundSupportNotAvailableErr      >();
		
		RegisterOSStatus< unsupportedAuxiliaryImportData    >();
		RegisterOSStatus< auxiliaryExportDataUnavailable    >();
		RegisterOSStatus< samplesAlreadyInMediaErr         >();
		RegisterOSStatus< noSourceTreeFoundErr             >();
		RegisterOSStatus< sourceNotFoundErr                >();
		RegisterOSStatus< movieTextNotFoundErr             >();
		RegisterOSStatus< missingRequiredParameterErr      >();
		RegisterOSStatus< invalidSpriteWorldPropertyErr    >();
		RegisterOSStatus< invalidSpritePropertyErr         >();
		RegisterOSStatus< gWorldsNotSameDepthAndSizeErr    >();
		RegisterOSStatus< invalidSpriteIndexErr            >();
		RegisterOSStatus< invalidImageIndexErr             >();
		RegisterOSStatus< invalidSpriteIDErr               >();
		
		RegisterOSStatus< internalComponentErr             >();
		RegisterOSStatus< notImplementedMusicOSErr         >();
		RegisterOSStatus< cantSendToSynthesizerOSErr       >();
		RegisterOSStatus< cantReceiveFromSynthesizerOSErr    >();
		RegisterOSStatus< illegalVoiceAllocationOSErr      >();
		RegisterOSStatus< illegalPartOSErr                 >();
		RegisterOSStatus< illegalChannelOSErr              >();
		RegisterOSStatus< illegalKnobOSErr                 >();
		RegisterOSStatus< illegalKnobValueOSErr            >();
		RegisterOSStatus< illegalInstrumentOSErr           >();
		RegisterOSStatus< illegalControllerOSErr           >();
		RegisterOSStatus< midiManagerAbsentOSErr           >();
		RegisterOSStatus< synthesizerNotRespondingOSErr    >();
		RegisterOSStatus< synthesizerOSErr                 >();
		RegisterOSStatus< illegalNoteChannelOSErr          >();
		RegisterOSStatus< noteChannelNotAllocatedOSErr     >();
		RegisterOSStatus< tunePlayerFullOSErr              >();
		RegisterOSStatus< tuneParseOSErr                   >();
		RegisterOSStatus< noExportProcAvailableErr         >();
		RegisterOSStatus< videoOutputInUseErr              >();
		
		// Windows specific errors
	//	RegisterOSStatus< componentDllLoadErr              >();
	//	RegisterOSStatus< componentDllEntryNotFoundErr     >();
	//	RegisterOSStatus< qtmlDllLoadErr                   >();
	//	RegisterOSStatus< qtmlDllEntryNotFoundErr          >();
		
		RegisterOSStatus< qtmlUninitialized                >();
		RegisterOSStatus< unsupportedOSErr                 >();
		RegisterOSStatus< unsupportedProcessorErr          >();
		
		RegisterOSStatus< cannotFindAtomErr                >();
		RegisterOSStatus< notLeafAtomErr                   >();
		RegisterOSStatus< atomsNotOfSameTypeErr            >();
		RegisterOSStatus< atomIndexInvalidErr              >();
		RegisterOSStatus< duplicateAtomTypeAndIDErr        >();
		RegisterOSStatus< invalidAtomErr                   >();
		RegisterOSStatus< invalidAtomContainerErr          >();
		RegisterOSStatus< invalidAtomTypeErr               >();
		RegisterOSStatus< cannotBeLeafAtomErr              >();
		RegisterOSStatus< pathTooLongErr                   >();
		RegisterOSStatus< emptyPathErr                     >();
		RegisterOSStatus< noPathMappingErr                 >();
		RegisterOSStatus< pathNotVerifiedErr               >();
		RegisterOSStatus< unknownFormatErr                 >();
		RegisterOSStatus< wackBadFileErr                   >();
		RegisterOSStatus< wackForkNotFoundErr              >();
		RegisterOSStatus< wackBadMetaDataErr               >();
		RegisterOSStatus< qfcbNotFoundErr                  >();
		RegisterOSStatus< qfcbNotCreatedErr                >();
		RegisterOSStatus< ::AAPNotCreatedErr               >();
		RegisterOSStatus< ::AAPNotFoundErr                 >();
		RegisterOSStatus< ::ASDBadHeaderErr                >();
		RegisterOSStatus< ::ASDBadForkErr                  >();
		RegisterOSStatus< ::ASDEntryNotFoundErr            >();
		RegisterOSStatus< fileOffsetTooBigErr              >();
		RegisterOSStatus< notAllowedToSaveMovieErr         >();
		RegisterOSStatus< qtNetworkAlreadyAllocatedErr     >();
		RegisterOSStatus< urlDataHHTTPProtocolErr          >();
		RegisterOSStatus< urlDataHHTTPNoNetDriverErr       >();
		RegisterOSStatus< urlDataHHTTPURLErr               >();
		RegisterOSStatus< urlDataHHTTPRedirectErr          >();
		RegisterOSStatus< urlDataHFTPProtocolErr           >();
		RegisterOSStatus< urlDataHFTPShutdownErr           >();
		RegisterOSStatus< urlDataHFTPBadUserErr            >();
		RegisterOSStatus< urlDataHFTPBadPasswordErr        >();
		RegisterOSStatus< urlDataHFTPServerErr             >();
		RegisterOSStatus< urlDataHFTPDataConnectionErr     >();
		RegisterOSStatus< urlDataHFTPNoDirectoryErr        >();
		RegisterOSStatus< urlDataHFTPQuotaErr              >();
		RegisterOSStatus< urlDataHFTPPermissionsErr        >();
		RegisterOSStatus< urlDataHFTPFilenameErr           >();
		RegisterOSStatus< urlDataHFTPNoNetDriverErr        >();
		RegisterOSStatus< urlDataHFTPBadNameListErr        >();
		RegisterOSStatus< urlDataHFTPNeedPasswordErr       >();
		RegisterOSStatus< urlDataHFTPNoPasswordErr         >();
		RegisterOSStatus< urlDataHFTPServerDisconnectedErr >();
		RegisterOSStatus< urlDataHFTPURLErr                >();
		RegisterOSStatus< notEnoughDataErr                 >();
		RegisterOSStatus< qtActionNotHandledErr            >();
		RegisterOSStatus< qtXMLParseErr                    >();
		RegisterOSStatus< qtXMLApplicationErr              >();
		
		RegisterOSStatus< digiUnimpErr                     >();
		RegisterOSStatus< qtParamErr                       >();
		RegisterOSStatus< matrixErr                        >();
		RegisterOSStatus< notExactMatrixErr                >();
		RegisterOSStatus< noMoreKeyColorsErr               >();
		RegisterOSStatus< notExactSizeErr                  >();
		RegisterOSStatus< badDepthErr                      >();
		RegisterOSStatus< noDMAErr                         >();
		RegisterOSStatus< badCallOrderErr                  >();
	}
	
}

