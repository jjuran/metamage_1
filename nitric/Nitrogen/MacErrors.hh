// Nitrogen/MacErrors.hh
// ---------------------

// Part of the Nitrogen project.
//
// Written 2002-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_MACERRORS_HH
#define NITROGEN_MACERRORS_HH

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

namespace Nitrogen
{
	
	DEFINE_OSSTATUS( paramErr,                                ParamErr                               );
	DEFINE_OSSTATUS( noHardwareErr,                           NoHardwareErr                          );
	DEFINE_OSSTATUS( notEnoughHardwareErr,                    NotEnoughHardwareErr                   );
	DEFINE_OSSTATUS( userCanceledErr,                         UserCanceledErr                        );
	DEFINE_OSSTATUS( qErr,                                    QErr                                   );
	DEFINE_OSSTATUS( vTypErr,                                 VTypErr                                );
	DEFINE_OSSTATUS( corErr,                                  CorErr                                 );
	DEFINE_OSSTATUS( unimpErr,                                UnimpErr                               );
	DEFINE_OSSTATUS( ::SlpTypeErr,                            SlpTypeErr                             );
	DEFINE_OSSTATUS( seNoDB,                                  SeNoDB                                 );
	DEFINE_OSSTATUS( controlErr,                              ControlErr                             );
	DEFINE_OSSTATUS( statusErr,                               StatusErr                              );
	DEFINE_OSSTATUS( readErr,                                 ReadErr                                );
	DEFINE_OSSTATUS( writErr,                                 WritErr                                );
	DEFINE_OSSTATUS( badUnitErr,                              BadUnitErr                             );
	DEFINE_OSSTATUS( unitEmptyErr,                            UnitEmptyErr                           );
	DEFINE_OSSTATUS( openErr,                                 OpenErr                                );
	DEFINE_OSSTATUS( closErr,                                 ClosErr                                );
	DEFINE_OSSTATUS( dRemovErr,                               DRemovErr                              );
	DEFINE_OSSTATUS( dInstErr,                                DInstErr                               );

	DEFINE_OSSTATUS( abortErr,                                AbortErr                               );
	DEFINE_OSSTATUS( iIOAbortErr,                             IIOAbortErr                            );
	DEFINE_OSSTATUS( notOpenErr,                              NotOpenErr                             );
	DEFINE_OSSTATUS( unitTblFullErr,                          UnitTblFullErr                         );
	DEFINE_OSSTATUS( dceExtErr,                               DCEExtErr                              );
	DEFINE_OSSTATUS( slotNumErr,                              SlotNumErr                             );
	DEFINE_OSSTATUS( gcrOnMFMErr,                             GcrOnMFMErr                            );
	DEFINE_OSSTATUS( dirFulErr,                               DirFulErr                              );
	DEFINE_OSSTATUS( dskFulErr,                               DskFulErr                              );
	DEFINE_OSSTATUS( nsvErr,                                  NSVErr                                 );
	DEFINE_OSSTATUS( ioErr,                                   IOErr                                  );
	DEFINE_OSSTATUS( bdNamErr,                                BdNamErr                               );
	DEFINE_OSSTATUS( fnOpnErr,                                FNOpnErr                               );
	DEFINE_OSSTATUS( eofErr,                                  EOFErr                                 );
	DEFINE_OSSTATUS( posErr,                                  PosErr                                 );
	DEFINE_OSSTATUS( mFulErr,                                 MFulErr                                );
	DEFINE_OSSTATUS( tmfoErr,                                 TMFOErr                                );
	DEFINE_OSSTATUS( fnfErr,                                  FNFErr                                 );
	DEFINE_OSSTATUS( wPrErr,                                  WPrErr                                 );
	DEFINE_OSSTATUS( fLckdErr,                                FLckdErr                               );

	DEFINE_OSSTATUS( vLckdErr,                                VLckdErr                               );
	DEFINE_OSSTATUS( fBsyErr,                                 FBsyErr                                );
	DEFINE_OSSTATUS( dupFNErr,                                DupFNErr                               );
	DEFINE_OSSTATUS( opWrErr,                                 OpWrErr                                );
	DEFINE_OSSTATUS( rfNumErr,                                RfNumErr                               );
	DEFINE_OSSTATUS( gfpErr,                                  GFPErr                                 );
	DEFINE_OSSTATUS( volOffLinErr,                            VolOffLinErr                           );
	DEFINE_OSSTATUS( permErr,                                 PermErr                                );
	DEFINE_OSSTATUS( volOnLinErr,                             VolOnLinErr                            );
	DEFINE_OSSTATUS( nsDrvErr,                                NSDrvErr                               );
	DEFINE_OSSTATUS( noMacDskErr,                             NoMacDskErr                            );
	DEFINE_OSSTATUS( extFSErr,                                ExtFSErr                               );
	DEFINE_OSSTATUS( fsRnErr,                                 FSRnErr                                );
	DEFINE_OSSTATUS( badMDBErr,                               BadMDBErr                              );
	DEFINE_OSSTATUS( wrPermErr,                               WrPermErr                              );
	DEFINE_OSSTATUS( dirNFErr,                                DirNFErr                               );
	DEFINE_OSSTATUS( tmwdoErr,                                TMWDOErr                               );
	DEFINE_OSSTATUS( badMovErr,                               BadMovErr                              );
	DEFINE_OSSTATUS( wrgVolTypErr,                            WrgVolTypErr                           );
	DEFINE_OSSTATUS( volGoneErr,                              VolGoneErr                             );

	DEFINE_OSSTATUS( fidNotFound,                             FIDNotFound                            );
	DEFINE_OSSTATUS( fidExists,                               FIDExists                              );
	DEFINE_OSSTATUS( notAFileErr,                             NotAFileErr                            );
	DEFINE_OSSTATUS( diffVolErr,                              DiffVolErr                             );
	DEFINE_OSSTATUS( catChangedErr,                           CatChangedErr                          );
	DEFINE_OSSTATUS( desktopDamagedErr,                       DesktopDamagedErr                      );
	DEFINE_OSSTATUS( sameFileErr,                             SameFileErr                            );
	DEFINE_OSSTATUS( badFidErr,                               BadFidErr                              );
	DEFINE_OSSTATUS( notARemountErr,                          NotARemountErr                         );
	DEFINE_OSSTATUS( fileBoundsErr,                           FileBoundsErr                          );
	DEFINE_OSSTATUS( fsDataTooBigErr,                         FSDataTooBigErr                        );
	DEFINE_OSSTATUS( volVMBusyErr,                            VolVMBusyErr                           );
	DEFINE_OSSTATUS( badFCBErr,                               BadFCBErr                              );
	DEFINE_OSSTATUS( errFSUnknownCall,                        ErrFSUnknownCall                       );
	DEFINE_OSSTATUS( errFSBadFSRef,                           ErrFSBadFSRef                          );
	DEFINE_OSSTATUS( errFSBadForkName,                        ErrFSBadForkName                       );
	DEFINE_OSSTATUS( errFSBadBuffer,                          ErrFSBadBuffer                         );
	DEFINE_OSSTATUS( errFSBadForkRef,                         ErrFSBadForkRef                        );
	DEFINE_OSSTATUS( errFSBadInfoBitmap,                      ErrFSBadInfoBitmap                     );
	DEFINE_OSSTATUS( errFSMissingCatInfo,                     ErrFSMissingCatInfo                    );
	DEFINE_OSSTATUS( errFSNotAFolder,                         ErrFSNotAFolder                        );
	DEFINE_OSSTATUS( errFSForkNotFound,                       ErrFSForkNotFound                      );
	DEFINE_OSSTATUS( errFSNameTooLong,                        ErrFSNameTooLong                       );
	DEFINE_OSSTATUS( errFSMissingName,                        ErrFSMissingName                       );
	DEFINE_OSSTATUS( errFSBadPosMode,                         ErrFSBadPosMode                        );
	DEFINE_OSSTATUS( errFSBadAllocFlags,                      ErrFSBadAllocFlags                     );
	DEFINE_OSSTATUS( errFSNoMoreItems,                        ErrFSNoMoreItems                       );
	DEFINE_OSSTATUS( errFSBadItemCount,                       ErrFSBadItemCount                      );
	DEFINE_OSSTATUS( errFSBadSearchParams,                    ErrFSBadSearchParams                   );
	DEFINE_OSSTATUS( errFSRefsDifferent,                      ErrFSRefsDifferent                     );
	DEFINE_OSSTATUS( errFSForkExists,                         ErrFSForkExists                        );
	DEFINE_OSSTATUS( errFSBadIteratorFlags,                   ErrFSBadIteratorFlags                  );
	DEFINE_OSSTATUS( errFSIteratorNotFound,                   ErrFSIteratorNotFound                  );
	DEFINE_OSSTATUS( errFSIteratorNotSupported,               ErrFSIteratorNotSupported              );
	DEFINE_OSSTATUS( envNotPresent,                           EnvNotPresent                          );
	DEFINE_OSSTATUS( envBadVers,                              EnvBadVers                             );
	DEFINE_OSSTATUS( envVersTooBig,                           EnvVersTooBig                          );
	DEFINE_OSSTATUS( fontDecError,                            FontDecError                           );
	DEFINE_OSSTATUS( fontNotDeclared,                         FontNotDeclared                        );
	DEFINE_OSSTATUS( fontSubErr,                              FontSubErr                             );
	DEFINE_OSSTATUS( fontNotOutlineErr,                       FontNotOutlineErr                      );
	DEFINE_OSSTATUS( firstDskErr,                             FirstDskErr                            );
	DEFINE_OSSTATUS( lastDskErr,                              LastDskErr                             );
	DEFINE_OSSTATUS( noDriveErr,                              NoDriveErr                             );
	DEFINE_OSSTATUS( offLinErr,                               OffLinErr                              );
	DEFINE_OSSTATUS( noNybErr,                                NoNybErr                               );

	DEFINE_OSSTATUS( noAdrMkErr,                              NoAdrMkErr                             );
	DEFINE_OSSTATUS( dataVerErr,                              DataVerErr                             );
	DEFINE_OSSTATUS( badCksmErr,                              BadCksmErr                             );
	DEFINE_OSSTATUS( badBtSlpErr,                             BadBtSlpErr                            );
	DEFINE_OSSTATUS( noDtaMkErr,                              NoDtaMkErr                             );
	DEFINE_OSSTATUS( badDCksum,                               BadDCksum                              );
	DEFINE_OSSTATUS( badDBtSlp,                               BadDBtSlp                              );
	DEFINE_OSSTATUS( wrUnderrun,                              WrUnderrun                             );
	DEFINE_OSSTATUS( cantStepErr,                             CantStepErr                            );
	DEFINE_OSSTATUS( tk0BadErr,                               Tk0BadErr                              );
	DEFINE_OSSTATUS( initIWMErr,                              InitIWMErr                             );
	DEFINE_OSSTATUS( twoSideErr,                              TwoSideErr                             );
	DEFINE_OSSTATUS( spdAdjErr,                               SpdAdjErr                              );
	DEFINE_OSSTATUS( seekErr,                                 SeekErr                                );
	DEFINE_OSSTATUS( sectNFErr,                               SectNFErr                              );
	DEFINE_OSSTATUS( fmt1Err,                                 Fmt1Err                                );
	DEFINE_OSSTATUS( fmt2Err,                                 Fmt2Err                                );
	DEFINE_OSSTATUS( verErr,                                  VerErr                                 );
	DEFINE_OSSTATUS( clkRdErr,                                ClkRdErr                               );
	DEFINE_OSSTATUS( clkWrErr,                                ClkWrErr                               );
	DEFINE_OSSTATUS( prWrErr,                                 PrWrErr                                );
	DEFINE_OSSTATUS( prInitErr,                               PrInitErr                              );
	DEFINE_OSSTATUS( rcvrErr,                                 RcvrErr                                );
	DEFINE_OSSTATUS( breakRecd,                               BreakRecd                              );

/*Scrap Manager errors*/
	DEFINE_OSSTATUS( noScrapErr,                              NoScrapErr                             );
	DEFINE_OSSTATUS( noTypeErr,                               NoTypeErr                              );

/* ENET error codes */
	DEFINE_OSSTATUS( eLenErr,                                 ELenErr                                );
	DEFINE_OSSTATUS( eMultiErr,                               EMultiErr                              );

	DEFINE_OSSTATUS( ddpSktErr,                               DDPSktErr                              );
	DEFINE_OSSTATUS( ddpLenErr,                               DDPLenErr                              );
	DEFINE_OSSTATUS( noBridgeErr,                             NoBridgeErr                            );
	DEFINE_OSSTATUS( lapProtErr,                              LAPProtErr                             );
	DEFINE_OSSTATUS( excessCollsns,                           ExcessCollsns                          );
	DEFINE_OSSTATUS( portNotPwr,                              PortNotPwr                             );
	DEFINE_OSSTATUS( portInUse,                               PortInUse                              );
	DEFINE_OSSTATUS( portNotCf,                               PortNotCf                              );

/* Memory Manager errors*/
	DEFINE_OSSTATUS( memROZWarn,                              MemROZWarn                             );
	DEFINE_OSSTATUS( memROZError,                             MemROZError                            );
	DEFINE_OSSTATUS( memROZErr,                               MemROZErr                              );
	DEFINE_OSSTATUS( memFullErr,                              MemFullErr                             );
	DEFINE_OSSTATUS( nilHandleErr,                            NilHandleErr                           );
	DEFINE_OSSTATUS( memWZErr,                                MemWZErr                               );
	DEFINE_OSSTATUS( memPurErr,                               MemPurErr                              );
	DEFINE_OSSTATUS( memAdrErr,                               MemAdrErr                              );
	DEFINE_OSSTATUS( memAZErr,                                MemAZErr                               );
	DEFINE_OSSTATUS( memPCErr,                                MemPCErr                               );
	DEFINE_OSSTATUS( memBCErr,                                MemBCErr                               );
	DEFINE_OSSTATUS( memSCErr,                                MemSCErr                               );
	DEFINE_OSSTATUS( memLockedErr,                            MemLockedErr                           );

/* Printing Errors */
	DEFINE_OSSTATUS( iMemFullErr,                             IMemFullErr                            );
	DEFINE_OSSTATUS( iIOAbort,                                IIOAbort                               );


	DEFINE_OSSTATUS( resourceInMemory,                        ResourceInMemory                       );
	DEFINE_OSSTATUS( writingPastEnd,                          WritingPastEnd                         );
	DEFINE_OSSTATUS( inputOutOfBounds,                        InputOutOfBounds                       );
	DEFINE_OSSTATUS( resNotFound,                             ResNotFound                            );
	DEFINE_OSSTATUS( resFNotFound,                            ResFNotFound                           );
	DEFINE_OSSTATUS( addResFailed,                            AddResFailed                           );
	DEFINE_OSSTATUS( addRefFailed,                            AddRefFailed                           );
	DEFINE_OSSTATUS( rmvResFailed,                            RmvResFailed                           );
	DEFINE_OSSTATUS( rmvRefFailed,                            RmvRefFailed                           );
	DEFINE_OSSTATUS( resAttrErr,                              ResAttrErr                             );
	DEFINE_OSSTATUS( mapReadErr,                              MapReadErr                             );
	DEFINE_OSSTATUS( ::CantDecompress,                        CantDecompress                         );
	DEFINE_OSSTATUS( badExtResource,                          BadExtResource                         );
	DEFINE_OSSTATUS( noMemForPictPlaybackErr,                 NoMemForPictPlaybackErr                );
	DEFINE_OSSTATUS( rgnOverflowErr,                          RgnOverflowErr                         );
	DEFINE_OSSTATUS( rgnTooBigError,                          RgnTooBigError                         );
	DEFINE_OSSTATUS( pixMapTooDeepErr,                        PixMapTooDeepErr                       );
	DEFINE_OSSTATUS( insufficientStackErr,                    InsufficientStackErr                   );
	DEFINE_OSSTATUS( nsStackErr,                              NSStackErr                             );

	DEFINE_OSSTATUS( evtNotEnb,                               EvtNotEnb                              );

/* OffScreen QuickDraw Errors */
	DEFINE_OSSTATUS( cMatchErr,                               CMatchErr                              );
	DEFINE_OSSTATUS( cTempMemErr,                             CTempMemErr                            );
	DEFINE_OSSTATUS( cNoMemErr,                               CNoMemErr                              );
	DEFINE_OSSTATUS( cRangeErr,                               CRangeErr                              );
	DEFINE_OSSTATUS( cProtectErr,                             CProtectErr                            );
	DEFINE_OSSTATUS( cDevErr,                                 CDevErr                                );
	DEFINE_OSSTATUS( cResErr,                                 CResErr                                );
	DEFINE_OSSTATUS( cDepthErr,                               CDepthErr                              );
	DEFINE_OSSTATUS( rgnTooBigErr,                            RgnTooBigErr                           );
	DEFINE_OSSTATUS( updPixMemErr,                            UpdPixMemErr                           );
	DEFINE_OSSTATUS( pictInfoVersionErr,                      PictInfoVersionErr                     );
	DEFINE_OSSTATUS( pictInfoIDErr,                           PictInfoIDErr                          );
	DEFINE_OSSTATUS( pictInfoVerbErr,                         PictInfoVerbErr                        );
	DEFINE_OSSTATUS( cantLoadPickMethodErr,                   CantLoadPickMethodErr                  );
	DEFINE_OSSTATUS( colorsRequestedErr,                      ColorsRequestedErr                     );
	DEFINE_OSSTATUS( pictureDataErr,                          PictureDataErr                         );

/* ColorSync Result codes */
	DEFINE_OSSTATUS( cmProfileError,                          CMProfileError                         );
	DEFINE_OSSTATUS( cmMethodError,                           CMMethodError                          );
	DEFINE_OSSTATUS( cmMethodNotFound,                        CMMethodNotFound                       );
	DEFINE_OSSTATUS( cmProfileNotFound,                       CMProfileNotFound                      );
	DEFINE_OSSTATUS( cmProfilesIdentical,                     CMProfilesIdentical                    );
	DEFINE_OSSTATUS( cmCantConcatenateError,                  CMCantConcatenateError                 );
	DEFINE_OSSTATUS( cmCantXYZ,                               CMCantXYZ                              );
	DEFINE_OSSTATUS( cmCantDeleteProfile,                     CMCantDeleteProfile                    );
	DEFINE_OSSTATUS( cmUnsupportedDataType,                   CMUnsupportedDataType                  );
	DEFINE_OSSTATUS( cmNoCurrentProfile,                      CMNoCurrentProfile                     );


/*Sound Manager errors*/
	DEFINE_OSSTATUS( noHardware,                              NoHardware                             );
	DEFINE_OSSTATUS( notEnoughHardware,                       NotEnoughHardware                      );
	DEFINE_OSSTATUS( queueFull,                               QueueFull                              );
	DEFINE_OSSTATUS( resProblem,                              ResProblem                             );
	DEFINE_OSSTATUS( badChannel,                              BadChannel                             );
	DEFINE_OSSTATUS( badFormat,                               BadFormat                              );
	DEFINE_OSSTATUS( notEnoughBufferSpace,                    NotEnoughBufferSpace                   );
	DEFINE_OSSTATUS( badFileFormat,                           BadFileFormat                          );
	DEFINE_OSSTATUS( channelBusy,                             ChannelBusy                            );
	DEFINE_OSSTATUS( buffersTooSmall,                         BuffersTooSmall                        );
	DEFINE_OSSTATUS( channelNotBusy,                          ChannelNotBusy                         );
	DEFINE_OSSTATUS( noMoreRealTime,                          NoMoreRealTime                         );
	DEFINE_OSSTATUS( siVBRCompressionNotSupported,            SIVBRCompressionNotSupported           );
	DEFINE_OSSTATUS( siNoSoundInHardware,                     SINoSoundInHardware                    );
	DEFINE_OSSTATUS( siBadSoundInDevice,                      SIBadSoundInDevice                     );
	DEFINE_OSSTATUS( siNoBufferSpecified,                     SINoBufferSpecified                    );
	DEFINE_OSSTATUS( siInvalidCompression,                    SIInvalidCompression                   );
	DEFINE_OSSTATUS( siHardDriveTooSlow,                      SIHardDriveTooSlow                     );
	DEFINE_OSSTATUS( siInvalidSampleRate,                     SIInvalidSampleRate                    );
	DEFINE_OSSTATUS( siInvalidSampleSize,                     SIInvalidSampleSize                    );
	DEFINE_OSSTATUS( siDeviceBusyErr,                         SIDeviceBusyErr                        );
	DEFINE_OSSTATUS( siBadDeviceName,                         SIBadDeviceName                        );
	DEFINE_OSSTATUS( siBadRefNum,                             SIBadRefNum                            );
	DEFINE_OSSTATUS( siInputDeviceErr,                        SIInputDeviceErr                       );
	DEFINE_OSSTATUS( siUnknownInfoType,                       SIUnknownInfoType                      );
	DEFINE_OSSTATUS( siUnknownQuality,                        SIUnknownQuality                       );

/*Speech Manager errors*/
	DEFINE_OSSTATUS( noSynthFound,                            NoSynthFound                           );
	DEFINE_OSSTATUS( synthOpenFailed,                         SynthOpenFailed                        );
	DEFINE_OSSTATUS( synthNotReady,                           SynthNotReady                          );
	DEFINE_OSSTATUS( bufTooSmall,                             BufTooSmall                            );
	DEFINE_OSSTATUS( voiceNotFound,                           VoiceNotFound                          );
	DEFINE_OSSTATUS( incompatibleVoice,                       IncompatibleVoice                      );
	DEFINE_OSSTATUS( badDictFormat,                           BadDictFormat                          );
	DEFINE_OSSTATUS( badInputText,                            BadInputText                           );

/* Midi Manager Errors: */
	DEFINE_OSSTATUS( midiNoClientErr,                         MIDINoClientErr                        );
	DEFINE_OSSTATUS( midiNoPortErr,                           MIDINoPortErr                          );
	DEFINE_OSSTATUS( midiTooManyPortsErr,                     MIDITooManyPortsErr                    );
	DEFINE_OSSTATUS( midiTooManyConsErr,                      MIDITooManyConsErr                     );
	DEFINE_OSSTATUS( midiVConnectErr,                         MIDIVConnectErr                        );
	DEFINE_OSSTATUS( midiVConnectMade,                        MIDIVConnectMade                       );
	DEFINE_OSSTATUS( midiVConnectRmvd,                        MIDIVConnectRmvd                       );
	DEFINE_OSSTATUS( midiNoConErr,                            MIDINoConErr                           );
	DEFINE_OSSTATUS( midiWriteErr,                            MIDIWriteErr                           );
	DEFINE_OSSTATUS( midiNameLenErr,                          MIDINameLenErr                         );
	DEFINE_OSSTATUS( midiDupIDErr,                            MIDIDupIDErr                           );
	DEFINE_OSSTATUS( midiInvalidCmdErr,                       MIDIInvalidCmdErr                      );


	DEFINE_OSSTATUS( nmTypErr,                                NMTypErr                               );


	DEFINE_OSSTATUS( siInitSDTblErr,                          SIInitSDTblErr                         );
	DEFINE_OSSTATUS( siInitVBLQsErr,                          SIInitVBLQsErr                         );
	DEFINE_OSSTATUS( siInitSPTblErr,                          SIInitSPTblErr                         );
	DEFINE_OSSTATUS( sdmJTInitErr,                            SDMJTInitErr                           );
	DEFINE_OSSTATUS( sdmInitErr,                              SDMInitErr                             );
	DEFINE_OSSTATUS( sdmSRTInitErr,                           SDMSRTInitErr                          );
	DEFINE_OSSTATUS( sdmPRAMInitErr,                          SDMPRAMInitErr                         );
	DEFINE_OSSTATUS( sdmPriInitErr,                           SDMPriInitErr                          );

	DEFINE_OSSTATUS( smSDMInitErr,                            SMSDMInitErr                           );
	DEFINE_OSSTATUS( smSRTInitErr,                            SMSRTInitErr                           );
	DEFINE_OSSTATUS( smPRAMInitErr,                           SMPRAMInitErr                          );
	DEFINE_OSSTATUS( smPriInitErr,                            SMPriInitErr                           );
	DEFINE_OSSTATUS( smEmptySlot,                             SMEmptySlot                            );
	DEFINE_OSSTATUS( smCRCFail,                               SMCRCFail                              );
	DEFINE_OSSTATUS( smFormatErr,                             SMFormatErr                            );
	DEFINE_OSSTATUS( smRevisionErr,                           SMRevisionErr                          );
	DEFINE_OSSTATUS( smNoDir,                                 SMNoDir                                );
	DEFINE_OSSTATUS( smDisabledSlot,                          SMDisabledSlot                         );
	DEFINE_OSSTATUS( smNosInfoArray,                          SMNosInfoArray                         );


	DEFINE_OSSTATUS( smResrvErr,                              SMResrvErr                             );
	DEFINE_OSSTATUS( smUnExBusErr,                            SMUnExBusErr                           );
	DEFINE_OSSTATUS( smBLFieldBad,                            SMBLFieldBad                           );
	DEFINE_OSSTATUS( smFHBlockRdErr,                          SMFHBlockRdErr                         );
	DEFINE_OSSTATUS( smFHBlkDispErr,                          SMFHBlkDispErr                         );
	DEFINE_OSSTATUS( smDisposePErr,                           SMDisposePErr                          );
	DEFINE_OSSTATUS( smNoBoardSRsrc,                          SMNoBoardSRsrc                         );
	DEFINE_OSSTATUS( smGetPRErr,                              SMGetPRErr                             );
	DEFINE_OSSTATUS( smNoBoardId,                             SMNoBoardId                            );
	DEFINE_OSSTATUS( smInitStatVErr,                          SMInitStatVErr                         );
	DEFINE_OSSTATUS( smInitTblVErr,                           SMInitTblVErr                          );
	DEFINE_OSSTATUS( smNoJmpTbl,                              SMNoJmpTbl                             );
	DEFINE_OSSTATUS( smReservedSlot,                          SMReservedSlot                         );
	DEFINE_OSSTATUS( smBadBoardId,                            SMBadBoardId                           );
	DEFINE_OSSTATUS( smBusErrTO,                              SMBusErrTO                             );

	DEFINE_OSSTATUS( svTempDisable,                           SVTempDisable                          );
	DEFINE_OSSTATUS( svDisabled,                              SVDisabled                             );
	DEFINE_OSSTATUS( smBadRefId,                              SMBadRefId                             );
	DEFINE_OSSTATUS( smBadsList,                              SMBadsList                             );
	DEFINE_OSSTATUS( smReservedErr,                           SMReservedErr                          );
	DEFINE_OSSTATUS( smCodeRevErr,                            SMCodeRevErr                           );

	DEFINE_OSSTATUS( smCPUErr,                                SMCPUErr                               );
	DEFINE_OSSTATUS( smsPointerNil,                           SMSPointerNil                          );
	DEFINE_OSSTATUS( smNilsBlockErr,                          SMNilsBlockErr                         );
	DEFINE_OSSTATUS( smSlotOOBErr,                            SMSlotOOBErr                           );
	DEFINE_OSSTATUS( smSelOOBErr,                             SMSelOOBErr                            );
	DEFINE_OSSTATUS( smNewPErr,                               SMNewPErr                              );
	DEFINE_OSSTATUS( smBlkMoveErr,                            SMBlkMoveErr                           );
	DEFINE_OSSTATUS( smCkStatusErr,                           SMCkStatusErr                          );
	DEFINE_OSSTATUS( smGetDrvrNamErr,                         SMGetDrvrNamErr                        );
	DEFINE_OSSTATUS( smDisDrvrNamErr,                         SMDisDrvrNamErr                        );
	DEFINE_OSSTATUS( smNoMoresRsrcs,                          SMNoMoresRsrcs                         );
	DEFINE_OSSTATUS( smsGetDrvrErr,                           SMSGetDrvrErr                          );
	DEFINE_OSSTATUS( smBadsPtrErr,                            SMBadsPtrErr                           );
	DEFINE_OSSTATUS( smByteLanesErr,                          SMByteLanesErr                         );
	DEFINE_OSSTATUS( smOffsetErr,                             SMOffsetErr                            );
	DEFINE_OSSTATUS( smNoGoodOpens,                           SMNoGoodOpens                          );
	DEFINE_OSSTATUS( smSRTOvrFlErr,                           SMSRTOvrFlErr                          );
	DEFINE_OSSTATUS( smRecNotFnd,                             SMRecNotFnd                            );


/*Dictionary Manager errors*/
	DEFINE_OSSTATUS( notBTree,                                NotBTree                               );
	DEFINE_OSSTATUS( btNoSpace,                               BTNoSpace                              );
	DEFINE_OSSTATUS( btDupRecErr,                             BTDupRecErr                            );
	DEFINE_OSSTATUS( btRecNotFnd,                             BTRecNotFnd                            );
	DEFINE_OSSTATUS( btKeyLenErr,                             BTKeyLenErr                            );
	DEFINE_OSSTATUS( btKeyAttrErr,                            BTKeyAttrErr                           );
	DEFINE_OSSTATUS( unknownInsertModeErr,                    UnknownInsertModeErr                   );
	DEFINE_OSSTATUS( recordDataTooBigErr,                     RecordDataTooBigErr                    );
	DEFINE_OSSTATUS( invalidIndexErr,                         InvalidIndexErr                        );


/* Error codes from FSM functions */
	DEFINE_OSSTATUS( fsmFFSNotFoundErr,                       FSMFFSNotFoundErr                      );
	DEFINE_OSSTATUS( fsmBusyFFSErr,                           FSMBusyFFSErr                          );
	DEFINE_OSSTATUS( fsmBadFFSNameErr,                        FSMBadFFSNameErr                       );
	DEFINE_OSSTATUS( fsmBadFSDLenErr,                         FSMBadFSDLenErr                        );
	DEFINE_OSSTATUS( fsmDuplicateFSIDErr,                     FSMDuplicateFSIDErr                    );
	DEFINE_OSSTATUS( fsmBadFSDVersionErr,                     FSMBadFSDVersionErr                    );
	DEFINE_OSSTATUS( fsmNoAlternateStackErr,                  FSMNoAlternateStackErr                 );
	DEFINE_OSSTATUS( fsmUnknownFSMMessageErr,                 FSMUnknownFSMMessageErr                );


/* Edition Mgr errors*/
	DEFINE_OSSTATUS( editionMgrInitErr,                       EditionMgrInitErr                      );
	DEFINE_OSSTATUS( badSectionErr,                           BadSectionErr                          );
	DEFINE_OSSTATUS( notRegisteredSectionErr,                 NotRegisteredSectionErr                );
	DEFINE_OSSTATUS( badEditionFileErr,                       BadEditionFileErr                      );
	DEFINE_OSSTATUS( badSubPartErr,                           BadSubPartErr                          );
	DEFINE_OSSTATUS( multiplePublisherWrn,                    MultiplePublisherWrn                   );
	DEFINE_OSSTATUS( containerNotFoundWrn,                    ContainerNotFoundWrn                   );
	DEFINE_OSSTATUS( containerAlreadyOpenWrn,                 ContainerAlreadyOpenWrn                );
	DEFINE_OSSTATUS( notThePublisherWrn,                      NotThePublisherWrn                     );

	DEFINE_OSSTATUS( teScrapSizeErr,                          TEScrapSizeErr                         );
	DEFINE_OSSTATUS( hwParamErr,                              HWParamErr                             );
	DEFINE_OSSTATUS( driverHardwareGoneErr,                   DriverHardwareGoneErr                  );

/*Process Manager errors*/
	DEFINE_OSSTATUS( procNotFound,                            ProcNotFound                           );
	DEFINE_OSSTATUS( memFragErr,                              MemFragErr                             );
	DEFINE_OSSTATUS( appModeErr,                              AppModeErr                             );
	DEFINE_OSSTATUS( protocolErr,                             ProtocolErr                            );
	DEFINE_OSSTATUS( hardwareConfigErr,                       HardwareConfigErr                      );
	DEFINE_OSSTATUS( appMemFullErr,                           AppMemFullErr                          );
	DEFINE_OSSTATUS( appIsDaemon,                             AppIsDaemon                            );
	DEFINE_OSSTATUS( bufferIsSmall,                           BufferIsSmall                          );
	DEFINE_OSSTATUS( noOutstandingHLE,                        NoOutstandingHLE                       );
	DEFINE_OSSTATUS( connectionInvalid,                       ConnectionInvalid                      );
	DEFINE_OSSTATUS( noUserInteractionAllowed,                NoUserInteractionAllowed               );
	DEFINE_OSSTATUS( wrongApplicationPlatform,                WrongApplicationPlatform               );
	DEFINE_OSSTATUS( appVersionTooOld,                        AppVersionTooOld                       );
	DEFINE_OSSTATUS( notAppropriateForClassic,                NotAppropriateForClassic               );

/* Thread Manager Error Codes */
	DEFINE_OSSTATUS( threadTooManyReqsErr,                    ThreadTooManyReqsErr                   );
	DEFINE_OSSTATUS( threadNotFoundErr,                       ThreadNotFoundErr                      );
	DEFINE_OSSTATUS( threadProtocolErr,                       ThreadProtocolErr                      );

/*MemoryDispatch errors*/
	DEFINE_OSSTATUS( notEnoughMemoryErr,                      NotEnoughMemoryErr                     );
	DEFINE_OSSTATUS( notHeldErr,                              NotHeldErr                             );
	DEFINE_OSSTATUS( cannotMakeContiguousErr,                 CannotMakeContiguousErr                );
	DEFINE_OSSTATUS( notLockedErr,                            NotLockedErr                           );
	DEFINE_OSSTATUS( interruptsMaskedErr,                     InterruptsMaskedErr                    );
	DEFINE_OSSTATUS( cannotDeferErr,                          CannotDeferErr                         );
	DEFINE_OSSTATUS( noMMUErr,                                NoMMUErr                               );

/* FileMapping errors */
	DEFINE_OSSTATUS( vmInvalidBackingFileIDErr,               VMInvalidBackingFileIDErr              );
	DEFINE_OSSTATUS( vmMappingPrivilegesErr,                  VMMappingPrivilegesErr                 );
	DEFINE_OSSTATUS( vmBusyBackingFileErr,                    VMBusyBackingFileErr                   );
	DEFINE_OSSTATUS( vmNoMoreBackingFilesErr,                 VMNoMoreBackingFilesErr                );
	DEFINE_OSSTATUS( vmInvalidFileViewIDErr,                  VMInvalidFileViewIDErr                 );
	DEFINE_OSSTATUS( vmFileViewAccessErr,                     VMFileViewAccessErr                    );
	DEFINE_OSSTATUS( vmNoMoreFileViewsErr,                    VMNoMoreFileViewsErr                   );
	DEFINE_OSSTATUS( vmAddressNotInFileViewErr,               VMAddressNotInFileViewErr              );
	DEFINE_OSSTATUS( vmInvalidOwningProcessErr,               VMInvalidOwningProcessErr              );

/* Database access error codes */
	DEFINE_OSSTATUS( rcDBNull,                                RCDBNull                               );
	DEFINE_OSSTATUS( rcDBValue,                               RCDBValue                              );
	DEFINE_OSSTATUS( rcDBError,                               RCDBError                              );
	DEFINE_OSSTATUS( rcDBBadType,                             RCDBBadType                            );
	DEFINE_OSSTATUS( rcDBBreak,                               RCDBBreak                              );
	DEFINE_OSSTATUS( rcDBExec,                                RCDBExec                               );
	DEFINE_OSSTATUS( rcDBBadSessID,                           RCDBBadSessID                          );
	DEFINE_OSSTATUS( rcDBBadSessNum,                          RCDBBadSessNum                         );
	DEFINE_OSSTATUS( rcDBBadDDEV,                             RCDBBadDDEV                            );
	DEFINE_OSSTATUS( rcDBAsyncNotSupp,                        RCDBAsyncNotSupp                       );
	DEFINE_OSSTATUS( rcDBBadAsyncPB,                          RCDBBadAsyncPB                         );
	DEFINE_OSSTATUS( rcDBNoHandler,                           RCDBNoHandler                          );
	DEFINE_OSSTATUS( rcDBWrongVersion,                        RCDBWrongVersion                       );
	DEFINE_OSSTATUS( rcDBPackNotInited,                       RCDBPackNotInited                      );


/*Help Mgr error range: -850 to -874*/
	DEFINE_OSSTATUS( hmHelpDisabled,                          HMHelpDisabled                         );
	DEFINE_OSSTATUS( hmBalloonAborted,                        HMBalloonAborted                       );
	DEFINE_OSSTATUS( hmSameAsLastBalloon,                     HMSameAsLastBalloon                    );
	DEFINE_OSSTATUS( hmHelpManagerNotInited,                  HMHelpManagerNotInited                 );
	DEFINE_OSSTATUS( hmSkippedBalloon,                        HMSkippedBalloon                       );
	DEFINE_OSSTATUS( hmWrongVersion,                          HMWrongVersion                         );
	DEFINE_OSSTATUS( hmUnknownHelpType,                       HMUnknownHelpType                      );
	DEFINE_OSSTATUS( hmOperationUnsupported,                  HMOperationUnsupported                 );
	DEFINE_OSSTATUS( hmNoBalloonUp,                           HMNoBalloonUp                          );
	DEFINE_OSSTATUS( hmCloseViewActive,                       HMCloseViewActive                      );



/*PPC errors*/
	DEFINE_OSSTATUS( notInitErr,                              NotInitErr                             );
	DEFINE_OSSTATUS( nameTypeErr,                             NameTypeErr                            );
	DEFINE_OSSTATUS( noPortErr,                               NoPortErr                              );
	DEFINE_OSSTATUS( noGlobalsErr,                            NoGlobalsErr                           );
	DEFINE_OSSTATUS( localOnlyErr,                            LocalOnlyErr                           );
	DEFINE_OSSTATUS( destPortErr,                             DestPortErr                            );
	DEFINE_OSSTATUS( sessTableErr,                            SessTableErr                           );
	DEFINE_OSSTATUS( noSessionErr,                            NoSessionErr                           );
	DEFINE_OSSTATUS( badReqErr,                               BadReqErr                              );
	DEFINE_OSSTATUS( portNameExistsErr,                       PortNameExistsErr                      );
	DEFINE_OSSTATUS( noUserNameErr,                           NoUserNameErr                          );
	DEFINE_OSSTATUS( userRejectErr,                           UserRejectErr                          );
	DEFINE_OSSTATUS( noMachineNameErr,                        NoMachineNameErr                       );
	DEFINE_OSSTATUS( noToolboxNameErr,                        NoToolboxNameErr                       );
	DEFINE_OSSTATUS( noResponseErr,                           NoResponseErr                          );
	DEFINE_OSSTATUS( portClosedErr,                           PortClosedErr                          );
	DEFINE_OSSTATUS( sessClosedErr,                           SessClosedErr                          );
	DEFINE_OSSTATUS( badPortNameErr,                          BadPortNameErr                         );
	DEFINE_OSSTATUS( noDefaultUserErr,                        NoDefaultUserErr                       );
	DEFINE_OSSTATUS( notLoggedInErr,                          NotLoggedInErr                         );
	DEFINE_OSSTATUS( noUserRefErr,                            NoUserRefErr                           );
	DEFINE_OSSTATUS( networkErr,                              NetworkErr                             );
	DEFINE_OSSTATUS( noInformErr,                             NoInformErr                            );
	DEFINE_OSSTATUS( authFailErr,                             AuthFailErr                            );
	DEFINE_OSSTATUS( noUserRecErr,                            NoUserRecErr                           );
	DEFINE_OSSTATUS( badServiceMethodErr,                     BadServiceMethodErr                    );
	DEFINE_OSSTATUS( badLocNameErr,                           BadLocNameErr                          );
	DEFINE_OSSTATUS( guestNotAllowedErr,                      GuestNotAllowedErr                     );

/* Font Mgr errors*/
	DEFINE_OSSTATUS( kFMIterationCompleted,                   FMIterationCompleted                   );
	DEFINE_OSSTATUS( kFMInvalidFontFamilyErr,                 FMInvalidFontFamilyErr                 );
	DEFINE_OSSTATUS( kFMInvalidFontErr,                       FMInvalidFontErr                       );
	DEFINE_OSSTATUS( kFMIterationScopeModifiedErr,            FMIterationScopeModifiedErr            );
	DEFINE_OSSTATUS( kFMFontTableAccessErr,                   FMFontTableAccessErr                   );
	DEFINE_OSSTATUS( kFMFontContainerAccessErr,               FMFontContainerAccessErr               );

	DEFINE_OSSTATUS( noMaskFoundErr,                          NoMaskFoundErr                         );

	DEFINE_OSSTATUS( nbpBuffOvr,                              NBPBuffOvr                             );
	DEFINE_OSSTATUS( nbpNoConfirm,                            NBPNoConfirm                           );
	DEFINE_OSSTATUS( nbpConfDiff,                             NBPConfDiff                            );
	DEFINE_OSSTATUS( nbpDuplicate,                            NBPDuplicate                           );
	DEFINE_OSSTATUS( nbpNotFound,                             NBPNotFound                            );
	DEFINE_OSSTATUS( nbpNISErr,                               NBPNISErr                              );

	DEFINE_OSSTATUS( aspBadVersNum,                           ASPBadVersNum                          );
	DEFINE_OSSTATUS( aspBufTooSmall,                          ASPBufTooSmall                         );
	DEFINE_OSSTATUS( aspNoMoreSess,                           ASPNoMoreSess                          );
	DEFINE_OSSTATUS( aspNoServers,                            ASPNoServers                           );
	DEFINE_OSSTATUS( aspParamErr,                             ASPParamErr                            );
	DEFINE_OSSTATUS( aspServerBusy,                           ASPServerBusy                          );
	DEFINE_OSSTATUS( aspSessClosed,                           ASPSessClosed                          );
	DEFINE_OSSTATUS( aspSizeErr,                              ASPSizeErr                             );
	DEFINE_OSSTATUS( aspTooMany,                              ASPTooMany                             );
	DEFINE_OSSTATUS( aspNoAck,                                ASPNoAck                               );

	DEFINE_OSSTATUS( reqFailed,                               ReqFailed                              );
	DEFINE_OSSTATUS( tooManyReqs,                             TooManyReqs                            );
	DEFINE_OSSTATUS( tooManySkts,                             TooManySkts                            );
	DEFINE_OSSTATUS( badATPSkt,                               BadATPSkt                              );
	DEFINE_OSSTATUS( badBuffNum,                              BadBuffNum                             );
	DEFINE_OSSTATUS( noRelErr,                                NoRelErr                               );
	DEFINE_OSSTATUS( cbNotFound,                              CbNotFound                             );
	DEFINE_OSSTATUS( noSendResp,                              NoSendResp                             );
	DEFINE_OSSTATUS( noDataArea,                              NoDataArea                             );
	DEFINE_OSSTATUS( reqAborted,                              ReqAborted                             );

/* ADSP Error Codes */
	DEFINE_OSSTATUS( errRefNum,                               ErrRefNum                              );
	DEFINE_OSSTATUS( errAborted,                              ErrAborted                             );
	DEFINE_OSSTATUS( errState,                                ErrState                               );
	DEFINE_OSSTATUS( errOpening,                              ErrOpening                             );
	DEFINE_OSSTATUS( errAttention,                            ErrAttention                           );
	DEFINE_OSSTATUS( errFwdReset,                             ErrFwdReset                            );
	DEFINE_OSSTATUS( errDSPQueueSize,                         ErrDSPQueueSize                        );
	DEFINE_OSSTATUS( errOpenDenied,                           ErrOpenDenied                          );


/* Apple event manager error messages */
	DEFINE_OSSTATUS( errAECoercionFail,                       ErrAECoercionFail                      );
	DEFINE_OSSTATUS( errAEDescNotFound,                       ErrAEDescNotFound                      );
	DEFINE_OSSTATUS( errAECorruptData,                        ErrAECorruptData                       );
	DEFINE_OSSTATUS( errAEWrongDataType,                      ErrAEWrongDataType                     );
	DEFINE_OSSTATUS( errAENotAEDesc,                          ErrAENotAEDesc                         );
	DEFINE_OSSTATUS( errAEBadListItem,                        ErrAEBadListItem                       );
	DEFINE_OSSTATUS( errAENewerVersion,                       ErrAENewerVersion                      );
	DEFINE_OSSTATUS( errAENotAppleEvent,                      ErrAENotAppleEvent                     );
	DEFINE_OSSTATUS( errAEEventNotHandled,                    ErrAEEventNotHandled                   );
	DEFINE_OSSTATUS( errAEReplyNotValid,                      ErrAEReplyNotValid                     );
	DEFINE_OSSTATUS( errAEUnknownSendMode,                    ErrAEUnknownSendMode                   );
	DEFINE_OSSTATUS( errAEWaitCanceled,                       ErrAEWaitCanceled                      );
	DEFINE_OSSTATUS( errAETimeout,                            ErrAETimeout                           );
	DEFINE_OSSTATUS( errAENoUserInteraction,                  ErrAENoUserInteraction                 );
	DEFINE_OSSTATUS( errAENotASpecialFunction,                ErrAENotASpecialFunction               );
	DEFINE_OSSTATUS( errAEParamMissed,                        ErrAEParamMissed                       );
	DEFINE_OSSTATUS( errAEUnknownAddressType,                 ErrAEUnknownAddressType                );
	DEFINE_OSSTATUS( errAEHandlerNotFound,                    ErrAEHandlerNotFound                   );
	DEFINE_OSSTATUS( errAEReplyNotArrived,                    ErrAEReplyNotArrived                   );
	DEFINE_OSSTATUS( errAEIllegalIndex,                       ErrAEIllegalIndex                      );
	DEFINE_OSSTATUS( errAEImpossibleRange,                    ErrAEImpossibleRange                   );
	DEFINE_OSSTATUS( errAEWrongNumberArgs,                    ErrAEWrongNumberArgs                   );
	DEFINE_OSSTATUS( errAEAccessorNotFound,                   ErrAEAccessorNotFound                  );
	DEFINE_OSSTATUS( errAENoSuchLogical,                      ErrAENoSuchLogical                     );
	DEFINE_OSSTATUS( errAEBadTestKey,                         ErrAEBadTestKey                        );
	DEFINE_OSSTATUS( errAENotAnObjSpec,                       ErrAENotAnObjSpec                      );
	DEFINE_OSSTATUS( errAENoSuchObject,                       ErrAENoSuchObject                      );
	DEFINE_OSSTATUS( errAENegativeCount,                      ErrAENegativeCount                     );
	DEFINE_OSSTATUS( errAEEmptyListContainer,                 ErrAEEmptyListContainer                );
	DEFINE_OSSTATUS( errAEUnknownObjectType,                  ErrAEUnknownObjectType                 );
	DEFINE_OSSTATUS( errAERecordingIsAlreadyOn,               ErrAERecordingIsAlreadyOn              );
	DEFINE_OSSTATUS( errAEReceiveTerminate,                   ErrAEReceiveTerminate                  );
	DEFINE_OSSTATUS( errAEReceiveEscapeCurrent,               ErrAEReceiveEscapeCurrent              );
	DEFINE_OSSTATUS( errAEEventFiltered,                      ErrAEEventFiltered                     );
	DEFINE_OSSTATUS( errAEDuplicateHandler,                   ErrAEDuplicateHandler                  );
	DEFINE_OSSTATUS( errAEStreamBadNesting,                   ErrAEStreamBadNesting                  );
	DEFINE_OSSTATUS( errAEStreamAlreadyConverted,             ErrAEStreamAlreadyConverted            );
	DEFINE_OSSTATUS( errAEDescIsNull,                         ErrAEDescIsNull                        );
	DEFINE_OSSTATUS( errAEBuildSyntaxError,                   ErrAEBuildSyntaxError                  );
	DEFINE_OSSTATUS( errAEBufferTooSmall,                     ErrAEBufferTooSmall                    );

	DEFINE_OSSTATUS( errOSASystemError,                       ErrOSASystemError                      );
	DEFINE_OSSTATUS( errOSAInvalidID,                         ErrOSAInvalidID                        );
	DEFINE_OSSTATUS( errOSABadStorageType,                    ErrOSABadStorageType                   );
	DEFINE_OSSTATUS( errOSAScriptError,                       ErrOSAScriptError                      );
	DEFINE_OSSTATUS( errOSABadSelector,                       ErrOSABadSelector                      );
	DEFINE_OSSTATUS( errOSASourceNotAvailable,                ErrOSASourceNotAvailable               );
	DEFINE_OSSTATUS( errOSANoSuchDialect,                     ErrOSANoSuchDialect                    );
	DEFINE_OSSTATUS( errOSADataFormatObsolete,                ErrOSADataFormatObsolete               );
	DEFINE_OSSTATUS( errOSADataFormatTooNew,                  ErrOSADataFormatTooNew                 );
	DEFINE_OSSTATUS( errOSACorruptData,                       ErrOSACorruptData                      );
	DEFINE_OSSTATUS( errOSARecordingIsAlreadyOn,              ErrOSARecordingIsAlreadyOn             );
	DEFINE_OSSTATUS( errOSAComponentMismatch,                 ErrOSAComponentMismatch                );
	DEFINE_OSSTATUS( errOSACantOpenComponent,                 ErrOSACantOpenComponent                );



/* AppleEvent error definitions */
	DEFINE_OSSTATUS( errOffsetInvalid,                        ErrOffsetInvalid                       );
	DEFINE_OSSTATUS( errOffsetIsOutsideOfView,                ErrOffsetIsOutsideOfView               );
	DEFINE_OSSTATUS( errTopOfDocument,                        ErrTopOfDocument                       );
	DEFINE_OSSTATUS( errTopOfBody,                            ErrTopOfBody                           );
	DEFINE_OSSTATUS( errEndOfDocument,                        ErrEndOfDocument                       );
	DEFINE_OSSTATUS( errEndOfBody,                            ErrEndOfBody                           );


/* Drag Manager error codes */
	DEFINE_OSSTATUS( badDragRefErr,                           BadDragRefErr                          );
	DEFINE_OSSTATUS( badDragItemErr,                          BadDragItemErr                         );
	DEFINE_OSSTATUS( badDragFlavorErr,                        BadDragFlavorErr                       );
	DEFINE_OSSTATUS( duplicateFlavorErr,                      DuplicateFlavorErr                     );
	DEFINE_OSSTATUS( cantGetFlavorErr,                        CantGetFlavorErr                       );
	DEFINE_OSSTATUS( duplicateHandlerErr,                     DuplicateHandlerErr                    );
	DEFINE_OSSTATUS( handlerNotFoundErr,                      HandlerNotFoundErr                     );
	DEFINE_OSSTATUS( dragNotAcceptedErr,                      DragNotAcceptedErr                     );
	DEFINE_OSSTATUS( unsupportedForPlatformErr,               UnsupportedForPlatformErr              );
	DEFINE_OSSTATUS( noSuitableDisplaysErr,                   NoSuitableDisplaysErr                  );
	DEFINE_OSSTATUS( badImageRgnErr,                          BadImageRgnErr                         );
	DEFINE_OSSTATUS( badImageErr,                             BadImageErr                            );
	DEFINE_OSSTATUS( nonDragOriginatorErr,                    NonDragOriginatorErr                   );


/*QuickTime errors*/
	DEFINE_OSSTATUS( couldNotResolveDataRef,                  CouldNotResolveDataRef                 );
	DEFINE_OSSTATUS( badImageDescription,                     BadImageDescription                    );
	DEFINE_OSSTATUS( badPublicMovieAtom,                      BadPublicMovieAtom                     );
	DEFINE_OSSTATUS( cantFindHandler,                         CantFindHandler                        );
	DEFINE_OSSTATUS( cantOpenHandler,                         CantOpenHandler                        );
	DEFINE_OSSTATUS( badComponentType,                        BadComponentType                       );
	DEFINE_OSSTATUS( noMediaHandler,                          NoMediaHandler                         );
	DEFINE_OSSTATUS( noDataHandler,                           NoDataHandler                          );
	DEFINE_OSSTATUS( invalidMedia,                            InvalidMedia                           );
	DEFINE_OSSTATUS( invalidTrack,                            InvalidTrack                           );
	DEFINE_OSSTATUS( invalidMovie,                            InvalidMovie                           );
	DEFINE_OSSTATUS( invalidSampleTable,                      InvalidSampleTable                     );
	DEFINE_OSSTATUS( invalidDataRef,                          InvalidDataRef                         );
	DEFINE_OSSTATUS( invalidHandler,                          InvalidHandler                         );
	DEFINE_OSSTATUS( invalidDuration,                         InvalidDuration                        );
	DEFINE_OSSTATUS( invalidTime,                             InvalidTime                            );
	DEFINE_OSSTATUS( cantPutPublicMovieAtom,                  CantPutPublicMovieAtom                 );
	DEFINE_OSSTATUS( badEditList,                             BadEditList                            );
	DEFINE_OSSTATUS( mediaTypesDontMatch,                     MediaTypesDontMatch                    );
	DEFINE_OSSTATUS( progressProcAborted,                     ProgressProcAborted                    );
	DEFINE_OSSTATUS( movieToolboxUninitialized,               MovieToolboxUninitialized              );
	DEFINE_OSSTATUS( noRecordOfApp,                           NoRecordOfApp                          );
	DEFINE_OSSTATUS( wfFileNotFound,                          WFFileNotFound                         );
	DEFINE_OSSTATUS( cantCreateSingleForkFile,                CantCreateSingleForkFile               );
	DEFINE_OSSTATUS( invalidEditState,                        InvalidEditState                       );
	DEFINE_OSSTATUS( nonMatchingEditState,                    NonMatchingEditState                   );
	DEFINE_OSSTATUS( staleEditState,                          StaleEditState                         );
	DEFINE_OSSTATUS( userDataItemNotFound,                    UserDataItemNotFound                   );
	DEFINE_OSSTATUS( maxSizeToGrowTooSmall,                   MaxSizeToGrowTooSmall                  );
	DEFINE_OSSTATUS( badTrackIndex,                           BadTrackIndex                          );
	DEFINE_OSSTATUS( trackIDNotFound,                         TrackIDNotFound                        );
	DEFINE_OSSTATUS( trackNotInMovie,                         TrackNotInMovie                        );
	DEFINE_OSSTATUS( timeNotInTrack,                          TimeNotInTrack                         );
	DEFINE_OSSTATUS( timeNotInMedia,                          TimeNotInMedia                         );
	DEFINE_OSSTATUS( badEditIndex,                            BadEditIndex                           );
	DEFINE_OSSTATUS( internalQuickTimeError,                  InternalQuickTimeError                 );
	DEFINE_OSSTATUS( cantEnableTrack,                         CantEnableTrack                        );
	DEFINE_OSSTATUS( invalidRect,                             InvalidRect                            );
	DEFINE_OSSTATUS( invalidSampleNum,                        InvalidSampleNum                       );
	DEFINE_OSSTATUS( invalidChunkNum,                         InvalidChunkNum                        );
	DEFINE_OSSTATUS( invalidSampleDescIndex,                  InvalidSampleDescIndex                 );
	DEFINE_OSSTATUS( invalidChunkCache,                       InvalidChunkCache                      );
	DEFINE_OSSTATUS( invalidSampleDescription,                InvalidSampleDescription               );
	DEFINE_OSSTATUS( dataNotOpenForRead,                      DataNotOpenForRead                     );
	DEFINE_OSSTATUS( dataNotOpenForWrite,                     DataNotOpenForWrite                    );
	DEFINE_OSSTATUS( dataAlreadyOpenForWrite,                 DataAlreadyOpenForWrite                );
	DEFINE_OSSTATUS( dataAlreadyClosed,                       DataAlreadyClosed                      );
	DEFINE_OSSTATUS( endOfDataReached,                        EndOfDataReached                       );
	DEFINE_OSSTATUS( dataNoDataRef,                           DataNoDataRef                          );
	DEFINE_OSSTATUS( noMovieFound,                            NoMovieFound                           );
	DEFINE_OSSTATUS( invalidDataRefContainer,                 InvalidDataRefContainer                );
	DEFINE_OSSTATUS( badDataRefIndex,                         BadDataRefIndex                        );
	DEFINE_OSSTATUS( noDefaultDataRef,                        NoDefaultDataRef                       );
	DEFINE_OSSTATUS( couldNotUseAnExistingSample,             CouldNotUseAnExistingSample            );
	DEFINE_OSSTATUS( featureUnsupported,                      FeatureUnsupported                     );
	DEFINE_OSSTATUS( noVideoTrackInMovieErr,                  NoVideoTrackInMovieErr                 );
	DEFINE_OSSTATUS( noSoundTrackInMovieErr,                  NoSoundTrackInMovieErr                 );
	DEFINE_OSSTATUS( soundSupportNotAvailableErr,             SoundSupportNotAvailableErr            );
	DEFINE_OSSTATUS( unsupportedAuxiliaryImportData,          UnsupportedAuxiliaryImportData         );
	DEFINE_OSSTATUS( auxiliaryExportDataUnavailable,          AuxiliaryExportDataUnavailable         );
	DEFINE_OSSTATUS( samplesAlreadyInMediaErr,                SamplesAlreadyInMediaErr               );
	DEFINE_OSSTATUS( noSourceTreeFoundErr,                    NoSourceTreeFoundErr                   );
	DEFINE_OSSTATUS( sourceNotFoundErr,                       SourceNotFoundErr                      );
	DEFINE_OSSTATUS( movieTextNotFoundErr,                    MovieTextNotFoundErr                   );
	DEFINE_OSSTATUS( missingRequiredParameterErr,             MissingRequiredParameterErr            );
	DEFINE_OSSTATUS( invalidSpriteWorldPropertyErr,           InvalidSpriteWorldPropertyErr          );
	DEFINE_OSSTATUS( invalidSpritePropertyErr,                InvalidSpritePropertyErr               );
	DEFINE_OSSTATUS( gWorldsNotSameDepthAndSizeErr,           GWorldsNotSameDepthAndSizeErr          );
	DEFINE_OSSTATUS( invalidSpriteIndexErr,                   InvalidSpriteIndexErr                  );
	DEFINE_OSSTATUS( invalidImageIndexErr,                    InvalidImageIndexErr                   );
	DEFINE_OSSTATUS( invalidSpriteIDErr,                      InvalidSpriteIDErr                     );

	DEFINE_OSSTATUS( internalComponentErr,                    InternalComponentErr                   );
	DEFINE_OSSTATUS( notImplementedMusicOSErr,                NotImplementedMusicOSErr               );
	DEFINE_OSSTATUS( cantSendToSynthesizerOSErr,              CantSendToSynthesizerOSErr             );
	DEFINE_OSSTATUS( cantReceiveFromSynthesizerOSErr,         CantReceiveFromSynthesizerOSErr        );
	DEFINE_OSSTATUS( illegalVoiceAllocationOSErr,             IllegalVoiceAllocationOSErr            );
	DEFINE_OSSTATUS( illegalPartOSErr,                        IllegalPartOSErr                       );
	DEFINE_OSSTATUS( illegalChannelOSErr,                     IllegalChannelOSErr                    );
	DEFINE_OSSTATUS( illegalKnobOSErr,                        IllegalKnobOSErr                       );
	DEFINE_OSSTATUS( illegalKnobValueOSErr,                   IllegalKnobValueOSErr                  );
	DEFINE_OSSTATUS( illegalInstrumentOSErr,                  IllegalInstrumentOSErr                 );
	DEFINE_OSSTATUS( illegalControllerOSErr,                  IllegalControllerOSErr                 );
	DEFINE_OSSTATUS( midiManagerAbsentOSErr,                  MIDIManagerAbsentOSErr                 );
	DEFINE_OSSTATUS( synthesizerNotRespondingOSErr,           SynthesizerNotRespondingOSErr          );
	DEFINE_OSSTATUS( synthesizerOSErr,                        SynthesizerOSErr                       );
	DEFINE_OSSTATUS( illegalNoteChannelOSErr,                 IllegalNoteChannelOSErr                );
	DEFINE_OSSTATUS( noteChannelNotAllocatedOSErr,            NoteChannelNotAllocatedOSErr           );
	DEFINE_OSSTATUS( tunePlayerFullOSErr,                     TunePlayerFullOSErr                    );
	DEFINE_OSSTATUS( tuneParseOSErr,                          TuneParseOSErr                         );
	DEFINE_OSSTATUS( noExportProcAvailableErr,                NoExportProcAvailableErr               );
	DEFINE_OSSTATUS( videoOutputInUseErr,                     VideoOutputInUseErr                    );

	DEFINE_OSSTATUS( componentDllLoadErr,                     ComponentDllLoadErr                    );
	DEFINE_OSSTATUS( componentDllEntryNotFoundErr,            ComponentDllEntryNotFoundErr           );
	DEFINE_OSSTATUS( qtmlDllLoadErr,                          QTMLDllLoadErr                         );
	DEFINE_OSSTATUS( qtmlDllEntryNotFoundErr,                 QTMLDllEntryNotFoundErr                );
	DEFINE_OSSTATUS( qtmlUninitialized,                       QTMLUninitialized                      );
	DEFINE_OSSTATUS( unsupportedOSErr,                        UnsupportedOSErr                       );
	DEFINE_OSSTATUS( unsupportedProcessorErr,                 UnsupportedProcessorErr                );

	DEFINE_OSSTATUS( cannotFindAtomErr,                       CannotFindAtomErr                      );
	DEFINE_OSSTATUS( notLeafAtomErr,                          NotLeafAtomErr                         );
	DEFINE_OSSTATUS( atomsNotOfSameTypeErr,                   AtomsNotOfSameTypeErr                  );
	DEFINE_OSSTATUS( atomIndexInvalidErr,                     AtomIndexInvalidErr                    );
	DEFINE_OSSTATUS( duplicateAtomTypeAndIDErr,               DuplicateAtomTypeAndIDErr              );
	DEFINE_OSSTATUS( invalidAtomErr,                          InvalidAtomErr                         );
	DEFINE_OSSTATUS( invalidAtomContainerErr,                 InvalidAtomContainerErr                );
	DEFINE_OSSTATUS( invalidAtomTypeErr,                      InvalidAtomTypeErr                     );
	DEFINE_OSSTATUS( cannotBeLeafAtomErr,                     CannotBeLeafAtomErr                    );
	DEFINE_OSSTATUS( pathTooLongErr,                          PathTooLongErr                         );
	DEFINE_OSSTATUS( emptyPathErr,                            EmptyPathErr                           );
	DEFINE_OSSTATUS( noPathMappingErr,                        NoPathMappingErr                       );
	DEFINE_OSSTATUS( pathNotVerifiedErr,                      PathNotVerifiedErr                     );
	DEFINE_OSSTATUS( unknownFormatErr,                        UnknownFormatErr                       );
	DEFINE_OSSTATUS( wackBadFileErr,                          WACKBadFileErr                         );
	DEFINE_OSSTATUS( wackForkNotFoundErr,                     WACKForkNotFoundErr                    );
	DEFINE_OSSTATUS( wackBadMetaDataErr,                      WACKBadMetaDataErr                     );
	DEFINE_OSSTATUS( qfcbNotFoundErr,                         QFCBNotFoundErr                        );
	DEFINE_OSSTATUS( qfcbNotCreatedErr,                       QFCBNotCreatedErr                      );
	DEFINE_OSSTATUS( ::AAPNotCreatedErr,                      AAPNotCreatedErr                       );
	DEFINE_OSSTATUS( ::AAPNotFoundErr,                        AAPNotFoundErr                         );
	DEFINE_OSSTATUS( ::ASDBadHeaderErr,                       ASDBadHeaderErr                        );
	DEFINE_OSSTATUS( ::ASDBadForkErr,                         ASDBadForkErr                          );
	DEFINE_OSSTATUS( ::ASDEntryNotFoundErr,                   ASDEntryNotFoundErr                    );
	DEFINE_OSSTATUS( fileOffsetTooBigErr,                     FileOffsetTooBigErr                    );
	DEFINE_OSSTATUS( notAllowedToSaveMovieErr,                NotAllowedToSaveMovieErr               );
	DEFINE_OSSTATUS( qtNetworkAlreadyAllocatedErr,            QTNetworkAlreadyAllocatedErr           );
	DEFINE_OSSTATUS( urlDataHHTTPProtocolErr,                 URLDataHHTTPProtocolErr                );
	DEFINE_OSSTATUS( urlDataHHTTPNoNetDriverErr,              URLDataHHTTPNoNetDriverErr             );
	DEFINE_OSSTATUS( urlDataHHTTPURLErr,                      URLDataHHTTPURLErr                     );
	DEFINE_OSSTATUS( urlDataHHTTPRedirectErr,                 URLDataHHTTPRedirectErr                );
	DEFINE_OSSTATUS( urlDataHFTPProtocolErr,                  URLDataHFTPProtocolErr                 );
	DEFINE_OSSTATUS( urlDataHFTPShutdownErr,                  URLDataHFTPShutdownErr                 );
	DEFINE_OSSTATUS( urlDataHFTPBadUserErr,                   URLDataHFTPBadUserErr                  );
	DEFINE_OSSTATUS( urlDataHFTPBadPasswordErr,               URLDataHFTPBadPasswordErr              );
	DEFINE_OSSTATUS( urlDataHFTPServerErr,                    URLDataHFTPServerErr                   );
	DEFINE_OSSTATUS( urlDataHFTPDataConnectionErr,            URLDataHFTPDataConnectionErr           );
	DEFINE_OSSTATUS( urlDataHFTPNoDirectoryErr,               URLDataHFTPNoDirectoryErr              );
	DEFINE_OSSTATUS( urlDataHFTPQuotaErr,                     URLDataHFTPQuotaErr                    );
	DEFINE_OSSTATUS( urlDataHFTPPermissionsErr,               URLDataHFTPPermissionsErr              );
	DEFINE_OSSTATUS( urlDataHFTPFilenameErr,                  URLDataHFTPFilenameErr                 );
	DEFINE_OSSTATUS( urlDataHFTPNoNetDriverErr,               URLDataHFTPNoNetDriverErr              );
	DEFINE_OSSTATUS( urlDataHFTPBadNameListErr,               URLDataHFTPBadNameListErr              );
	DEFINE_OSSTATUS( urlDataHFTPNeedPasswordErr,              URLDataHFTPNeedPasswordErr             );
	DEFINE_OSSTATUS( urlDataHFTPNoPasswordErr,                URLDataHFTPNoPasswordErr               );
	DEFINE_OSSTATUS( urlDataHFTPServerDisconnectedErr,        URLDataHFTPServerDisconnectedErr       );
	DEFINE_OSSTATUS( urlDataHFTPURLErr,                       URLDataHFTPURLErr                      );
	DEFINE_OSSTATUS( notEnoughDataErr,                        NotEnoughDataErr                       );
	DEFINE_OSSTATUS( qtActionNotHandledErr,                   QTActionNotHandledErr                  );
	DEFINE_OSSTATUS( qtXMLParseErr,                           QTXMLParseErr                          );
	DEFINE_OSSTATUS( qtXMLApplicationErr,                     QTXMLApplicationErr                    );


	DEFINE_OSSTATUS( digiUnimpErr,                            DigiUnimpErr                           );
	DEFINE_OSSTATUS( qtParamErr,                              QTParamErr                             );
	DEFINE_OSSTATUS( matrixErr,                               MatrixErr                              );
	DEFINE_OSSTATUS( notExactMatrixErr,                       NotExactMatrixErr                      );
	DEFINE_OSSTATUS( noMoreKeyColorsErr,                      NoMoreKeyColorsErr                     );
	DEFINE_OSSTATUS( notExactSizeErr,                         NotExactSizeErr                        );
	DEFINE_OSSTATUS( badDepthErr,                             BadDepthErr                            );
	DEFINE_OSSTATUS( noDMAErr,                                NoDMAErr                               );
	DEFINE_OSSTATUS( badCallOrderErr,                         BadCallOrderErr                        );


/*  Kernel Error Codes  */
	DEFINE_OSSTATUS( kernelIncompleteErr,                     KernelIncompleteErr                    );
	DEFINE_OSSTATUS( kernelCanceledErr,                       KernelCanceledErr                      );
	DEFINE_OSSTATUS( kernelOptionsErr,                        KernelOptionsErr                       );
	DEFINE_OSSTATUS( kernelPrivilegeErr,                      KernelPrivilegeErr                     );
	DEFINE_OSSTATUS( kernelUnsupportedErr,                    KernelUnsupportedErr                   );
	DEFINE_OSSTATUS( kernelObjectExistsErr,                   KernelObjectExistsErr                  );
	DEFINE_OSSTATUS( kernelWritePermissionErr,                KernelWritePermissionErr               );
	DEFINE_OSSTATUS( kernelReadPermissionErr,                 KernelReadPermissionErr                );
	DEFINE_OSSTATUS( kernelExecutePermissionErr,              KernelExecutePermissionErr             );
	DEFINE_OSSTATUS( kernelDeletePermissionErr,               KernelDeletePermissionErr              );
	DEFINE_OSSTATUS( kernelExecutionLevelErr,                 KernelExecutionLevelErr                );
	DEFINE_OSSTATUS( kernelAttributeErr,                      KernelAttributeErr                     );
	DEFINE_OSSTATUS( kernelAsyncSendLimitErr,                 KernelAsyncSendLimitErr                );
	DEFINE_OSSTATUS( kernelAsyncReceiveLimitErr,              KernelAsyncReceiveLimitErr             );
	DEFINE_OSSTATUS( kernelTimeoutErr,                        KernelTimeoutErr                       );
	DEFINE_OSSTATUS( kernelInUseErr,                          KernelInUseErr                         );
	DEFINE_OSSTATUS( kernelTerminatedErr,                     KernelTerminatedErr                    );
	DEFINE_OSSTATUS( kernelExceptionErr,                      KernelExceptionErr                     );
	DEFINE_OSSTATUS( kernelIDErr,                             KernelIDErr                            );
	DEFINE_OSSTATUS( kernelAlreadyFreeErr,                    KernelAlreadyFreeErr                   );
	DEFINE_OSSTATUS( kernelReturnValueErr,                    KernelReturnValueErr                   );
	DEFINE_OSSTATUS( kernelUnrecoverableErr,                  KernelUnrecoverableErr                 );



/* Text Services Mgr error codes */
	DEFINE_OSSTATUS( tsmComponentNoErr,                       TSMComponentNoErr                      );
	DEFINE_OSSTATUS( tsmUnsupScriptLanguageErr,               TSMUnsupScriptLanguageErr              );
	DEFINE_OSSTATUS( tsmInputMethodNotFoundErr,               TSMInputMethodNotFoundErr              );
	DEFINE_OSSTATUS( tsmNotAnAppErr,                          TSMNotAnAppErr                         );
	DEFINE_OSSTATUS( tsmAlreadyRegisteredErr,                 TSMAlreadyRegisteredErr                );
	DEFINE_OSSTATUS( tsmNeverRegisteredErr,                   TSMNeverRegisteredErr                  );
	DEFINE_OSSTATUS( tsmInvalidDocIDErr,                      TSMInvalidDocIDErr                     );
	DEFINE_OSSTATUS( tsmTSMDocBusyErr,                        TSMTSMDocBusyErr                       );
	DEFINE_OSSTATUS( tsmDocNotActiveErr,                      TSMDocNotActiveErr                     );
	DEFINE_OSSTATUS( tsmNoOpenTSErr,                          TSMNoOpenTSErr                         );
	DEFINE_OSSTATUS( tsmCantOpenComponentErr,                 TSMCantOpenComponentErr                );
	DEFINE_OSSTATUS( tsmTextServiceNotFoundErr,               TSMTextServiceNotFoundErr              );
	DEFINE_OSSTATUS( tsmDocumentOpenErr,                      TSMDocumentOpenErr                     );
	DEFINE_OSSTATUS( tsmUseInputWindowErr,                    TSMUseInputWindowErr                   );
	DEFINE_OSSTATUS( tsmTSHasNoMenuErr,                       TSMTSHasNoMenuErr                      );
	DEFINE_OSSTATUS( tsmTSNotOpenErr,                         TSMTSNotOpenErr                        );
	DEFINE_OSSTATUS( tsmComponentAlreadyOpenErr,              TSMComponentAlreadyOpenErr             );
	DEFINE_OSSTATUS( tsmInputMethodIsOldErr,                  TSMInputMethodIsOldErr                 );
	DEFINE_OSSTATUS( tsmScriptHasNoIMErr,                     TSMScriptHasNoIMErr                    );
	DEFINE_OSSTATUS( tsmUnsupportedTypeErr,                   TSMUnsupportedTypeErr                  );
	DEFINE_OSSTATUS( tsmUnknownErr,                           TSMUnknownErr                          );
	DEFINE_OSSTATUS( tsmInvalidContext,                       TSMInvalidContext                      );
	DEFINE_OSSTATUS( tsmNoHandler,                            TSMNoHandler                           );
	DEFINE_OSSTATUS( tsmNoMoreTokens,                         TSMNoMoreTokens                        );
	DEFINE_OSSTATUS( tsmNoStem,                               TSMNoStem                              );
	DEFINE_OSSTATUS( tsmDefaultIsNotInputMethodErr,           TSMDefaultIsNotInputMethodErr          );


/* Mixed Mode error codes */
	DEFINE_OSSTATUS( mmInternalError,                         MMInternalError                        );

/* NameRegistry error codes */
	DEFINE_OSSTATUS( nrLockedErr,                             NRLockedErr                            );
	DEFINE_OSSTATUS( nrNotEnoughMemoryErr,                    NRNotEnoughMemoryErr                   );
	DEFINE_OSSTATUS( nrInvalidNodeErr,                        NRInvalidNodeErr                       );
	DEFINE_OSSTATUS( nrNotFoundErr,                           NRNotFoundErr                          );
	DEFINE_OSSTATUS( nrNotCreatedErr,                         NRNotCreatedErr                        );
	DEFINE_OSSTATUS( nrNameErr,                               NRNameErr                              );
	DEFINE_OSSTATUS( nrNotSlotDeviceErr,                      NRNotSlotDeviceErr                     );
	DEFINE_OSSTATUS( nrDataTruncatedErr,                      NRDataTruncatedErr                     );
	DEFINE_OSSTATUS( nrPowerErr,                              NRPowerErr                             );
	DEFINE_OSSTATUS( nrPowerSwitchAbortErr,                   NRPowerSwitchAbortErr                  );
	DEFINE_OSSTATUS( nrTypeMismatchErr,                       NRTypeMismatchErr                      );
	DEFINE_OSSTATUS( nrNotModifiedErr,                        NRNotModifiedErr                       );
	DEFINE_OSSTATUS( nrOverrunErr,                            NROverrunErr                           );
	DEFINE_OSSTATUS( nrResultCodeBase,                        NRResultCodeBase                       );
	DEFINE_OSSTATUS( nrPathNotFound,                          NRPathNotFound                         );
	DEFINE_OSSTATUS( nrPathBufferTooSmall,                    NRPathBufferTooSmall                   );
	DEFINE_OSSTATUS( nrInvalidEntryIterationOp,               NRInvalidEntryIterationOp              );
	DEFINE_OSSTATUS( nrPropertyAlreadyExists,                 NRPropertyAlreadyExists                );
	DEFINE_OSSTATUS( nrIterationDone,                         NRIterationDone                        );
	DEFINE_OSSTATUS( nrExitedIteratorScope,                   NRExitedIteratorScope                  );
	DEFINE_OSSTATUS( nrTransactionAborted,                    NRTransactionAborted                   );
	DEFINE_OSSTATUS( nrCallNotSupported,                      NRCallNotSupported                     );

/* Icon Services error codes */
	DEFINE_OSSTATUS( invalidIconRefErr,                       InvalidIconRefErr                      );
	DEFINE_OSSTATUS( noSuchIconErr,                           NoSuchIconErr                          );
	DEFINE_OSSTATUS( noIconDataAvailableErr,                  NoIconDataAvailableErr                 );


/* Dynamic AppleScript errors */
	DEFINE_OSSTATUS( errOSACantCoerce,                        ErrOSACantCoerce                       );
	DEFINE_OSSTATUS( errOSACantAccess,                        ErrOSACantAccess                       );
	DEFINE_OSSTATUS( errOSACantAssign,                        ErrOSACantAssign                       );
	DEFINE_OSSTATUS( errOSAGeneralError,                      ErrOSAGeneralError                     );
	DEFINE_OSSTATUS( errOSADivideByZero,                      ErrOSADivideByZero                     );
	DEFINE_OSSTATUS( errOSANumericOverflow,                   ErrOSANumericOverflow                  );
	DEFINE_OSSTATUS( errOSACantLaunch,                        ErrOSACantLaunch                       );
	DEFINE_OSSTATUS( errOSAAppNotHighLevelEventAware,         ErrOSAAppNotHighLevelEventAware        );
	DEFINE_OSSTATUS( errOSACorruptTerminology,                ErrOSACorruptTerminology               );
	DEFINE_OSSTATUS( errOSAStackOverflow,                     ErrOSAStackOverflow                    );
	DEFINE_OSSTATUS( errOSAInternalTableOverflow,             ErrOSAInternalTableOverflow            );
	DEFINE_OSSTATUS( errOSADataBlockTooLarge,                 ErrOSADataBlockTooLarge                );
	DEFINE_OSSTATUS( errOSACantGetTerminology,                ErrOSACantGetTerminology               );
	DEFINE_OSSTATUS( errOSACantCreate,                        ErrOSACantCreate                       );

/* Static AppleScript errors */
	DEFINE_OSSTATUS( errOSATypeError,                         ErrOSATypeError                        );
	DEFINE_OSSTATUS( ::OSAMessageNotUnderstood,               OSAMessageNotUnderstood                );
	DEFINE_OSSTATUS( ::OSAUndefinedHandler,                   OSAUndefinedHandler                    );
	DEFINE_OSSTATUS( ::OSAIllegalAccess,                      OSAIllegalAccess                       );
	DEFINE_OSSTATUS( ::OSAIllegalIndex,                       OSAIllegalIndex                        );
	DEFINE_OSSTATUS( ::OSAIllegalRange,                       OSAIllegalRange                        );
	DEFINE_OSSTATUS( ::OSAIllegalAssign,                      OSAIllegalAssign                       );
	DEFINE_OSSTATUS( ::OSASyntaxError,                        OSASyntaxError                         );
	DEFINE_OSSTATUS( ::OSASyntaxTypeError,                    OSASyntaxTypeError                     );
	DEFINE_OSSTATUS( ::OSATokenTooLong,                       OSATokenTooLong                        );
	DEFINE_OSSTATUS( ::OSAMissingParameter,                   OSAMissingParameter                    );
	DEFINE_OSSTATUS( ::OSAParameterMismatch,                  OSAParameterMismatch                   );
	DEFINE_OSSTATUS( ::OSADuplicateParameter,                 OSADuplicateParameter                  );
	DEFINE_OSSTATUS( ::OSADuplicateProperty,                  OSADuplicateProperty                   );
	DEFINE_OSSTATUS( ::OSADuplicateHandler,                   OSADuplicateHandler                    );
	DEFINE_OSSTATUS( ::OSAUndefinedVariable,                  OSAUndefinedVariable                   );
	DEFINE_OSSTATUS( ::OSAInconsistentDeclarations,           OSAInconsistentDeclarations            );
	DEFINE_OSSTATUS( ::OSAControlFlowError,                   OSAControlFlowError                    );

/* Runtime errors: */
	DEFINE_OSSTATUS( errASCantConsiderAndIgnore,              ErrASCantConsiderAndIgnore             );
	DEFINE_OSSTATUS( errASCantCompareMoreThan32k,             ErrASCantCompareMoreThan32k            );
	DEFINE_OSSTATUS( errASTerminologyNestingTooDeep,          ErrASTerminologyNestingTooDeep         );
	DEFINE_OSSTATUS( errASIllegalFormalParameter,             ErrASIllegalFormalParameter            );
	DEFINE_OSSTATUS( errASParameterNotForEvent,               ErrASParameterNotForEvent              );
	DEFINE_OSSTATUS( errASNoResultReturned,                   ErrASNoResultReturned                  );
	DEFINE_OSSTATUS( errASInconsistentNames,                  ErrASInconsistentNames                 );


/* Code Fragment Manager errors */
	DEFINE_OSSTATUS( cfragFirstErrCode,                       CFragFirstErrCode                      );
	DEFINE_OSSTATUS( cfragContextIDErr,                       CFragContextIDErr                      );
	DEFINE_OSSTATUS( cfragConnectionIDErr,                    CFragConnectionIDErr                   );
	DEFINE_OSSTATUS( cfragNoSymbolErr,                        CFragNoSymbolErr                       );
	DEFINE_OSSTATUS( cfragNoSectionErr,                       CFragNoSectionErr                      );
	DEFINE_OSSTATUS( cfragNoLibraryErr,                       CFragNoLibraryErr                      );
	DEFINE_OSSTATUS( cfragDupRegistrationErr,                 CFragDupRegistrationErr                );
	DEFINE_OSSTATUS( cfragFragmentFormatErr,                  CFragFragmentFormatErr                 );
	DEFINE_OSSTATUS( cfragUnresolvedErr,                      CFragUnresolvedErr                     );
	DEFINE_OSSTATUS( cfragNoPositionErr,                      CFragNoPositionErr                     );
	DEFINE_OSSTATUS( cfragNoPrivateMemErr,                    CFragNoPrivateMemErr                   );
	DEFINE_OSSTATUS( cfragNoClientMemErr,                     CFragNoClientMemErr                    );
	DEFINE_OSSTATUS( cfragNoIDsErr,                           CFragNoIDsErr                          );
	DEFINE_OSSTATUS( cfragInitOrderErr,                       CFragInitOrderErr                      );
	DEFINE_OSSTATUS( cfragImportTooOldErr,                    CFragImportTooOldErr                   );
	DEFINE_OSSTATUS( cfragImportTooNewErr,                    CFragImportTooNewErr                   );
	DEFINE_OSSTATUS( cfragInitLoopErr,                        CFragInitLoopErr                       );
	DEFINE_OSSTATUS( cfragInitAtBootErr,                      CFragInitAtBootErr                     );
	DEFINE_OSSTATUS( cfragLibConnErr,                         CFragLibConnErr                        );
	DEFINE_OSSTATUS( cfragCFMStartupErr,                      CFragCFMStartupErr                     );
	DEFINE_OSSTATUS( cfragCFMInternalErr,                     CFragCFMInternalErr                    );
	DEFINE_OSSTATUS( cfragFragmentCorruptErr,                 CFragFragmentCorruptErr                );
	DEFINE_OSSTATUS( cfragInitFunctionErr,                    CFragInitFunctionErr                   );
	DEFINE_OSSTATUS( cfragNoApplicationErr,                   CFragNoApplicationErr                  );
	DEFINE_OSSTATUS( cfragArchitectureErr,                    CFragArchitectureErr                   );
	DEFINE_OSSTATUS( cfragFragmentUsageErr,                   CFragFragmentUsageErr                  );
	DEFINE_OSSTATUS( cfragFileSizeErr,                        CFragFileSizeErr                       );
	DEFINE_OSSTATUS( cfragNotClosureErr,                      CFragNotClosureErr                     );
	DEFINE_OSSTATUS( cfragNoRegistrationErr,                  CFragNoRegistrationErr                 );
	DEFINE_OSSTATUS( cfragContainerIDErr,                     CFragContainerIDErr                    );
	DEFINE_OSSTATUS( cfragClosureIDErr,                       CFragClosureIDErr                      );
	DEFINE_OSSTATUS( cfragAbortClosureErr,                    CFragAbortClosureErr                   );
	DEFINE_OSSTATUS( cfragOutputLengthErr,                    CFragOutputLengthErr                   );
	DEFINE_OSSTATUS( cfragLastErrCode,                        CFragLastErrCode                       );

/*Component Manager & component errors*/
	DEFINE_OSSTATUS( invalidComponentID,                      InvalidComponentID                     );
	DEFINE_OSSTATUS( validInstancesExist,                     ValidInstancesExist                    );
	DEFINE_OSSTATUS( componentNotCaptured,                    ComponentNotCaptured                   );
	DEFINE_OSSTATUS( componentDontRegister,                   ComponentDontRegister                  );
	DEFINE_OSSTATUS( unresolvedComponentDLLErr,               UnresolvedComponentDLLErr              );
	DEFINE_OSSTATUS( retryComponentRegistrationErr,           RetryComponentRegistrationErr          );

/*Translation manager & Translation components*/
	DEFINE_OSSTATUS( invalidTranslationPathErr,               InvalidTranslationPathErr              );
	DEFINE_OSSTATUS( couldNotParseSourceFileErr,              CouldNotParseSourceFileErr             );
	DEFINE_OSSTATUS( noTranslationPathErr,                    NoTranslationPathErr                   );
	DEFINE_OSSTATUS( badTranslationSpecErr,                   BadTranslationSpecErr                  );
	DEFINE_OSSTATUS( noPrefAppErr,                            NoPrefAppErr                           );

	DEFINE_OSSTATUS( buf2SmallErr,                            Buf2SmallErr                           );
	DEFINE_OSSTATUS( noMPPErr,                                NoMPPErr                               );
	DEFINE_OSSTATUS( ckSumErr,                                CKSumErr                               );
	DEFINE_OSSTATUS( extractErr,                              ExtractErr                             );
	DEFINE_OSSTATUS( readQErr,                                ReadQErr                               );
	DEFINE_OSSTATUS( atpLenErr,                               ATPLenErr                              );
	DEFINE_OSSTATUS( atpBadRsp,                               ATPBadRsp                              );
	DEFINE_OSSTATUS( recNotFnd,                               RecNotFnd                              );
	DEFINE_OSSTATUS( sktClosedErr,                            SktClosedErr                           );


/* OpenTransport errors*/
	DEFINE_OSSTATUS( kOTNoError,                              OTNoError                              );
	DEFINE_OSSTATUS( kOTOutOfMemoryErr,                       OTOutOfMemoryErr                       );
	DEFINE_OSSTATUS( kOTNotFoundErr,                          OTNotFoundErr                          );
	DEFINE_OSSTATUS( kOTDuplicateFoundErr,                    OTDuplicateFoundErr                    );
	DEFINE_OSSTATUS( kOTBadAddressErr,                        OTBadAddressErr                        );
	DEFINE_OSSTATUS( kOTBadOptionErr,                         OTBadOptionErr                         );
	DEFINE_OSSTATUS( kOTAccessErr,                            OTAccessErr                            );
	DEFINE_OSSTATUS( kOTBadReferenceErr,                      OTBadReferenceErr                      );
	DEFINE_OSSTATUS( kOTNoAddressErr,                         OTNoAddressErr                         );
	DEFINE_OSSTATUS( kOTOutStateErr,                          OTOutStateErr                          );
	DEFINE_OSSTATUS( kOTBadSequenceErr,                       OTBadSequenceErr                       );
	DEFINE_OSSTATUS( kOTSysErrorErr,                          OTSysErrorErr                          );
	DEFINE_OSSTATUS( kOTLookErr,                              OTLookErr                              );
	DEFINE_OSSTATUS( kOTBadDataErr,                           OTBadDataErr                           );
	DEFINE_OSSTATUS( kOTBufferOverflowErr,                    OTBufferOverflowErr                    );
	DEFINE_OSSTATUS( kOTFlowErr,                              OTFlowErr                              );
	DEFINE_OSSTATUS( kOTNoDataErr,                            OTNoDataErr                            );
	DEFINE_OSSTATUS( kOTNoDisconnectErr,                      OTNoDisconnectErr                      );
	DEFINE_OSSTATUS( kOTNoUDErrErr,                           OTNoUDErrErr                           );
	DEFINE_OSSTATUS( kOTBadFlagErr,                           OTBadFlagErr                           );
	DEFINE_OSSTATUS( kOTNoReleaseErr,                         OTNoReleaseErr                         );
	DEFINE_OSSTATUS( kOTNotSupportedErr,                      OTNotSupportedErr                      );
	DEFINE_OSSTATUS( kOTStateChangeErr,                       OTStateChangeErr                       );
	DEFINE_OSSTATUS( kOTNoStructureTypeErr,                   OTNoStructureTypeErr                   );
	DEFINE_OSSTATUS( kOTBadNameErr,                           OTBadNameErr                           );
	DEFINE_OSSTATUS( kOTBadQLenErr,                           OTBadQLenErr                           );
	DEFINE_OSSTATUS( kOTAddressBusyErr,                       OTAddressBusyErr                       );
	DEFINE_OSSTATUS( kOTIndOutErr,                            OTIndOutErr                            );
	DEFINE_OSSTATUS( kOTProviderMismatchErr,                  OTProviderMismatchErr                  );
	DEFINE_OSSTATUS( kOTResQLenErr,                           OTResQLenErr                           );
	DEFINE_OSSTATUS( kOTResAddressErr,                        OTResAddressErr                        );
	DEFINE_OSSTATUS( kOTQFullErr,                             OTQFullErr                             );
	DEFINE_OSSTATUS( kOTProtocolErr,                          OTProtocolErr                          );
	DEFINE_OSSTATUS( kOTBadSyncErr,                           OTBadSyncErr                           );
	DEFINE_OSSTATUS( kOTCanceledErr,                          OTCanceledErr                          );
	DEFINE_OSSTATUS( kEPERMErr,                               EPERMErr                               );
	DEFINE_OSSTATUS( kENOENTErr,                              ENOENTErr                              );
	DEFINE_OSSTATUS( kENORSRCErr,                             ENORSRCErr                             );
	DEFINE_OSSTATUS( kEINTRErr,                               EINTRErr                               );
	DEFINE_OSSTATUS( kEIOErr,                                 EIOErr                                 );
	DEFINE_OSSTATUS( kENXIOErr,                               ENXIOErr                               );
	DEFINE_OSSTATUS( kEBADFErr,                               EBADFErr                               );
	DEFINE_OSSTATUS( kEAGAINErr,                              EAGAINErr                              );
	DEFINE_OSSTATUS( kENOMEMErr,                              ENOMEMErr                              );
	DEFINE_OSSTATUS( kEACCESErr,                              EACCESErr                              );
	DEFINE_OSSTATUS( kEFAULTErr,                              EFAULTErr                              );
	DEFINE_OSSTATUS( kEBUSYErr,                               EBUSYErr                               );
	DEFINE_OSSTATUS( kEEXISTErr,                              EEXISTErr                              );
	DEFINE_OSSTATUS( kENODEVErr,                              ENODEVErr                              );
	DEFINE_OSSTATUS( kEINVALErr,                              EINVALErr                              );
	DEFINE_OSSTATUS( kENOTTYErr,                              ENOTTYErr                              );
	DEFINE_OSSTATUS( kEPIPEErr,                               EPIPEErr                               );
	DEFINE_OSSTATUS( kERANGEErr,                              ERANGEErr                              );
	DEFINE_OSSTATUS( kEWOULDBLOCKErr,                         EWOULDBLOCKErr                         );
	DEFINE_OSSTATUS( kEDEADLKErr,                             EDEADLKErr                             );
	DEFINE_OSSTATUS( kEALREADYErr,                            EALREADYErr                            );
	DEFINE_OSSTATUS( kENOTSOCKErr,                            ENOTSOCKErr                            );
	DEFINE_OSSTATUS( kEDESTADDRREQErr,                        EDESTADDRREQErr                        );
	DEFINE_OSSTATUS( kEMSGSIZEErr,                            EMSGSIZEErr                            );
	DEFINE_OSSTATUS( kEPROTOTYPEErr,                          EPROTOTYPEErr                          );
	DEFINE_OSSTATUS( kENOPROTOOPTErr,                         ENOPROTOOPTErr                         );
	DEFINE_OSSTATUS( kEPROTONOSUPPORTErr,                     EPROTONOSUPPORTErr                     );
	DEFINE_OSSTATUS( kESOCKTNOSUPPORTErr,                     ESOCKTNOSUPPORTErr                     );
	DEFINE_OSSTATUS( kEOPNOTSUPPErr,                          EOPNOTSUPPErr                          );
	DEFINE_OSSTATUS( kEADDRINUSEErr,                          EADDRINUSEErr                          );
	DEFINE_OSSTATUS( kEADDRNOTAVAILErr,                       EADDRNOTAVAILErr                       );
	DEFINE_OSSTATUS( kENETDOWNErr,                            ENETDOWNErr                            );
	DEFINE_OSSTATUS( kENETUNREACHErr,                         ENETUNREACHErr                         );
	DEFINE_OSSTATUS( kENETRESETErr,                           ENETRESETErr                           );
	DEFINE_OSSTATUS( kECONNABORTEDErr,                        ECONNABORTEDErr                        );
	DEFINE_OSSTATUS( kECONNRESETErr,                          ECONNRESETErr                          );
	DEFINE_OSSTATUS( kENOBUFSErr,                             ENOBUFSErr                             );
	DEFINE_OSSTATUS( kEISCONNErr,                             EISCONNErr                             );
	DEFINE_OSSTATUS( kENOTCONNErr,                            ENOTCONNErr                            );
	DEFINE_OSSTATUS( kESHUTDOWNErr,                           ESHUTDOWNErr                           );
	DEFINE_OSSTATUS( kETOOMANYREFSErr,                        ETOOMANYREFSErr                        );
	DEFINE_OSSTATUS( kETIMEDOUTErr,                           ETIMEDOUTErr                           );
	DEFINE_OSSTATUS( kECONNREFUSEDErr,                        ECONNREFUSEDErr                        );
	DEFINE_OSSTATUS( kEHOSTDOWNErr,                           EHOSTDOWNErr                           );
	DEFINE_OSSTATUS( kEHOSTUNREACHErr,                        EHOSTUNREACHErr                        );
	DEFINE_OSSTATUS( kEPROTOErr,                              EPROTOErr                              );
	DEFINE_OSSTATUS( kETIMEErr,                               ETIMEErr                               );
	DEFINE_OSSTATUS( kENOSRErr,                               ENOSRErr                               );
	DEFINE_OSSTATUS( kEBADMSGErr,                             EBADMSGErr                             );
	DEFINE_OSSTATUS( kECANCELErr,                             ECANCELErr                             );
	DEFINE_OSSTATUS( kENOSTRErr,                              ENOSTRErr                              );
	DEFINE_OSSTATUS( kENODATAErr,                             ENODATAErr                             );
	DEFINE_OSSTATUS( kEINPROGRESSErr,                         EINPROGRESSErr                         );
	DEFINE_OSSTATUS( kESRCHErr,                               ESRCHErr                               );
	DEFINE_OSSTATUS( kENOMSGErr,                              ENOMSGErr                              );
	DEFINE_OSSTATUS( kOTClientNotInittedErr,                  OTClientNotInittedErr                  );
	DEFINE_OSSTATUS( kOTPortHasDiedErr,                       OTPortHasDiedErr                       );
	DEFINE_OSSTATUS( kOTPortWasEjectedErr,                    OTPortWasEjectedErr                    );
	DEFINE_OSSTATUS( kOTBadConfigurationErr,                  OTBadConfigurationErr                  );
	DEFINE_OSSTATUS( kOTConfigurationChangedErr,              OTConfigurationChangedErr              );
	DEFINE_OSSTATUS( kOTUserRequestedErr,                     OTUserRequestedErr                     );
	DEFINE_OSSTATUS( kOTPortLostConnection,                   OTPortLostConnection                   );


/* Color Picker errors*/
	DEFINE_OSSTATUS( firstPickerError,                        FirstPickerError                       );
	DEFINE_OSSTATUS( invalidPickerType,                       InvalidPickerType                      );
	DEFINE_OSSTATUS( requiredFlagsDontMatch,                  RequiredFlagsDontMatch                 );
	DEFINE_OSSTATUS( pickerResourceError,                     PickerResourceError                    );
	DEFINE_OSSTATUS( cantLoadPicker,                          CantLoadPicker                         );
	DEFINE_OSSTATUS( cantCreatePickerWindow,                  CantCreatePickerWindow                 );
	DEFINE_OSSTATUS( cantLoadPackage,                         CantLoadPackage                        );
	DEFINE_OSSTATUS( pickerCantLive,                          PickerCantLive                         );
	DEFINE_OSSTATUS( colorSyncNotInstalled,                   ColorSyncNotInstalled                  );
	DEFINE_OSSTATUS( badProfileError,                         BadProfileError                        );
	DEFINE_OSSTATUS( noHelpForItem,                           NoHelpForItem                          );



/* NSL error codes*/
	DEFINE_OSSTATUS( kNSL68kContextNotSupported,              NSL68kContextNotSupported              );
	DEFINE_OSSTATUS( kNSLSchedulerError,                      NSLSchedulerError                      );
	DEFINE_OSSTATUS( kNSLBadURLSyntax,                        NSLBadURLSyntax                        );
	DEFINE_OSSTATUS( kNSLNoCarbonLib,                         NSLNoCarbonLib                         );
	DEFINE_OSSTATUS( kNSLUILibraryNotAvailable,               NSLUILibraryNotAvailable               );
	DEFINE_OSSTATUS( kNSLNotImplementedYet,                   NSLNotImplementedYet                   );
	DEFINE_OSSTATUS( kNSLErrNullPtrError,                     NSLErrNullPtrError                     );
	DEFINE_OSSTATUS( kNSLSomePluginsFailedToLoad,             NSLSomePluginsFailedToLoad             );
	DEFINE_OSSTATUS( kNSLNullNeighborhoodPtr,                 NSLNullNeighborhoodPtr                 );
	DEFINE_OSSTATUS( kNSLNoPluginsForSearch,                  NSLNoPluginsForSearch                  );
	DEFINE_OSSTATUS( kNSLSearchAlreadyInProgress,             NSLSearchAlreadyInProgress             );
	DEFINE_OSSTATUS( kNSLNoPluginsFound,                      NSLNoPluginsFound                      );
	DEFINE_OSSTATUS( kNSLPluginLoadFailed,                    NSLPluginLoadFailed                    );
	DEFINE_OSSTATUS( kNSLBadProtocolTypeErr,                  NSLBadProtocolTypeErr                  );
	DEFINE_OSSTATUS( kNSLNullListPtr,                         NSLNullListPtr                         );
	DEFINE_OSSTATUS( kNSLBadClientInfoPtr,                    NSLBadClientInfoPtr                    );
	DEFINE_OSSTATUS( kNSLCannotContinueLookup,                NSLCannotContinueLookup                );
	DEFINE_OSSTATUS( kNSLBufferTooSmallForData,               NSLBufferTooSmallForData               );
	DEFINE_OSSTATUS( kNSLNoContextAvailable,                  NSLNoContextAvailable                  );
	DEFINE_OSSTATUS( kNSLRequestBufferAlreadyInList,          NSLRequestBufferAlreadyInList          );
	DEFINE_OSSTATUS( kNSLInvalidPluginSpec,                   NSLInvalidPluginSpec                   );
	DEFINE_OSSTATUS( kNSLNoSupportForService,                 NSLNoSupportForService                 );
	DEFINE_OSSTATUS( kNSLBadNetConnection,                    NSLBadNetConnection                    );
	DEFINE_OSSTATUS( kNSLBadDataTypeErr,                      NSLBadDataTypeErr                      );
	DEFINE_OSSTATUS( kNSLBadServiceTypeErr,                   NSLBadServiceTypeErr                   );
	DEFINE_OSSTATUS( kNSLBadReferenceErr,                     NSLBadReferenceErr                     );
	DEFINE_OSSTATUS( kNSLNoElementsInList,                    NSLNoElementsInList                    );
	DEFINE_OSSTATUS( kNSLInsufficientOTVer,                   NSLInsufficientOTVer                   );
	DEFINE_OSSTATUS( kNSLInsufficientSysVer,                  NSLInsufficientSysVer                  );
	DEFINE_OSSTATUS( kNSLNotInitialized,                      NSLNotInitialized                      );
	DEFINE_OSSTATUS( kNSLInitializationFailed,                NSLInitializationFailed                );



/* desktop printing error codes*/
	DEFINE_OSSTATUS( kDTPHoldJobErr,                          DTPHoldJobErr                          );
	DEFINE_OSSTATUS( kDTPStopQueueErr,                        DTPStopQueueErr                        );
	DEFINE_OSSTATUS( kDTPTryAgainErr,                         DTPTryAgainErr                         );
	DEFINE_OSSTATUS( kDTPAbortJobErr,                         DTPAbortJobErr                         );


/* ColorSync Result codes */
/* Profile Access Errors */
	DEFINE_OSSTATUS( cmElementTagNotFound,                    CMElementTagNotFound                   );
	DEFINE_OSSTATUS( cmIndexRangeErr,                         CMIndexRangeErr                        );
	DEFINE_OSSTATUS( cmCantDeleteElement,                     CMCantDeleteElement                    );
	DEFINE_OSSTATUS( cmFatalProfileErr,                       CMFatalProfileErr                      );
	DEFINE_OSSTATUS( cmInvalidProfile,                        CMInvalidProfile                       );
	DEFINE_OSSTATUS( cmInvalidProfileLocation,                CMInvalidProfileLocation               );
	DEFINE_OSSTATUS( cmCantCopyModifiedV1Profile,             CMCantCopyModifiedV1Profile            );
/* Profile Search Errors */
	DEFINE_OSSTATUS( cmInvalidSearch,                         CMInvalidSearch                        );
	DEFINE_OSSTATUS( cmSearchError,                           CMSearchError                          );
	DEFINE_OSSTATUS( cmErrIncompatibleProfile,                CMErrIncompatibleProfile               );
	DEFINE_OSSTATUS( cmInvalidColorSpace,                     CMInvalidColorSpace                    );
	DEFINE_OSSTATUS( cmInvalidSrcMap,                         CMInvalidSrcMap                        );
	DEFINE_OSSTATUS( cmInvalidDstMap,                         CMInvalidDstMap                        );
	DEFINE_OSSTATUS( cmNoGDevicesError,                       CMNoGDevicesError                      );
	DEFINE_OSSTATUS( cmInvalidProfileComment,                 CMInvalidProfileComment                );
	DEFINE_OSSTATUS( cmRangeOverFlow,                         CMRangeOverFlow                        );
	DEFINE_OSSTATUS( cmNamedColorNotFound,                    CMNamedColorNotFound                   );
	DEFINE_OSSTATUS( cmCantGamutCheckError,                   CMCantGamutCheckError                  );

/* new Folder Manager error codes */
	DEFINE_OSSTATUS( badFolderDescErr,                        BadFolderDescErr                       );
	DEFINE_OSSTATUS( duplicateFolderDescErr,                  DuplicateFolderDescErr                 );
	DEFINE_OSSTATUS( noMoreFolderDescErr,                     NoMoreFolderDescErr                    );
	DEFINE_OSSTATUS( invalidFolderTypeErr,                    InvalidFolderTypeErr                   );
	DEFINE_OSSTATUS( duplicateRoutingErr,                     DuplicateRoutingErr                    );
	DEFINE_OSSTATUS( routingNotFoundErr,                      RoutingNotFoundErr                     );
	DEFINE_OSSTATUS( badRoutingSizeErr,                       BadRoutingSizeErr                      );


/* Core Foundation errors*/
	DEFINE_OSSTATUS( coreFoundationUnknownErr,                CoreFoundationUnknownErr               );

/* ScrapMgr error codes (CarbonLib 1.0 and later)*/
	DEFINE_OSSTATUS( internalScrapErr,                        InternalScrapErr                       );
	DEFINE_OSSTATUS( duplicateScrapFlavorErr,                 DuplicateScrapFlavorErr                );
	DEFINE_OSSTATUS( badScrapRefErr,                          BadScrapRefErr                         );
	DEFINE_OSSTATUS( processStateIncorrectErr,                ProcessStateIncorrectErr               );
	DEFINE_OSSTATUS( scrapPromiseNotKeptErr,                  ScrapPromiseNotKeptErr                 );
	DEFINE_OSSTATUS( noScrapPromiseKeeperErr,                 NoScrapPromiseKeeperErr                );
	DEFINE_OSSTATUS( nilScrapFlavorDataErr,                   NilScrapFlavorDataErr                  );
	DEFINE_OSSTATUS( scrapFlavorFlagsMismatchErr,             ScrapFlavorFlagsMismatchErr            );
	DEFINE_OSSTATUS( scrapFlavorSizeMismatchErr,              ScrapFlavorSizeMismatchErr             );
	DEFINE_OSSTATUS( illegalScrapFlavorFlagsErr,              IllegalScrapFlavorFlagsErr             );
	DEFINE_OSSTATUS( illegalScrapFlavorTypeErr,               IllegalScrapFlavorTypeErr              );
	DEFINE_OSSTATUS( illegalScrapFlavorSizeErr,               IllegalScrapFlavorSizeErr              );
	DEFINE_OSSTATUS( scrapFlavorNotFoundErr,                  ScrapFlavorNotFoundErr                 );
	DEFINE_OSSTATUS( needClearScrapErr,                       NeedClearScrapErr                      );


/*  AFP Protocol Errors */
	DEFINE_OSSTATUS( afpAccessDenied,                         AFPAccessDenied                        );
	DEFINE_OSSTATUS( afpAuthContinue,                         AFPAuthContinue                        );
	DEFINE_OSSTATUS( afpBadUAM,                               AFPBadUAM                              );
	DEFINE_OSSTATUS( afpBadVersNum,                           AFPBadVersNum                          );
	DEFINE_OSSTATUS( afpBitmapErr,                            AFPBitmapErr                           );
	DEFINE_OSSTATUS( afpCantMove,                             AFPCantMove                            );
	DEFINE_OSSTATUS( afpDenyConflict,                         AFPDenyConflict                        );
	DEFINE_OSSTATUS( afpDirNotEmpty,                          AFPDirNotEmpty                         );
	DEFINE_OSSTATUS( afpDiskFull,                             AFPDiskFull                            );
	DEFINE_OSSTATUS( afpEofError,                             AFPEofError                            );
	DEFINE_OSSTATUS( afpFileBusy,                             AFPFileBusy                            );
	DEFINE_OSSTATUS( afpFlatVol,                              AFPFlatVol                             );
	DEFINE_OSSTATUS( afpItemNotFound,                         AFPItemNotFound                        );
	DEFINE_OSSTATUS( afpLockErr,                              AFPLockErr                             );
	DEFINE_OSSTATUS( afpMiscErr,                              AFPMiscErr                             );
	DEFINE_OSSTATUS( afpNoMoreLocks,                          AFPNoMoreLocks                         );
	DEFINE_OSSTATUS( afpNoServer,                             AFPNoServer                            );
	DEFINE_OSSTATUS( afpObjectExists,                         AFPObjectExists                        );
	DEFINE_OSSTATUS( afpObjectNotFound,                       AFPObjectNotFound                      );
	DEFINE_OSSTATUS( afpParmErr,                              AFPParmErr                             );
	DEFINE_OSSTATUS( afpRangeNotLocked,                       AFPRangeNotLocked                      );
	DEFINE_OSSTATUS( afpRangeOverlap,                         AFPRangeOverlap                        );
	DEFINE_OSSTATUS( afpSessClosed,                           AFPSessClosed                          );
	DEFINE_OSSTATUS( afpUserNotAuth,                          AFPUserNotAuth                         );
	DEFINE_OSSTATUS( afpCallNotSupported,                     AFPCallNotSupported                    );
	DEFINE_OSSTATUS( afpObjectTypeErr,                        AFPObjectTypeErr                       );
	DEFINE_OSSTATUS( afpTooManyFilesOpen,                     AFPTooManyFilesOpen                    );
	DEFINE_OSSTATUS( afpServerGoingDown,                      AFPServerGoingDown                     );
	DEFINE_OSSTATUS( afpCantRename,                           AFPCantRename                          );
	DEFINE_OSSTATUS( afpDirNotFound,                          AFPDirNotFound                         );
	DEFINE_OSSTATUS( afpIconTypeError,                        AFPIconTypeError                       );
	DEFINE_OSSTATUS( afpVolLocked,                            AFPVolLocked                           );
	DEFINE_OSSTATUS( afpObjectLocked,                         AFPObjectLocked                        );
	DEFINE_OSSTATUS( afpContainsSharedErr,                    AFPContainsSharedErr                   );
	DEFINE_OSSTATUS( afpIDNotFound,                           AFPIDNotFound                          );
	DEFINE_OSSTATUS( afpIDExists,                             AFPIDExists                            );
	DEFINE_OSSTATUS( afpDiffVolErr,                           AFPDiffVolErr                          );
	DEFINE_OSSTATUS( afpCatalogChanged,                       AFPCatalogChanged                      );
	DEFINE_OSSTATUS( afpSameObjectErr,                        AFPSameObjectErr                       );
	DEFINE_OSSTATUS( afpBadIDErr,                             AFPBadIDErr                            );
	DEFINE_OSSTATUS( afpPwdSameErr,                           AFPPwdSameErr                          );
	DEFINE_OSSTATUS( afpPwdTooShortErr,                       AFPPwdTooShortErr                      );
	DEFINE_OSSTATUS( afpPwdExpiredErr,                        AFPPwdExpiredErr                       );
	DEFINE_OSSTATUS( afpInsideSharedErr,                      AFPInsideSharedErr                     );
	DEFINE_OSSTATUS( afpInsideTrashErr,                       AFPInsideTrashErr                      );
	DEFINE_OSSTATUS( afpPwdNeedsChangeErr,                    AFPPwdNeedsChangeErr                   );
	DEFINE_OSSTATUS( afpPwdPolicyErr,                         AFPPwdPolicyErr                        );
	DEFINE_OSSTATUS( afpAlreadyLoggedInErr,                   AFPAlreadyLoggedInErr                  );
	DEFINE_OSSTATUS( afpCallNotAllowed,                       AFPCallNotAllowed                      );

/*  AppleShare Client Errors */
	DEFINE_OSSTATUS( afpBadDirIDType,                         AFPBadDirIDType                        );
	DEFINE_OSSTATUS( afpCantMountMoreSrvre,                   AFPCantMountMoreSrvre                  );
	DEFINE_OSSTATUS( afpAlreadyMounted,                       AFPAlreadyMounted                      );
	DEFINE_OSSTATUS( afpSameNodeErr,                          AFPSameNodeErr                         );

/*Text Engines, TSystemTextEngines, HIEditText error coded*/
	DEFINE_OSSTATUS( errUnknownAttributeTag,                  ErrUnknownAttributeTag                 );
	DEFINE_OSSTATUS( errMarginWilllNotFit,                    ErrMarginWilllNotFit                   );
	DEFINE_OSSTATUS( errNotInImagingMode,                     ErrNotInImagingMode                    );
	DEFINE_OSSTATUS( errAlreadyInImagingMode,                 ErrAlreadyInImagingMode                );
	DEFINE_OSSTATUS( errEngineNotFound,                       ErrEngineNotFound                      );
	DEFINE_OSSTATUS( errIteratorReachedEnd,                   ErrIteratorReachedEnd                  );
	DEFINE_OSSTATUS( errInvalidRange,                         ErrInvalidRange                        );
	DEFINE_OSSTATUS( errOffsetNotOnElementBounday,            ErrOffsetNotOnElementBounday           );
	DEFINE_OSSTATUS( errNoHiliteText,                         ErrNoHiliteText                        );
	DEFINE_OSSTATUS( errEmptyScrap,                           ErrEmptyScrap                          );
	DEFINE_OSSTATUS( errReadOnlyText,                         ErrReadOnlyText                        );
	DEFINE_OSSTATUS( errUnknownElement,                       ErrUnknownElement                      );
	DEFINE_OSSTATUS( errNonContiuousAttribute,                ErrNonContiuousAttribute               );
	DEFINE_OSSTATUS( errCannotUndo,                           ErrCannotUndo                          );

/* HTMLRendering OSStaus codes*/
	DEFINE_OSSTATUS( hrHTMLRenderingLibNotInstalledErr,       HRHTMLRenderingLibNotInstalledErr      );
	DEFINE_OSSTATUS( hrMiscellaneousExceptionErr,             HRMiscellaneousExceptionErr            );
	DEFINE_OSSTATUS( hrUnableToResizeHandleErr,               HRUnableToResizeHandleErr              );
	DEFINE_OSSTATUS( hrURLNotHandledErr,                      HRURLNotHandledErr                     );

/* IAExtractor result codes */
	DEFINE_OSSTATUS( errIANoErr,                              ErrIANoErr                             );
	DEFINE_OSSTATUS( errIAUnknownErr,                         ErrIAUnknownErr                        );
	DEFINE_OSSTATUS( errIAAllocationErr,                      ErrIAAllocationErr                     );
	DEFINE_OSSTATUS( errIAParamErr,                           ErrIAParamErr                          );
	DEFINE_OSSTATUS( errIANoMoreItems,                        ErrIANoMoreItems                       );
	DEFINE_OSSTATUS( errIABufferTooSmall,                     ErrIABufferTooSmall                    );
	DEFINE_OSSTATUS( errIACanceled,                           ErrIACanceled                          );
	DEFINE_OSSTATUS( errIAInvalidDocument,                    ErrIAInvalidDocument                   );
	DEFINE_OSSTATUS( errIATextExtractionErr,                  ErrIATextExtractionErr                 );
	DEFINE_OSSTATUS( errIAEndOfTextRun,                       ErrIAEndOfTextRun                      );

/* QuickTime Streaming Errors */
	DEFINE_OSSTATUS( qtsBadSelectorErr,                       QTSBadSelectorErr                      );
	DEFINE_OSSTATUS( qtsBadStateErr,                          QTSBadStateErr                         );
	DEFINE_OSSTATUS( qtsBadDataErr,                           QTSBadDataErr                          );
	DEFINE_OSSTATUS( qtsUnsupportedDataTypeErr,               QTSUnsupportedDataTypeErr              );
	DEFINE_OSSTATUS( qtsUnsupportedRateErr,                   QTSUnsupportedRateErr                  );
	DEFINE_OSSTATUS( qtsUnsupportedFeatureErr,                QTSUnsupportedFeatureErr               );
	DEFINE_OSSTATUS( qtsTooMuchDataErr,                       QTSTooMuchDataErr                      );
	DEFINE_OSSTATUS( qtsUnknownValueErr,                      QTSUnknownValueErr                     );
	DEFINE_OSSTATUS( qtsTimeoutErr,                           QTSTimeoutErr                          );
	DEFINE_OSSTATUS( qtsConnectionFailedErr,                  QTSConnectionFailedErr                 );
	DEFINE_OSSTATUS( qtsAddressBusyErr,                       QTSAddressBusyErr                      );

/*Gestalt error codes*/
	DEFINE_OSSTATUS( gestaltUnknownErr,                       GestaltUnknownErr                      );
	DEFINE_OSSTATUS( gestaltUndefSelectorErr,                 GestaltUndefSelectorErr                );
	DEFINE_OSSTATUS( gestaltDupSelectorErr,                   GestaltDupSelectorErr                  );
	DEFINE_OSSTATUS( gestaltLocationErr,                      GestaltLocationErr                     );


/* Menu Manager error codes*/
	DEFINE_OSSTATUS( menuPropertyInvalidErr,                  MenuPropertyInvalidErr                 );
	DEFINE_OSSTATUS( menuPropertyInvalid,                     MenuPropertyInvalid                    );
	DEFINE_OSSTATUS( menuPropertyNotFoundErr,                 MenuPropertyNotFoundErr                );
	DEFINE_OSSTATUS( menuNotFoundErr,                         MenuNotFoundErr                        );
	DEFINE_OSSTATUS( menuUsesSystemDefErr,                    MenuUsesSystemDefErr                   );
	DEFINE_OSSTATUS( menuItemNotFoundErr,                     MenuItemNotFoundErr                    );
	DEFINE_OSSTATUS( menuInvalidErr,                          MenuInvalidErr                         );


/* Window Manager error codes*/
	DEFINE_OSSTATUS( errInvalidWindowPtr,                     ErrInvalidWindowPtr                    );
	DEFINE_OSSTATUS( errInvalidWindowRef,                     ErrInvalidWindowRef                    );
	DEFINE_OSSTATUS( errUnsupportedWindowAttributesForClass,  ErrUnsupportedWindowAttributesForClass );
	DEFINE_OSSTATUS( errWindowDoesNotHaveProxy,               ErrWindowDoesNotHaveProxy              );
	DEFINE_OSSTATUS( errInvalidWindowProperty,                ErrInvalidWindowProperty               );
	DEFINE_OSSTATUS( errWindowPropertyNotFound,               ErrWindowPropertyNotFound              );
	DEFINE_OSSTATUS( errUnrecognizedWindowClass,              ErrUnrecognizedWindowClass             );
	DEFINE_OSSTATUS( errCorruptWindowDescription,             ErrCorruptWindowDescription            );
	DEFINE_OSSTATUS( errUserWantsToDragWindow,                ErrUserWantsToDragWindow               );
	DEFINE_OSSTATUS( errWindowsAlreadyInitialized,            ErrWindowsAlreadyInitialized           );
	DEFINE_OSSTATUS( errFloatingWindowsNotInitialized,        ErrFloatingWindowsNotInitialized       );
	DEFINE_OSSTATUS( errWindowNotFound,                       ErrWindowNotFound                      );
	DEFINE_OSSTATUS( errWindowDoesNotFitOnscreen,             ErrWindowDoesNotFitOnscreen            );
	DEFINE_OSSTATUS( windowAttributeImmutableErr,             WindowAttributeImmutableErr            );
	DEFINE_OSSTATUS( windowAttributesConflictErr,             WindowAttributesConflictErr            );
	DEFINE_OSSTATUS( windowManagerInternalErr,                WindowManagerInternalErr               );
	DEFINE_OSSTATUS( windowWrongStateErr,                     WindowWrongStateErr                    );
	DEFINE_OSSTATUS( windowGroupInvalidErr,                   WindowGroupInvalidErr                  );
	DEFINE_OSSTATUS( windowAppModalStateAlreadyExistsErr,     WindowAppModalStateAlreadyExistsErr    );
	DEFINE_OSSTATUS( windowNoAppModalStateErr,                WindowNoAppModalStateErr               );
	DEFINE_OSSTATUS( errWindowDoesntSupportFocus,             ErrWindowDoesntSupportFocus            );
	DEFINE_OSSTATUS( errWindowRegionCodeInvalid,              ErrWindowRegionCodeInvalid             );

/* Dialog Mgr error codes*/
	DEFINE_OSSTATUS( dialogNoTimeoutErr,                      DialogNoTimeoutErr                     );

/* NavigationLib error codes*/
	DEFINE_OSSTATUS( kNavWrongDialogStateErr,                 NavWrongDialogStateErr                 );
	DEFINE_OSSTATUS( kNavWrongDialogClassErr,                 NavWrongDialogClassErr                 );
	DEFINE_OSSTATUS( kNavInvalidSystemConfigErr,              NavInvalidSystemConfigErr              );
	DEFINE_OSSTATUS( kNavCustomControlMessageFailedErr,       NavCustomControlMessageFailedErr       );
	DEFINE_OSSTATUS( kNavInvalidCustomControlMessageErr,      NavInvalidCustomControlMessageErr      );
	DEFINE_OSSTATUS( kNavMissingKindStringErr,                NavMissingKindStringErr                );


/* Collection Manager errors */
	DEFINE_OSSTATUS( collectionItemLockedErr,                 CollectionItemLockedErr                );
	DEFINE_OSSTATUS( collectionItemNotFoundErr,               CollectionItemNotFoundErr              );
	DEFINE_OSSTATUS( collectionIndexRangeErr,                 CollectionIndexRangeErr                );
	DEFINE_OSSTATUS( collectionVersionErr,                    CollectionVersionErr                   );


/* QuickTime Streaming Server Errors */
	DEFINE_OSSTATUS( kQTSSUnknownErr,                         QTSSUnknownErr                         );


/* Display Manager error codes (-6220...-6269)*/
	DEFINE_OSSTATUS( kDMGenErr,                               DMGenErr                               );
	DEFINE_OSSTATUS( kDMMirroringOnAlready,                   DMMirroringOnAlready                   );
	DEFINE_OSSTATUS( kDMWrongNumberOfDisplays,                DMWrongNumberOfDisplays                );
	DEFINE_OSSTATUS( kDMMirroringBlocked,                     DMMirroringBlocked                     );
	DEFINE_OSSTATUS( kDMCantBlock,                            DMCantBlock                            );
	DEFINE_OSSTATUS( kDMMirroringNotOn,                       DMMirroringNotOn                       );
	DEFINE_OSSTATUS( kSysSWTooOld,                            SysSWTooOld                            );
	DEFINE_OSSTATUS( kDMSWNotInitializedErr,                  DMSWNotInitializedErr                  );
	DEFINE_OSSTATUS( kDMDriverNotDisplayMgrAwareErr,          DMDriverNotDisplayMgrAwareErr          );
	DEFINE_OSSTATUS( kDMDisplayNotFoundErr,                   DMDisplayNotFoundErr                   );
	DEFINE_OSSTATUS( kDMNotFoundErr,                          DMNotFoundErr                          );
	DEFINE_OSSTATUS( kDMDisplayAlreadyInstalledErr,           DMDisplayAlreadyInstalledErr           );
	DEFINE_OSSTATUS( kDMMainDisplayCannotMoveErr,             DMMainDisplayCannotMoveErr             );
	DEFINE_OSSTATUS( kDMNoDeviceTableclothErr,                DMNoDeviceTableclothErr                );
	DEFINE_OSSTATUS( kDMFoundErr,                             DMFoundErr                             );


/* Language Analysis error codes */
	DEFINE_OSSTATUS( laTooSmallBufferErr,                     LATooSmallBufferErr                    );
	DEFINE_OSSTATUS( laEnvironmentBusyErr,                    LAEnvironmentBusyErr                   );
	DEFINE_OSSTATUS( laEnvironmentNotFoundErr,                LAEnvironmentNotFoundErr               );
	DEFINE_OSSTATUS( laEnvironmentExistErr,                   LAEnvironmentExistErr                  );
	DEFINE_OSSTATUS( laInvalidPathErr,                        LAInvalidPathErr                       );
	DEFINE_OSSTATUS( laNoMoreMorphemeErr,                     LANoMoreMorphemeErr                    );
	DEFINE_OSSTATUS( laFailAnalysisErr,                       LAFailAnalysisErr                      );
	DEFINE_OSSTATUS( laTextOverFlowErr,                       LATextOverFlowErr                      );
	DEFINE_OSSTATUS( laDictionaryNotOpenedErr,                LADictionaryNotOpenedErr               );
	DEFINE_OSSTATUS( laDictionaryUnknownErr,                  LADictionaryUnknownErr                 );
	DEFINE_OSSTATUS( laDictionaryTooManyErr,                  LADictionaryTooManyErr                 );
	DEFINE_OSSTATUS( laPropertyValueErr,                      LAPropertyValueErr                     );
	DEFINE_OSSTATUS( laPropertyUnknownErr,                    LAPropertyUnknownErr                   );
	DEFINE_OSSTATUS( laPropertyIsReadOnlyErr,                 LAPropertyIsReadOnlyErr                );
	DEFINE_OSSTATUS( laPropertyNotFoundErr,                   LAPropertyNotFoundErr                  );
	DEFINE_OSSTATUS( laPropertyErr,                           LAPropertyErr                          );
	DEFINE_OSSTATUS( laEngineNotFoundErr,                     LAEngineNotFoundErr                    );


	DEFINE_OSSTATUS( kUSBNoErr,                               USBNoErr                               );
	DEFINE_OSSTATUS( kUSBNoTran,                              USBNoTran                              );
	DEFINE_OSSTATUS( kUSBNoDelay,                             USBNoDelay                             );
	DEFINE_OSSTATUS( kUSBPending,                             USBPending                             );

/* USB Hardware Errors */
	DEFINE_OSSTATUS( kUSBNotSent2Err,                         USBNotSent2Err                         );
	DEFINE_OSSTATUS( kUSBNotSent1Err,                         USBNotSent1Err                         );
	DEFINE_OSSTATUS( kUSBBufUnderRunErr,                      USBBufUnderRunErr                      );
	DEFINE_OSSTATUS( kUSBBufOvrRunErr,                        USBBufOvrRunErr                        );
	DEFINE_OSSTATUS( kUSBRes2Err,                             USBRes2Err                             );
	DEFINE_OSSTATUS( kUSBRes1Err,                             USBRes1Err                             );
	DEFINE_OSSTATUS( kUSBUnderRunErr,                         USBUnderRunErr                         );
	DEFINE_OSSTATUS( kUSBOverRunErr,                          USBOverRunErr                          );
	DEFINE_OSSTATUS( kUSBWrongPIDErr,                         USBWrongPIDErr                         );
	DEFINE_OSSTATUS( kUSBPIDCheckErr,                         USBPIDCheckErr                         );
	DEFINE_OSSTATUS( kUSBNotRespondingErr,                    USBNotRespondingErr                    );
	DEFINE_OSSTATUS( kUSBEndpointStallErr,                    USBEndpointStallErr                    );
	DEFINE_OSSTATUS( kUSBDataToggleErr,                       USBDataToggleErr                       );
	DEFINE_OSSTATUS( kUSBBitstufErr,                          USBBitstufErr                          );
	DEFINE_OSSTATUS( kUSBCRCErr,                              USBCRCErr                              );
	DEFINE_OSSTATUS( kUSBLinkErr,                             USBLinkErr                             );


/* USB Manager Errors */
	DEFINE_OSSTATUS( kUSBQueueFull,                           USBQueueFull                           );
	DEFINE_OSSTATUS( kUSBNotHandled,                          USBNotHandled                          );
	DEFINE_OSSTATUS( kUSBUnknownNotification,                 USBUnknownNotification                 );
	DEFINE_OSSTATUS( kUSBBadDispatchTable,                    USBBadDispatchTable                    );


/* USB Services Errors */
	DEFINE_OSSTATUS( kUSBPortDisabled,                        USBPortDisabled                        );
	DEFINE_OSSTATUS( kUSBQueueAborted,                        USBQueueAborted                        );
	DEFINE_OSSTATUS( kUSBTimedOut,                            USBTimedOut                            );
	DEFINE_OSSTATUS( kUSBDeviceDisconnected,                  USBDeviceDisconnected                  );
	DEFINE_OSSTATUS( kUSBDeviceNotSuspended,                  USBDeviceNotSuspended                  );
	DEFINE_OSSTATUS( kUSBDeviceSuspended,                     USBDeviceSuspended                     );
	DEFINE_OSSTATUS( kUSBInvalidBuffer,                       USBInvalidBuffer                       );
	DEFINE_OSSTATUS( kUSBDevicePowerProblem,                  USBDevicePowerProblem                  );
	DEFINE_OSSTATUS( kUSBDeviceBusy,                          USBDeviceBusy                          );
	DEFINE_OSSTATUS( kUSBUnknownInterfaceErr,                 USBUnknownInterfaceErr                 );
	DEFINE_OSSTATUS( kUSBPipeStalledError,                    USBPipeStalledError                    );
	DEFINE_OSSTATUS( kUSBPipeIdleError,                       USBPipeIdleError                       );
	DEFINE_OSSTATUS( kUSBNoBandwidthError,                    USBNoBandwidthError                    );
	DEFINE_OSSTATUS( kUSBAbortedError,                        USBAbortedError                        );
	DEFINE_OSSTATUS( kUSBFlagsError,                          USBFlagsError                          );
	DEFINE_OSSTATUS( kUSBCompletionError,                     USBCompletionError                     );
	DEFINE_OSSTATUS( kUSBPBLengthError,                       USBPBLengthError                       );
	DEFINE_OSSTATUS( kUSBPBVersionError,                      USBPBVersionError                      );
	DEFINE_OSSTATUS( kUSBNotFound,                            USBNotFound                            );
	DEFINE_OSSTATUS( kUSBOutOfMemoryErr,                      USBOutOfMemoryErr                      );
	DEFINE_OSSTATUS( kUSBDeviceErr,                           USBDeviceErr                           );
	DEFINE_OSSTATUS( kUSBNoDeviceErr,                         USBNoDeviceErr                         );
	DEFINE_OSSTATUS( kUSBAlreadyOpenErr,                      USBAlreadyOpenErr                      );
	DEFINE_OSSTATUS( kUSBTooManyTransactionsErr,              USBTooManyTransactionsErr              );
	DEFINE_OSSTATUS( kUSBUnknownRequestErr,                   USBUnknownRequestErr                   );
	DEFINE_OSSTATUS( kUSBRqErr,                               USBRqErr                               );
	DEFINE_OSSTATUS( kUSBIncorrectTypeErr,                    USBIncorrectTypeErr                    );
	DEFINE_OSSTATUS( kUSBTooManyPipesErr,                     USBTooManyPipesErr                     );
	DEFINE_OSSTATUS( kUSBUnknownPipeErr,                      USBUnknownPipeErr                      );
	DEFINE_OSSTATUS( kUSBUnknownDeviceErr,                    USBUnknownDeviceErr                    );
	DEFINE_OSSTATUS( kUSBInternalErr,                         USBInternalErr                         );


/* DictionaryMgr error codes */
	DEFINE_OSSTATUS( dcmParamErr,                             DCMParamErr                            );
	DEFINE_OSSTATUS( dcmNotDictionaryErr,                     DCMNotDictionaryErr                    );
	DEFINE_OSSTATUS( dcmBadDictionaryErr,                     DCMBadDictionaryErr                    );
	DEFINE_OSSTATUS( dcmPermissionErr,                        DCMPermissionErr                       );
	DEFINE_OSSTATUS( dcmDictionaryNotOpenErr,                 DCMDictionaryNotOpenErr                );
	DEFINE_OSSTATUS( dcmDictionaryBusyErr,                    DCMDictionaryBusyErr                   );
	DEFINE_OSSTATUS( dcmBlockFullErr,                         DCMBlockFullErr                        );
	DEFINE_OSSTATUS( dcmNoRecordErr,                          DCMNoRecordErr                         );
	DEFINE_OSSTATUS( dcmDupRecordErr,                         DCMDupRecordErr                        );
	DEFINE_OSSTATUS( dcmNecessaryFieldErr,                    DCMNecessaryFieldErr                   );
	DEFINE_OSSTATUS( dcmBadFieldInfoErr,                      DCMBadFieldInfoErr                     );
	DEFINE_OSSTATUS( dcmBadFieldTypeErr,                      DCMBadFieldTypeErr                     );
	DEFINE_OSSTATUS( dcmNoFieldErr,                           DCMNoFieldErr                          );
	DEFINE_OSSTATUS( dcmBadKeyErr,                            DCMBadKeyErr                           );
	DEFINE_OSSTATUS( dcmTooManyKeyErr,                        DCMTooManyKeyErr                       );
	DEFINE_OSSTATUS( dcmBadDataSizeErr,                       DCMBadDataSizeErr                      );
	DEFINE_OSSTATUS( dcmBadFindMethodErr,                     DCMBadFindMethodErr                    );
	DEFINE_OSSTATUS( dcmBadPropertyErr,                       DCMBadPropertyErr                      );
	DEFINE_OSSTATUS( dcmProtectedErr,                         DCMProtectedErr                        );
	DEFINE_OSSTATUS( dcmNoAccessMethodErr,                    DCMNoAccessMethodErr                   );
	DEFINE_OSSTATUS( dcmBadFeatureErr,                        DCMBadFeatureErr                       );
	DEFINE_OSSTATUS( dcmIterationCompleteErr,                 DCMIterationCompleteErr                );
	DEFINE_OSSTATUS( dcmBufferOverflowErr,                    DCMBufferOverflowErr                   );


/* Apple Remote Access error codes*/
	DEFINE_OSSTATUS( kRAInvalidParameter,                     RAInvalidParameter                     );
	DEFINE_OSSTATUS( kRAInvalidPort,                          RAInvalidPort                          );
	DEFINE_OSSTATUS( kRAStartupFailed,                        RAStartupFailed                        );
	DEFINE_OSSTATUS( kRAPortSetupFailed,                      RAPortSetupFailed                      );
	DEFINE_OSSTATUS( kRAOutOfMemory,                          RAOutOfMemory                          );
	DEFINE_OSSTATUS( kRANotSupported,                         RANotSupported                         );
	DEFINE_OSSTATUS( kRAMissingResources,                     RAMissingResources                     );
	DEFINE_OSSTATUS( kRAIncompatiblePrefs,                    RAIncompatiblePrefs                    );
	DEFINE_OSSTATUS( kRANotConnected,                         RANotConnected                         );
	DEFINE_OSSTATUS( kRAConnectionCanceled,                   RAConnectionCanceled                   );
	DEFINE_OSSTATUS( kRAUnknownUser,                          RAUnknownUser                          );
	DEFINE_OSSTATUS( kRAInvalidPassword,                      RAInvalidPassword                      );
	DEFINE_OSSTATUS( kRAInternalError,                        RAInternalError                        );
	DEFINE_OSSTATUS( kRAInstallationDamaged,                  RAInstallationDamaged                  );
	DEFINE_OSSTATUS( kRAPortBusy,                             RAPortBusy                             );
	DEFINE_OSSTATUS( kRAUnknownPortState,                     RAUnknownPortState                     );
	DEFINE_OSSTATUS( kRAInvalidPortState,                     RAInvalidPortState                     );
	DEFINE_OSSTATUS( kRAInvalidSerialProtocol,                RAInvalidSerialProtocol                );
	DEFINE_OSSTATUS( kRAUserLoginDisabled,                    RAUserLoginDisabled                    );
	DEFINE_OSSTATUS( kRAUserPwdChangeRequired,                RAUserPwdChangeRequired                );
	DEFINE_OSSTATUS( kRAUserPwdEntryRequired,                 RAUserPwdEntryRequired                 );
	DEFINE_OSSTATUS( kRAUserInteractionRequired,              RAUserInteractionRequired              );
	DEFINE_OSSTATUS( kRAInitOpenTransportFailed,              RAInitOpenTransportFailed              );
	DEFINE_OSSTATUS( kRARemoteAccessNotReady,                 RARemoteAccessNotReady                 );
	DEFINE_OSSTATUS( kRATCPIPInactive,                        RATCPIPInactive                        );
	DEFINE_OSSTATUS( kRATCPIPNotConfigured,                   RATCPIPNotConfigured                   );
	DEFINE_OSSTATUS( kRANotPrimaryInterface,                  RANotPrimaryInterface                  );
	DEFINE_OSSTATUS( kRAConfigurationDBInitErr,               RAConfigurationDBInitErr               );
	DEFINE_OSSTATUS( kRAPPPProtocolRejected,                  RAPPPProtocolRejected                  );
	DEFINE_OSSTATUS( kRAPPPAuthenticationFailed,              RAPPPAuthenticationFailed              );
	DEFINE_OSSTATUS( kRAPPPNegotiationFailed,                 RAPPPNegotiationFailed                 );
	DEFINE_OSSTATUS( kRAPPPUserDisconnected,                  RAPPPUserDisconnected                  );
	DEFINE_OSSTATUS( kRAPPPPeerDisconnected,                  RAPPPPeerDisconnected                  );
	DEFINE_OSSTATUS( kRAPeerNotResponding,                    RAPeerNotResponding                    );
	DEFINE_OSSTATUS( kRAATalkInactive,                        RAATalkInactive                        );
	DEFINE_OSSTATUS( kRAExtAuthenticationFailed,              RAExtAuthenticationFailed              );
	DEFINE_OSSTATUS( kRANCPRejectedbyPeer,                    RANCPRejectedbyPeer                    );
	DEFINE_OSSTATUS( kRADuplicateIPAddr,                      RADuplicateIPAddr                      );
	DEFINE_OSSTATUS( kRACallBackFailed,                       RACallBackFailed                       );
	DEFINE_OSSTATUS( kRANotEnabled,                           RANotEnabled                           );

/* ATSUnicode error codes*/
	DEFINE_OSSTATUS( kATSUInvalidTextLayoutErr,               ATSUInvalidTextLayoutErr               );
	DEFINE_OSSTATUS( kATSUInvalidStyleErr,                    ATSUInvalidStyleErr                    );
	DEFINE_OSSTATUS( kATSUInvalidTextRangeErr,                ATSUInvalidTextRangeErr                );
	DEFINE_OSSTATUS( kATSUFontsMatched,                       ATSUFontsMatched                       );
	DEFINE_OSSTATUS( kATSUFontsNotMatched,                    ATSUFontsNotMatched                    );
	DEFINE_OSSTATUS( kATSUNoCorrespondingFontErr,             ATSUNoCorrespondingFontErr             );
	DEFINE_OSSTATUS( kATSUInvalidFontErr,                     ATSUInvalidFontErr                     );
	DEFINE_OSSTATUS( kATSUInvalidAttributeValueErr,           ATSUInvalidAttributeValueErr           );
	DEFINE_OSSTATUS( kATSUInvalidAttributeSizeErr,            ATSUInvalidAttributeSizeErr            );
	DEFINE_OSSTATUS( kATSUInvalidAttributeTagErr,             ATSUInvalidAttributeTagErr             );
	DEFINE_OSSTATUS( kATSUInvalidCacheErr,                    ATSUInvalidCacheErr                    );
	DEFINE_OSSTATUS( kATSUNotSetErr,                          ATSUNotSetErr                          );
	DEFINE_OSSTATUS( kATSUNoStyleRunsAssignedErr,             ATSUNoStyleRunsAssignedErr             );
	DEFINE_OSSTATUS( kATSUQuickDrawTextErr,                   ATSUQuickDrawTextErr                   );
	DEFINE_OSSTATUS( kATSULowLevelErr,                        ATSULowLevelErr                        );
	DEFINE_OSSTATUS( kATSUNoFontCmapAvailableErr,             ATSUNoFontCmapAvailableErr             );
	DEFINE_OSSTATUS( kATSUNoFontScalerAvailableErr,           ATSUNoFontScalerAvailableErr           );
	DEFINE_OSSTATUS( kATSUCoordinateOverflowErr,              ATSUCoordinateOverflowErr              );
	DEFINE_OSSTATUS( kATSULineBreakInWord,                    ATSULineBreakInWord                    );
	DEFINE_OSSTATUS( kATSUBusyObjectErr,                      ATSUBusyObjectErr                      );
	DEFINE_OSSTATUS( kATSUInvalidFontFallbacksErr,            ATSUInvalidFontFallbacksErr            );
	DEFINE_OSSTATUS( kATSULastErr,                            ATSULastErr                            );


/* general text errors*/
	DEFINE_OSSTATUS( kTextUnsupportedEncodingErr,             TextUnsupportedEncodingErr             );
	DEFINE_OSSTATUS( kTextMalformedInputErr,                  TextMalformedInputErr                  );
	DEFINE_OSSTATUS( kTextUndefinedElementErr,                TextUndefinedElementErr                );
	DEFINE_OSSTATUS( kTECMissingTableErr,                     TECMissingTableErr                     );
	DEFINE_OSSTATUS( kTECTableChecksumErr,                    TECTableChecksumErr                    );
	DEFINE_OSSTATUS( kTECTableFormatErr,                      TECTableFormatErr                      );
	DEFINE_OSSTATUS( kTECCorruptConverterErr,                 TECCorruptConverterErr                 );
	DEFINE_OSSTATUS( kTECNoConversionPathErr,                 TECNoConversionPathErr                 );
	DEFINE_OSSTATUS( kTECBufferBelowMinimumSizeErr,           TECBufferBelowMinimumSizeErr           );
	DEFINE_OSSTATUS( kTECArrayFullErr,                        TECArrayFullErr                        );
	DEFINE_OSSTATUS( kTECBadTextRunErr,                       TECBadTextRunErr                       );
	DEFINE_OSSTATUS( kTECPartialCharErr,                      TECPartialCharErr                      );
	DEFINE_OSSTATUS( kTECUnmappableElementErr,                TECUnmappableElementErr                );
	DEFINE_OSSTATUS( kTECIncompleteElementErr,                TECIncompleteElementErr                );
	DEFINE_OSSTATUS( kTECDirectionErr,                        TECDirectionErr                        );
	DEFINE_OSSTATUS( kTECGlobalsUnavailableErr,               TECGlobalsUnavailableErr               );
	DEFINE_OSSTATUS( kTECItemUnavailableErr,                  TECItemUnavailableErr                  );

/* text conversion status codes*/
	DEFINE_OSSTATUS( kTECUsedFallbacksStatus,                 TECUsedFallbacksStatus                 );
	DEFINE_OSSTATUS( kTECNeedFlushStatus,                     TECNeedFlushStatus                     );
	DEFINE_OSSTATUS( kTECOutputBufferFullStatus,              TECOutputBufferFullStatus              );

/* deprecated error & status codes for low-level converter*/
	DEFINE_OSSTATUS( unicodeChecksumErr,                      UnicodeChecksumErr                     );
	DEFINE_OSSTATUS( unicodeNoTableErr,                       UnicodeNoTableErr                      );
	DEFINE_OSSTATUS( unicodeVariantErr,                       UnicodeVariantErr                      );
	DEFINE_OSSTATUS( unicodeFallbacksErr,                     UnicodeFallbacksErr                    );
	DEFINE_OSSTATUS( unicodePartConvertErr,                   UnicodePartConvertErr                  );
	DEFINE_OSSTATUS( unicodeBufErr,                           UnicodeBufErr                          );
	DEFINE_OSSTATUS( unicodeCharErr,                          UnicodeCharErr                         );
	DEFINE_OSSTATUS( unicodeElementErr,                       UnicodeElementErr                      );
	DEFINE_OSSTATUS( unicodeNotFoundErr,                      UnicodeNotFoundErr                     );
	DEFINE_OSSTATUS( unicodeTableFormatErr,                   UnicodeTableFormatErr                  );
	DEFINE_OSSTATUS( unicodeDirectionErr,                     UnicodeDirectionErr                    );
	DEFINE_OSSTATUS( unicodeContextualErr,                    UnicodeContextualErr                   );
	DEFINE_OSSTATUS( unicodeTextEncodingDataErr,              UnicodeTextEncodingDataErr             );


/* UTCUtils Status Codes */
	DEFINE_OSSTATUS( kUTCUnderflowErr,                        UTCUnderflowErr                        );
	DEFINE_OSSTATUS( kUTCOverflowErr,                         UTCOverflowErr                         );
	DEFINE_OSSTATUS( kIllegalClockValueErr,                   IllegalClockValueErr                   );


	DEFINE_OSSTATUS( codecErr,                                CodecErr                               );
	DEFINE_OSSTATUS( noCodecErr,                              NoCodecErr                             );
	DEFINE_OSSTATUS( codecUnimpErr,                           CodecUnimpErr                          );
	DEFINE_OSSTATUS( codecSizeErr,                            CodecSizeErr                           );
	DEFINE_OSSTATUS( codecScreenBufErr,                       CodecScreenBufErr                      );
	DEFINE_OSSTATUS( codecImageBufErr,                        CodecImageBufErr                       );
	DEFINE_OSSTATUS( codecSpoolErr,                           CodecSpoolErr                          );
	DEFINE_OSSTATUS( codecAbortErr,                           CodecAbortErr                          );
	DEFINE_OSSTATUS( codecWouldOffscreenErr,                  CodecWouldOffscreenErr                 );
	DEFINE_OSSTATUS( codecBadDataErr,                         CodecBadDataErr                        );
	DEFINE_OSSTATUS( codecDataVersErr,                        CodecDataVersErr                       );
	DEFINE_OSSTATUS( codecExtensionNotFoundErr,               CodecExtensionNotFoundErr              );
	DEFINE_OSSTATUS( scTypeNotFoundErr,                       ScTypeNotFoundErr                      );
	DEFINE_OSSTATUS( codecConditionErr,                       CodecConditionErr                      );
	DEFINE_OSSTATUS( codecOpenErr,                            CodecOpenErr                           );
	DEFINE_OSSTATUS( codecCantWhenErr,                        CodecCantWhenErr                       );
	DEFINE_OSSTATUS( codecCantQueueErr,                       CodecCantQueueErr                      );
	DEFINE_OSSTATUS( codecNothingToBlitErr,                   CodecNothingToBlitErr                  );
	DEFINE_OSSTATUS( codecNoMemoryPleaseWaitErr,              CodecNoMemoryPleaseWaitErr             );
	DEFINE_OSSTATUS( codecDisabledErr,                        CodecDisabledErr                       );
	DEFINE_OSSTATUS( codecNeedToFlushChainErr,                CodecNeedToFlushChainErr               );
	DEFINE_OSSTATUS( lockPortBitsBadSurfaceErr,               LockPortBitsBadSurfaceErr              );
	DEFINE_OSSTATUS( lockPortBitsWindowMovedErr,              LockPortBitsWindowMovedErr             );
	DEFINE_OSSTATUS( lockPortBitsWindowResizedErr,            LockPortBitsWindowResizedErr           );
	DEFINE_OSSTATUS( lockPortBitsWindowClippedErr,            LockPortBitsWindowClippedErr           );
	DEFINE_OSSTATUS( lockPortBitsBadPortErr,                  LockPortBitsBadPortErr                 );
	DEFINE_OSSTATUS( lockPortBitsSurfaceLostErr,              LockPortBitsSurfaceLostErr             );
	DEFINE_OSSTATUS( codecParameterDialogConfirm,             CodecParameterDialogConfirm            );
	DEFINE_OSSTATUS( codecNeedAccessKeyErr,                   CodecNeedAccessKeyErr                  );
	DEFINE_OSSTATUS( codecOffscreenFailedErr,                 CodecOffscreenFailedErr                );
	DEFINE_OSSTATUS( codecDroppedFrameErr,                    CodecDroppedFrameErr                   );
	DEFINE_OSSTATUS( directXObjectAlreadyExists,              DirectXObjectAlreadyExists             );
	DEFINE_OSSTATUS( lockPortBitsWrongGDeviceErr,             LockPortBitsWrongGDeviceErr            );
	DEFINE_OSSTATUS( codecOffscreenFailedPleaseRetryErr,      CodecOffscreenFailedPleaseRetryErr     );


/* PCCard error codes */
	DEFINE_OSSTATUS( kBadAdapterErr,                          BadAdapterErr                          );
	DEFINE_OSSTATUS( kBadAttributeErr,                        BadAttributeErr                        );
	DEFINE_OSSTATUS( kBadBaseErr,                             BadBaseErr                             );
	DEFINE_OSSTATUS( kBadEDCErr,                              BadEDCErr                              );
	DEFINE_OSSTATUS( kBadIRQErr,                              BadIRQErr                              );
	DEFINE_OSSTATUS( kBadOffsetErr,                           BadOffsetErr                           );
	DEFINE_OSSTATUS( kBadPageErr,                             BadPageErr                             );
	DEFINE_OSSTATUS( kBadSizeErr,                             BadSizeErr                             );
	DEFINE_OSSTATUS( kBadSocketErr,                           BadSocketErr                           );
	DEFINE_OSSTATUS( kBadTypeErr,                             BadTypeErr                             );
	DEFINE_OSSTATUS( kBadVccErr,                              BadVccErr                              );
	DEFINE_OSSTATUS( kBadVppErr,                              BadVppErr                              );
	DEFINE_OSSTATUS( kBadWindowErr,                           BadWindowErr                           );
	DEFINE_OSSTATUS( kBadArgLengthErr,                        BadArgLengthErr                        );
	DEFINE_OSSTATUS( kBadArgsErr,                             BadArgsErr                             );
	DEFINE_OSSTATUS( kBadHandleErr,                           BadHandleErr                           );
	DEFINE_OSSTATUS( kBadCISErr,                              BadCISErr                              );
	DEFINE_OSSTATUS( kBadSpeedErr,                            BadSpeedErr                            );
	DEFINE_OSSTATUS( kReadFailureErr,                         ReadFailureErr                         );
	DEFINE_OSSTATUS( kWriteFailureErr,                        WriteFailureErr                        );
	DEFINE_OSSTATUS( kGeneralFailureErr,                      GeneralFailureErr                      );
	DEFINE_OSSTATUS( kNoCardErr,                              NoCardErr                              );
	DEFINE_OSSTATUS( kUnsupportedFunctionErr,                 UnsupportedFunctionErr                 );
	DEFINE_OSSTATUS( kUnsupportedModeErr,                     UnsupportedModeErr                     );
	DEFINE_OSSTATUS( kBusyErr,                                BusyErr                                );
	DEFINE_OSSTATUS( kWriteProtectedErr,                      WriteProtectedErr                      );
	DEFINE_OSSTATUS( kConfigurationLockedErr,                 ConfigurationLockedErr                 );
	DEFINE_OSSTATUS( kInUseErr,                               InUseErr                               );
	DEFINE_OSSTATUS( kNoMoreItemsErr,                         NoMoreItemsErr                         );
	DEFINE_OSSTATUS( kOutOfResourceErr,                       OutOfResourceErr                       );
	DEFINE_OSSTATUS( kNoCardSevicesSocketsErr,                NoCardSevicesSocketsErr                );
	DEFINE_OSSTATUS( kInvalidRegEntryErr,                     InvalidRegEntryErr                     );
	DEFINE_OSSTATUS( kBadLinkErr,                             BadLinkErr                             );
	DEFINE_OSSTATUS( kBadDeviceErr,                           BadDeviceErr                           );
	DEFINE_OSSTATUS( k16BitCardErr,                           K16BitCardErr                          );
	DEFINE_OSSTATUS( kCardBusCardErr,                         CardBusCardErr                         );
	DEFINE_OSSTATUS( kPassCallToChainErr,                     PassCallToChainErr                     );
	DEFINE_OSSTATUS( kCantConfigureCardErr,                   CantConfigureCardErr                   );
	DEFINE_OSSTATUS( kPostCardEventErr,                       PostCardEventErr                       );
	DEFINE_OSSTATUS( kInvalidDeviceNumber,                    InvalidDeviceNumber                    );
	DEFINE_OSSTATUS( kUnsupportedVsErr,                       UnsupportedVsErr                       );
	DEFINE_OSSTATUS( kInvalidCSClientErr,                     InvalidCSClientErr                     );
	DEFINE_OSSTATUS( kBadTupleDataErr,                        BadTupleDataErr                        );
	DEFINE_OSSTATUS( kBadCustomIFIDErr,                       BadCustomIFIDErr                       );
	DEFINE_OSSTATUS( kNoIOWindowRequestedErr,                 NoIOWindowRequestedErr                 );
	DEFINE_OSSTATUS( kNoMoreTimerClientsErr,                  NoMoreTimerClientsErr                  );
	DEFINE_OSSTATUS( kNoMoreInterruptSlotsErr,                NoMoreInterruptSlotsErr                );
	DEFINE_OSSTATUS( kNoClientTableErr,                       NoClientTableErr                       );
	DEFINE_OSSTATUS( kUnsupportedCardErr,                     UnsupportedCardErr                     );
	DEFINE_OSSTATUS( kNoCardEnablersFoundErr,                 NoCardEnablersFoundErr                 );
	DEFINE_OSSTATUS( kNoEnablerForCardErr,                    NoEnablerForCardErr                    );
	DEFINE_OSSTATUS( kNoCompatibleNameErr,                    NoCompatibleNameErr                    );
	DEFINE_OSSTATUS( kClientRequestDenied,                    ClientRequestDenied                    );
	DEFINE_OSSTATUS( kNotReadyErr,                            NotReadyErr                            );
	DEFINE_OSSTATUS( kTooManyIOWindowsErr,                    TooManyIOWindowsErr                    );
	DEFINE_OSSTATUS( kAlreadySavedStateErr,                   AlreadySavedStateErr                   );
	DEFINE_OSSTATUS( kAttemptDupCardEntryErr,                 AttemptDupCardEntryErr                 );
	DEFINE_OSSTATUS( kCardPowerOffErr,                        CardPowerOffErr                        );
	DEFINE_OSSTATUS( kNotZVCapableErr,                        NotZVCapableErr                        );
	DEFINE_OSSTATUS( kNoCardBusCISErr,                        NoCardBusCISErr                        );


	DEFINE_OSSTATUS( noDeviceForChannel,                      NoDeviceForChannel                     );
	DEFINE_OSSTATUS( grabTimeComplete,                        GrabTimeComplete                       );
	DEFINE_OSSTATUS( cantDoThatInCurrentMode,                 CantDoThatInCurrentMode                );
	DEFINE_OSSTATUS( notEnoughMemoryToGrab,                   NotEnoughMemoryToGrab                  );
	DEFINE_OSSTATUS( notEnoughDiskSpaceToGrab,                NotEnoughDiskSpaceToGrab               );
	DEFINE_OSSTATUS( couldntGetRequiredComponent,             CouldntGetRequiredComponent            );
	DEFINE_OSSTATUS( badSGChannel,                            BadSGChannel                           );
	DEFINE_OSSTATUS( seqGrabInfoNotAvailable,                 SeqGrabInfoNotAvailable                );
	DEFINE_OSSTATUS( deviceCantMeetRequest,                   DeviceCantMeetRequest                  );
	DEFINE_OSSTATUS( badControllerHeight,                     BadControllerHeight                    );
	DEFINE_OSSTATUS( editingNotAllowed,                       EditingNotAllowed                      );
	DEFINE_OSSTATUS( controllerBoundsNotExact,                ControllerBoundsNotExact               );
	DEFINE_OSSTATUS( cannotSetWidthOfAttachedController,      CannotSetWidthOfAttachedController     );
	DEFINE_OSSTATUS( controllerHasFixedHeight,                ControllerHasFixedHeight               );
	DEFINE_OSSTATUS( cannotMoveAttachedController,            CannotMoveAttachedController           );

/* AERegistry Errors */
	DEFINE_OSSTATUS( errAEBadKeyForm,                         ErrAEBadKeyForm                        );
	DEFINE_OSSTATUS( errAECantHandleClass,                    ErrAECantHandleClass                   );
	DEFINE_OSSTATUS( errAECantSupplyType,                     ErrAECantSupplyType                    );
	DEFINE_OSSTATUS( errAECantUndo,                           ErrAECantUndo                          );
	DEFINE_OSSTATUS( errAEEventFailed,                        ErrAEEventFailed                       );
	DEFINE_OSSTATUS( errAEIndexTooLarge,                      ErrAEIndexTooLarge                     );
	DEFINE_OSSTATUS( errAEInTransaction,                      ErrAEInTransaction                     );
	DEFINE_OSSTATUS( errAELocalOnly,                          ErrAELocalOnly                         );
	DEFINE_OSSTATUS( errAENoSuchTransaction,                  ErrAENoSuchTransaction                 );
	DEFINE_OSSTATUS( errAENotAnElement,                       ErrAENotAnElement                      );
	DEFINE_OSSTATUS( errAENotASingleObject,                   ErrAENotASingleObject                  );
	DEFINE_OSSTATUS( errAENotModifiable,                      ErrAENotModifiable                     );
	DEFINE_OSSTATUS( errAENoUserSelection,                    ErrAENoUserSelection                   );
	DEFINE_OSSTATUS( errAEPrivilegeError,                     ErrAEPrivilegeError                    );
	DEFINE_OSSTATUS( errAEReadDenied,                         ErrAEReadDenied                        );
	DEFINE_OSSTATUS( errAETypeError,                          ErrAETypeError                         );
	DEFINE_OSSTATUS( errAEWriteDenied,                        ErrAEWriteDenied                       );
	DEFINE_OSSTATUS( errAENotAnEnumMember,                    ErrAENotAnEnumMember                   );
	DEFINE_OSSTATUS( errAECantPutThatThere,                   ErrAECantPutThatThere                  );
	DEFINE_OSSTATUS( errAEPropertiesClash,                    ErrAEPropertiesClash                   );

/* TELErr */
	DEFINE_OSSTATUS( telGenericError,                         TelGenericError                        );
	DEFINE_OSSTATUS( telNoErr,                                TelNoErr                               );
	DEFINE_OSSTATUS( telNoTools,                              TelNoTools                             );
	DEFINE_OSSTATUS( telBadTermErr,                           TelBadTermErr                          );
	DEFINE_OSSTATUS( telBadDNErr,                             TelBadDNErr                            );
	DEFINE_OSSTATUS( telBadCAErr,                             TelBadCAErr                            );
	DEFINE_OSSTATUS( telBadHandErr,                           TelBadHandErr                          );
	DEFINE_OSSTATUS( telBadProcErr,                           TelBadProcErr                          );
	DEFINE_OSSTATUS( telCAUnavail,                            TelCAUnavail                           );
	DEFINE_OSSTATUS( telNoMemErr,                             TelNoMemErr                            );
	DEFINE_OSSTATUS( telNoOpenErr,                            TelNoOpenErr                           );
	DEFINE_OSSTATUS( telBadHTypeErr,                          TelBadHTypeErr                         );
	DEFINE_OSSTATUS( telHTypeNotSupp,                         TelHTypeNotSupp                        );
	DEFINE_OSSTATUS( telBadLevelErr,                          TelBadLevelErr                         );
	DEFINE_OSSTATUS( telBadVTypeErr,                          TelBadVTypeErr                         );
	DEFINE_OSSTATUS( telVTypeNotSupp,                         TelVTypeNotSupp                        );
	DEFINE_OSSTATUS( telBadAPattErr,                          TelBadAPattErr                         );
	DEFINE_OSSTATUS( telAPattNotSupp,                         TelAPattNotSupp                        );
	DEFINE_OSSTATUS( telBadIndex,                             TelBadIndex                            );
	DEFINE_OSSTATUS( telIndexNotSupp,                         TelIndexNotSupp                        );
	DEFINE_OSSTATUS( telBadStateErr,                          TelBadStateErr                         );
	DEFINE_OSSTATUS( telStateNotSupp,                         TelStateNotSupp                        );
	DEFINE_OSSTATUS( telBadIntExt,                            TelBadIntExt                           );
	DEFINE_OSSTATUS( telIntExtNotSupp,                        TelIntExtNotSupp                       );
	DEFINE_OSSTATUS( telBadDNDType,                           TelBadDNDType                          );
	DEFINE_OSSTATUS( telDNDTypeNotSupp,                       TelDNDTypeNotSupp                      );
	DEFINE_OSSTATUS( telFeatNotSub,                           TelFeatNotSub                          );
	DEFINE_OSSTATUS( telFeatNotAvail,                         TelFeatNotAvail                        );
	DEFINE_OSSTATUS( telFeatActive,                           TelFeatActive                          );
	DEFINE_OSSTATUS( telFeatNotSupp,                          TelFeatNotSupp                         );
	DEFINE_OSSTATUS( telConfLimitErr,                         TelConfLimitErr                        );
	DEFINE_OSSTATUS( telConfNoLimit,                          TelConfNoLimit                         );
	DEFINE_OSSTATUS( telConfErr,                              TelConfErr                             );
	DEFINE_OSSTATUS( telConfRej,                              TelConfRej                             );
	DEFINE_OSSTATUS( telTransferErr,                          TelTransferErr                         );
	DEFINE_OSSTATUS( telTransferRej,                          TelTransferRej                         );
	DEFINE_OSSTATUS( telCBErr,                                TelCBErr                               );
	DEFINE_OSSTATUS( telConfLimitExceeded,                    TelConfLimitExceeded                   );
	DEFINE_OSSTATUS( telBadDNType,                            TelBadDNType                           );
	DEFINE_OSSTATUS( telBadPageID,                            TelBadPageID                           );
	DEFINE_OSSTATUS( telBadIntercomID,                        TelBadIntercomID                       );
	DEFINE_OSSTATUS( telBadFeatureID,                         TelBadFeatureID                        );
	DEFINE_OSSTATUS( telBadFwdType,                           TelBadFwdType                          );
	DEFINE_OSSTATUS( telBadPickupGroupID,                     TelBadPickupGroupID                    );
	DEFINE_OSSTATUS( telBadParkID,                            TelBadParkID                           );
	DEFINE_OSSTATUS( telBadSelect,                            TelBadSelect                           );
	DEFINE_OSSTATUS( telBadBearerType,                        TelBadBearerType                       );
	DEFINE_OSSTATUS( telBadRate,                              TelBadRate                             );
	DEFINE_OSSTATUS( telDNTypeNotSupp,                        TelDNTypeNotSupp                       );
	DEFINE_OSSTATUS( telFwdTypeNotSupp,                       TelFwdTypeNotSupp                      );
	DEFINE_OSSTATUS( telBadDisplayMode,                       TelBadDisplayMode                      );
	DEFINE_OSSTATUS( telDisplayModeNotSupp,                   TelDisplayModeNotSupp                  );
	DEFINE_OSSTATUS( telNoCallbackRef,                        TelNoCallbackRef                       );
	DEFINE_OSSTATUS( telAlreadyOpen,                          TelAlreadyOpen                         );
	DEFINE_OSSTATUS( telStillNeeded,                          TelStillNeeded                         );
	DEFINE_OSSTATUS( telTermNotOpen,                          TelTermNotOpen                         );
	DEFINE_OSSTATUS( telCANotAcceptable,                      TelCANotAcceptable                     );
	DEFINE_OSSTATUS( telCANotRejectable,                      TelCANotRejectable                     );
	DEFINE_OSSTATUS( telCANotDeflectable,                     TelCANotDeflectable                    );
	DEFINE_OSSTATUS( telPBErr,                                TelPBErr                               );
	DEFINE_OSSTATUS( telBadFunction,                          TelBadFunction                         );
	DEFINE_OSSTATUS( telNoSuchTool,                           TelNoSuchTool                          );
	DEFINE_OSSTATUS( telUnknownErr,                           TelUnknownErr                          );
	DEFINE_OSSTATUS( telNoCommFolder,                         TelNoCommFolder                        );
	DEFINE_OSSTATUS( telInitFailed,                           TelInitFailed                          );
	DEFINE_OSSTATUS( telBadCodeResource,                      TelBadCodeResource                     );
	DEFINE_OSSTATUS( telDeviceNotFound,                       TelDeviceNotFound                      );
	DEFINE_OSSTATUS( telBadProcID,                            TelBadProcID                           );
	DEFINE_OSSTATUS( telValidateFailed,                       TelValidateFailed                      );
	DEFINE_OSSTATUS( telAutoAnsNotOn,                         TelAutoAnsNotOn                        );
	DEFINE_OSSTATUS( telDetAlreadyOn,                         TelDetAlreadyOn                        );
	DEFINE_OSSTATUS( telBadSWErr,                             TelBadSWErr                            );
	DEFINE_OSSTATUS( telBadSampleRate,                        TelBadSampleRate                       );
	DEFINE_OSSTATUS( telNotEnoughdspBW,                       TelNotEnoughdspBW                      );

	DEFINE_OSSTATUS( errTaskNotFound,                         ErrTaskNotFound                        );

/*Power Manager Errors*/
	DEFINE_OSSTATUS( pmBusyErr,                               PMBusyErr                              );
	DEFINE_OSSTATUS( pmReplyTOErr,                            PMReplyTOErr                           );
	DEFINE_OSSTATUS( pmSendStartErr,                          PMSendStartErr                         );
	DEFINE_OSSTATUS( pmSendEndErr,                            PMSendEndErr                           );
	DEFINE_OSSTATUS( pmRecvStartErr,                          PMRecvStartErr                         );
	DEFINE_OSSTATUS( pmRecvEndErr,                            PMRecvEndErr                           );

/*Power Manager 2.0 Errors*/
	DEFINE_OSSTATUS( kPowerHandlerExistsForDeviceErr,         PowerHandlerExistsForDeviceErr         );
	DEFINE_OSSTATUS( kPowerHandlerNotFoundForDeviceErr,       PowerHandlerNotFoundForDeviceErr       );
	DEFINE_OSSTATUS( kPowerHandlerNotFoundForProcErr,         PowerHandlerNotFoundForProcErr         );
	DEFINE_OSSTATUS( kPowerMgtMessageNotHandled,              PowerMgtMessageNotHandled              );
	DEFINE_OSSTATUS( kPowerMgtRequestDenied,                  PowerMgtRequestDenied                  );
	DEFINE_OSSTATUS( kCantReportProcessorTemperatureErr,      CantReportProcessorTemperatureErr      );
	DEFINE_OSSTATUS( kProcessorTempRoutineRequiresMPLib2,     ProcessorTempRoutineRequiresMPLib2     );
	DEFINE_OSSTATUS( kNoSuchPowerSource,                      NoSuchPowerSource                      );
	DEFINE_OSSTATUS( kBridgeSoftwareRunningCantSleep,         BridgeSoftwareRunningCantSleep         );


/* Debugging library errors */
	DEFINE_OSSTATUS( debuggingExecutionContextErr,            DebuggingExecutionContextErr           );
	DEFINE_OSSTATUS( debuggingDuplicateSignatureErr,          DebuggingDuplicateSignatureErr         );
	DEFINE_OSSTATUS( debuggingDuplicateOptionErr,             DebuggingDuplicateOptionErr            );
	DEFINE_OSSTATUS( debuggingInvalidSignatureErr,            DebuggingInvalidSignatureErr           );
	DEFINE_OSSTATUS( debuggingInvalidOptionErr,               DebuggingInvalidOptionErr              );
	DEFINE_OSSTATUS( debuggingInvalidNameErr,                 DebuggingInvalidNameErr                );
	DEFINE_OSSTATUS( debuggingNoCallbackErr,                  DebuggingNoCallbackErr                 );
	DEFINE_OSSTATUS( debuggingNoMatchErr,                     DebuggingNoMatchErr                    );


/* HID device driver error codes */
	DEFINE_OSSTATUS( kHIDVersionIncompatibleErr,              HIDVersionIncompatibleErr              );
	DEFINE_OSSTATUS( kHIDDeviceNotReady,                      HIDDeviceNotReady                      );


/* HID error codes */
	DEFINE_OSSTATUS( kHIDSuccess,                             HIDSuccess                             );
	DEFINE_OSSTATUS( kHIDInvalidRangePageErr,                 HIDInvalidRangePageErr                 );
	DEFINE_OSSTATUS( kHIDReportIDZeroErr,                     HIDReportIDZeroErr                     );
	DEFINE_OSSTATUS( kHIDReportCountZeroErr,                  HIDReportCountZeroErr                  );
	DEFINE_OSSTATUS( kHIDReportSizeZeroErr,                   HIDReportSizeZeroErr                   );
	DEFINE_OSSTATUS( kHIDUnmatchedDesignatorRangeErr,         HIDUnmatchedDesignatorRangeErr         );
	DEFINE_OSSTATUS( kHIDUnmatchedStringRangeErr,             HIDUnmatchedStringRangeErr             );
	DEFINE_OSSTATUS( kHIDInvertedUsageRangeErr,               HIDInvertedUsageRangeErr               );
	DEFINE_OSSTATUS( kHIDUnmatchedUsageRangeErr,              HIDUnmatchedUsageRangeErr              );
	DEFINE_OSSTATUS( kHIDInvertedPhysicalRangeErr,            HIDInvertedPhysicalRangeErr            );
	DEFINE_OSSTATUS( kHIDInvertedLogicalRangeErr,             HIDInvertedLogicalRangeErr             );
	DEFINE_OSSTATUS( kHIDBadLogicalMaximumErr,                HIDBadLogicalMaximumErr                );
	DEFINE_OSSTATUS( kHIDBadLogicalMinimumErr,                HIDBadLogicalMinimumErr                );
	DEFINE_OSSTATUS( kHIDUsagePageZeroErr,                    HIDUsagePageZeroErr                    );
	DEFINE_OSSTATUS( kHIDEndOfDescriptorErr,                  HIDEndOfDescriptorErr                  );
	DEFINE_OSSTATUS( kHIDNotEnoughMemoryErr,                  HIDNotEnoughMemoryErr                  );
	DEFINE_OSSTATUS( kHIDBadParameterErr,                     HIDBadParameterErr                     );
	DEFINE_OSSTATUS( kHIDNullPointerErr,                      HIDNullPointerErr                      );
	DEFINE_OSSTATUS( kHIDInvalidReportLengthErr,              HIDInvalidReportLengthErr              );
	DEFINE_OSSTATUS( kHIDInvalidReportTypeErr,                HIDInvalidReportTypeErr                );
	DEFINE_OSSTATUS( kHIDBadLogPhysValuesErr,                 HIDBadLogPhysValuesErr                 );
	DEFINE_OSSTATUS( kHIDIncompatibleReportErr,               HIDIncompatibleReportErr               );
	DEFINE_OSSTATUS( kHIDInvalidPreparsedDataErr,             HIDInvalidPreparsedDataErr             );
	DEFINE_OSSTATUS( kHIDNotValueArrayErr,                    HIDNotValueArrayErr                    );
	DEFINE_OSSTATUS( kHIDUsageNotFoundErr,                    HIDUsageNotFoundErr                    );
	DEFINE_OSSTATUS( kHIDValueOutOfRangeErr,                  HIDValueOutOfRangeErr                  );
	DEFINE_OSSTATUS( kHIDBufferTooSmallErr,                   HIDBufferTooSmallErr                   );
	DEFINE_OSSTATUS( kHIDNullStateErr,                        HIDNullStateErr                        );
	DEFINE_OSSTATUS( kHIDBaseError,                           HIDBaseError                           );


/* the OT modem module may return the following error codes:*/
	DEFINE_OSSTATUS( kModemOutOfMemory,                       ModemOutOfMemory                       );
	DEFINE_OSSTATUS( kModemPreferencesMissing,                ModemPreferencesMissing                );
	DEFINE_OSSTATUS( kModemScriptMissing,                     ModemScriptMissing                     );



/* MacTextEditor error codes */
	DEFINE_OSSTATUS( kTXNEndIterationErr,                     TXNEndIterationErr                     );
	DEFINE_OSSTATUS( kTXNCannotAddFrameErr,                   TXNCannotAddFrameErr                   );
	DEFINE_OSSTATUS( kTXNInvalidFrameIDErr,                   TXNInvalidFrameIDErr                   );
	DEFINE_OSSTATUS( kTXNIllegalToCrossDataBoundariesErr,     TXNIllegalToCrossDataBoundariesErr     );
	DEFINE_OSSTATUS( kTXNUserCanceledOperationErr,            TXNUserCanceledOperationErr            );
	DEFINE_OSSTATUS( kTXNBadDefaultFileTypeWarning,           TXNBadDefaultFileTypeWarning           );
	DEFINE_OSSTATUS( kTXNCannotSetAutoIndentErr,              TXNCannotSetAutoIndentErr              );
	DEFINE_OSSTATUS( kTXNRunIndexOutofBoundsErr,              TXNRunIndexOutofBoundsErr              );
	DEFINE_OSSTATUS( kTXNNoMatchErr,                          TXNNoMatchErr                          );
	DEFINE_OSSTATUS( kTXNAttributeTagInvalidForRunErr,        TXNAttributeTagInvalidForRunErr        );
	DEFINE_OSSTATUS( kTXNSomeOrAllTagsInvalidForRunErr,       TXNSomeOrAllTagsInvalidForRunErr       );
	DEFINE_OSSTATUS( kTXNInvalidRunIndex,                     TXNInvalidRunIndex                     );
	DEFINE_OSSTATUS( kTXNAlreadyInitializedErr,               TXNAlreadyInitializedErr               );
	DEFINE_OSSTATUS( kTXNCannotTurnTSMOffWhenUsingUnicodeErr, TXNCannotTurnTSMOffWhenUsingUnicodeErr );
	DEFINE_OSSTATUS( kTXNCopyNotAllowedInEchoModeErr,         TXNCopyNotAllowedInEchoModeErr         );
	DEFINE_OSSTATUS( kTXNDataTypeNotAllowedErr,               TXNDataTypeNotAllowedErr               );
	DEFINE_OSSTATUS( kTXNATSUIIsNotInstalledErr,              TXNATSUIIsNotInstalledErr              );
	DEFINE_OSSTATUS( kTXNOutsideOfLineErr,                    TXNOutsideOfLineErr                    );
	DEFINE_OSSTATUS( kTXNOutsideOfFrameErr,                   TXNOutsideOfFrameErr                   );




/*Possible errors from the PrinterStatus bottleneck*/
	DEFINE_OSSTATUS( printerStatusOpCodeNotSupportedErr,      PrinterStatusOpCodeNotSupportedErr     );


/* Keychain Manager error codes */
	DEFINE_OSSTATUS( errKCNotAvailable,                       ErrKCNotAvailable                      );
	DEFINE_OSSTATUS( errKCReadOnly,                           ErrKCReadOnly                          );
	DEFINE_OSSTATUS( errKCAuthFailed,                         ErrKCAuthFailed                        );
	DEFINE_OSSTATUS( errKCNoSuchKeychain,                     ErrKCNoSuchKeychain                    );
	DEFINE_OSSTATUS( errKCInvalidKeychain,                    ErrKCInvalidKeychain                   );
	DEFINE_OSSTATUS( errKCDuplicateKeychain,                  ErrKCDuplicateKeychain                 );
	DEFINE_OSSTATUS( errKCDuplicateCallback,                  ErrKCDuplicateCallback                 );
	DEFINE_OSSTATUS( errKCInvalidCallback,                    ErrKCInvalidCallback                   );
	DEFINE_OSSTATUS( errKCDuplicateItem,                      ErrKCDuplicateItem                     );
	DEFINE_OSSTATUS( errKCItemNotFound,                       ErrKCItemNotFound                      );
	DEFINE_OSSTATUS( errKCBufferTooSmall,                     ErrKCBufferTooSmall                    );
	DEFINE_OSSTATUS( errKCDataTooLarge,                       ErrKCDataTooLarge                      );
	DEFINE_OSSTATUS( errKCNoSuchAttr,                         ErrKCNoSuchAttr                        );
	DEFINE_OSSTATUS( errKCInvalidItemRef,                     ErrKCInvalidItemRef                    );
	DEFINE_OSSTATUS( errKCInvalidSearchRef,                   ErrKCInvalidSearchRef                  );
	DEFINE_OSSTATUS( errKCNoSuchClass,                        ErrKCNoSuchClass                       );
	DEFINE_OSSTATUS( errKCNoDefaultKeychain,                  ErrKCNoDefaultKeychain                 );
	DEFINE_OSSTATUS( errKCInteractionNotAllowed,              ErrKCInteractionNotAllowed             );
	DEFINE_OSSTATUS( errKCReadOnlyAttr,                       ErrKCReadOnlyAttr                      );
	DEFINE_OSSTATUS( errKCWrongKCVersion,                     ErrKCWrongKCVersion                    );
	DEFINE_OSSTATUS( errKCKeySizeNotAllowed,                  ErrKCKeySizeNotAllowed                 );
	DEFINE_OSSTATUS( errKCNoStorageModule,                    ErrKCNoStorageModule                   );
	DEFINE_OSSTATUS( errKCNoCertificateModule,                ErrKCNoCertificateModule               );
	DEFINE_OSSTATUS( errKCNoPolicyModule,                     ErrKCNoPolicyModule                    );
	DEFINE_OSSTATUS( errKCInteractionRequired,                ErrKCInteractionRequired               );
	DEFINE_OSSTATUS( errKCDataNotAvailable,                   ErrKCDataNotAvailable                  );
	DEFINE_OSSTATUS( errKCDataNotModifiable,                  ErrKCDataNotModifiable                 );
	DEFINE_OSSTATUS( errKCCreateChainFailed,                  ErrKCCreateChainFailed                 );


/* UnicodeUtilities error & status codes*/
	DEFINE_OSSTATUS( kUCOutputBufferTooSmall,                 UCOutputBufferTooSmall                 );
	DEFINE_OSSTATUS( kUCTextBreakLocatorMissingType,          UCTextBreakLocatorMissingType          );

/* Multiprocessing API error codes*/
	DEFINE_OSSTATUS( kMPIterationEndErr,                      MPIterationEndErr                      );
	DEFINE_OSSTATUS( kMPPrivilegedErr,                        MPPrivilegedErr                        );
	DEFINE_OSSTATUS( kMPProcessCreatedErr,                    MPProcessCreatedErr                    );
	DEFINE_OSSTATUS( kMPProcessTerminatedErr,                 MPProcessTerminatedErr                 );
	DEFINE_OSSTATUS( kMPTaskCreatedErr,                       MPTaskCreatedErr                       );
	DEFINE_OSSTATUS( kMPTaskBlockedErr,                       MPTaskBlockedErr                       );
	DEFINE_OSSTATUS( kMPTaskStoppedErr,                       MPTaskStoppedErr                       );
	DEFINE_OSSTATUS( kMPBlueBlockingErr,                      MPBlueBlockingErr                      );
	DEFINE_OSSTATUS( kMPDeletedErr,                           MPDeletedErr                           );
	DEFINE_OSSTATUS( kMPTimeoutErr,                           MPTimeoutErr                           );
	DEFINE_OSSTATUS( kMPTaskAbortedErr,                       MPTaskAbortedErr                       );
	DEFINE_OSSTATUS( kMPInsufficientResourcesErr,             MPInsufficientResourcesErr             );
	DEFINE_OSSTATUS( kMPInvalidIDErr,                         MPInvalidIDErr                         );

/* StringCompare error codes (in TextUtils range)*/
	DEFINE_OSSTATUS( kCollateAttributesNotFoundErr,           CollateAttributesNotFoundErr           );
	DEFINE_OSSTATUS( kCollateInvalidOptions,                  CollateInvalidOptions                  );
	DEFINE_OSSTATUS( kCollateMissingUnicodeTableErr,          CollateMissingUnicodeTableErr          );
	DEFINE_OSSTATUS( kCollateUnicodeConvertFailedErr,         CollateUnicodeConvertFailedErr         );
	DEFINE_OSSTATUS( kCollatePatternNotFoundErr,              CollatePatternNotFoundErr              );
	DEFINE_OSSTATUS( kCollateInvalidChar,                     CollateInvalidChar                     );
	DEFINE_OSSTATUS( kCollateBufferTooSmall,                  CollateBufferTooSmall                  );
	DEFINE_OSSTATUS( kCollateInvalidCollationRef,             CollateInvalidCollationRef             );


/* FontSync OSStatus Codes */
	DEFINE_OSSTATUS( kFNSInvalidReferenceErr,                 FNSInvalidReferenceErr                 );
	DEFINE_OSSTATUS( kFNSBadReferenceVersionErr,              FNSBadReferenceVersionErr              );
	DEFINE_OSSTATUS( kFNSInvalidProfileErr,                   FNSInvalidProfileErr                   );
	DEFINE_OSSTATUS( kFNSBadProfileVersionErr,                FNSBadProfileVersionErr                );
	DEFINE_OSSTATUS( kFNSDuplicateReferenceErr,               FNSDuplicateReferenceErr               );
	DEFINE_OSSTATUS( kFNSMismatchErr,                         FNSMismatchErr                         );
	DEFINE_OSSTATUS( kFNSInsufficientDataErr,                 FNSInsufficientDataErr                 );
	DEFINE_OSSTATUS( kFNSBadFlattenedSizeErr,                 FNSBadFlattenedSizeErr                 );
	DEFINE_OSSTATUS( kFNSNameNotFoundErr,                     FNSNameNotFoundErr                     );



/* MacLocales error codes*/
	DEFINE_OSSTATUS( kLocalesBufferTooSmallErr,               LocalesBufferTooSmallErr               );
	DEFINE_OSSTATUS( kLocalesTableFormatErr,                  LocalesTableFormatErr                  );
	DEFINE_OSSTATUS( kLocalesDefaultDisplayStatus,            LocalesDefaultDisplayStatus            );


/* Settings Manager (formerly known as Location Manager) Errors */
	DEFINE_OSSTATUS( kALMInternalErr,                         ALMInternalErr                         );
	DEFINE_OSSTATUS( kALMGroupNotFoundErr,                    ALMGroupNotFoundErr                    );
	DEFINE_OSSTATUS( kALMNoSuchModuleErr,                     ALMNoSuchModuleErr                     );
	DEFINE_OSSTATUS( kALMModuleCommunicationErr,              ALMModuleCommunicationErr              );
	DEFINE_OSSTATUS( kALMDuplicateModuleErr,                  ALMDuplicateModuleErr                  );
	DEFINE_OSSTATUS( kALMInstallationErr,                     ALMInstallationErr                     );
	DEFINE_OSSTATUS( kALMDeferSwitchErr,                      ALMDeferSwitchErr                      );
	DEFINE_OSSTATUS( kALMRebootFlagsLevelErr,                 ALMRebootFlagsLevelErr                 );

	DEFINE_OSSTATUS( kALMLocationNotFoundErr,                 ALMLocationNotFoundErr                 );


/* SoundSprocket Error Codes */
	DEFINE_OSSTATUS( kSSpInternalErr,                         SSpInternalErr                         );
	DEFINE_OSSTATUS( kSSpVersionErr,                          SSpVersionErr                          );
	DEFINE_OSSTATUS( kSSpCantInstallErr,                      SSpCantInstallErr                      );
	DEFINE_OSSTATUS( kSSpParallelUpVectorErr,                 SSpParallelUpVectorErr                 );
	DEFINE_OSSTATUS( kSSpScaleToZeroErr,                      SSpScaleToZeroErr                      );


/* NetSprocket Error Codes */
	DEFINE_OSSTATUS( kNSpInitializationFailedErr,             NSpInitializationFailedErr             );
	DEFINE_OSSTATUS( kNSpAlreadyInitializedErr,               NSpAlreadyInitializedErr               );
	DEFINE_OSSTATUS( kNSpTopologyNotSupportedErr,             NSpTopologyNotSupportedErr             );
	DEFINE_OSSTATUS( kNSpPipeFullErr,                         NSpPipeFullErr                         );
	DEFINE_OSSTATUS( kNSpHostFailedErr,                       NSpHostFailedErr                       );
	DEFINE_OSSTATUS( kNSpProtocolNotAvailableErr,             NSpProtocolNotAvailableErr             );
	DEFINE_OSSTATUS( kNSpInvalidGameRefErr,                   NSpInvalidGameRefErr                   );
	DEFINE_OSSTATUS( kNSpInvalidParameterErr,                 NSpInvalidParameterErr                 );
	DEFINE_OSSTATUS( kNSpOTNotPresentErr,                     NSpOTNotPresentErr                     );
	DEFINE_OSSTATUS( kNSpOTVersionTooOldErr,                  NSpOTVersionTooOldErr                  );
	DEFINE_OSSTATUS( kNSpMemAllocationErr,                    NSpMemAllocationErr                    );
	DEFINE_OSSTATUS( kNSpAlreadyAdvertisingErr,               NSpAlreadyAdvertisingErr               );
	DEFINE_OSSTATUS( kNSpNotAdvertisingErr,                   NSpNotAdvertisingErr                   );
	DEFINE_OSSTATUS( kNSpInvalidAddressErr,                   NSpInvalidAddressErr                   );
	DEFINE_OSSTATUS( kNSpFreeQExhaustedErr,                   NSpFreeQExhaustedErr                   );
	DEFINE_OSSTATUS( kNSpRemovePlayerFailedErr,               NSpRemovePlayerFailedErr               );
	DEFINE_OSSTATUS( kNSpAddressInUseErr,                     NSpAddressInUseErr                     );
	DEFINE_OSSTATUS( kNSpFeatureNotImplementedErr,            NSpFeatureNotImplementedErr            );
	DEFINE_OSSTATUS( kNSpNameRequiredErr,                     NSpNameRequiredErr                     );
	DEFINE_OSSTATUS( kNSpInvalidPlayerIDErr,                  NSpInvalidPlayerIDErr                  );
	DEFINE_OSSTATUS( kNSpInvalidGroupIDErr,                   NSpInvalidGroupIDErr                   );
	DEFINE_OSSTATUS( kNSpNoPlayersErr,                        NSpNoPlayersErr                        );
	DEFINE_OSSTATUS( kNSpNoGroupsErr,                         NSpNoGroupsErr                         );
	DEFINE_OSSTATUS( kNSpNoHostVolunteersErr,                 NSpNoHostVolunteersErr                 );
	DEFINE_OSSTATUS( kNSpCreateGroupFailedErr,                NSpCreateGroupFailedErr                );
	DEFINE_OSSTATUS( kNSpAddPlayerFailedErr,                  NSpAddPlayerFailedErr                  );
	DEFINE_OSSTATUS( kNSpInvalidDefinitionErr,                NSpInvalidDefinitionErr                );
	DEFINE_OSSTATUS( kNSpInvalidProtocolRefErr,               NSpInvalidProtocolRefErr               );
	DEFINE_OSSTATUS( kNSpInvalidProtocolListErr,              NSpInvalidProtocolListErr              );
	DEFINE_OSSTATUS( kNSpTimeoutErr,                          NSpTimeoutErr                          );
	DEFINE_OSSTATUS( kNSpGameTerminatedErr,                   NSpGameTerminatedErr                   );
	DEFINE_OSSTATUS( kNSpConnectFailedErr,                    NSpConnectFailedErr                    );
	DEFINE_OSSTATUS( kNSpSendFailedErr,                       NSpSendFailedErr                       );
	DEFINE_OSSTATUS( kNSpMessageTooBigErr,                    NSpMessageTooBigErr                    );
	DEFINE_OSSTATUS( kNSpCantBlockErr,                        NSpCantBlockErr                        );
	DEFINE_OSSTATUS( kNSpJoinFailedErr,                       NSpJoinFailedErr                       );


/* InputSprockets error codes */
	DEFINE_OSSTATUS( kISpInternalErr,                         ISpInternalErr                         );
	DEFINE_OSSTATUS( kISpSystemListErr,                       ISpSystemListErr                       );
	DEFINE_OSSTATUS( kISpBufferToSmallErr,                    ISpBufferToSmallErr                    );
	DEFINE_OSSTATUS( kISpElementInListErr,                    ISpElementInListErr                    );
	DEFINE_OSSTATUS( kISpElementNotInListErr,                 ISpElementNotInListErr                 );
	DEFINE_OSSTATUS( kISpSystemInactiveErr,                   ISpSystemInactiveErr                   );
	DEFINE_OSSTATUS( kISpDeviceInactiveErr,                   ISpDeviceInactiveErr                   );
	DEFINE_OSSTATUS( kISpSystemActiveErr,                     ISpSystemActiveErr                     );
	DEFINE_OSSTATUS( kISpDeviceActiveErr,                     ISpDeviceActiveErr                     );
	DEFINE_OSSTATUS( kISpListBusyErr,                         ISpListBusyErr                         );

/* DrawSprockets error/warning codes */
	DEFINE_OSSTATUS( kDSpNotInitializedErr,                   DSpNotInitializedErr                   );
	DEFINE_OSSTATUS( kDSpSystemSWTooOldErr,                   DSpSystemSWTooOldErr                   );
	DEFINE_OSSTATUS( kDSpInvalidContextErr,                   DSpInvalidContextErr                   );
	DEFINE_OSSTATUS( kDSpInvalidAttributesErr,                DSpInvalidAttributesErr                );
	DEFINE_OSSTATUS( kDSpContextAlreadyReservedErr,           DSpContextAlreadyReservedErr           );
	DEFINE_OSSTATUS( kDSpContextNotReservedErr,               DSpContextNotReservedErr               );
	DEFINE_OSSTATUS( kDSpContextNotFoundErr,                  DSpContextNotFoundErr                  );
	DEFINE_OSSTATUS( kDSpFrameRateNotReadyErr,                DSpFrameRateNotReadyErr                );
	DEFINE_OSSTATUS( kDSpConfirmSwitchWarning,                DSpConfirmSwitchWarning                );
	DEFINE_OSSTATUS( kDSpInternalErr,                         DSpInternalErr                         );
	DEFINE_OSSTATUS( kDSpStereoContextErr,                    DSpStereoContextErr                    );


/* Find By Content errors */
	DEFINE_OSSTATUS( kFBCvTwinExceptionErr,                   FBCvTwinExceptionErr                   );
	DEFINE_OSSTATUS( kFBCnoIndexesFound,                      FBCnoIndexesFound                      );
	DEFINE_OSSTATUS( kFBCallocFailed,                         FBCallocFailed                         );
	DEFINE_OSSTATUS( kFBCbadParam,                            FBCbadParam                            );
	DEFINE_OSSTATUS( kFBCfileNotIndexed,                      FBCfileNotIndexed                      );
	DEFINE_OSSTATUS( kFBCbadIndexFile,                        FBCbadIndexFile                        );
	DEFINE_OSSTATUS( kFBCcompactionFailed,                    FBCcompactionFailed                    );
	DEFINE_OSSTATUS( kFBCvalidationFailed,                    FBCvalidationFailed                    );
	DEFINE_OSSTATUS( kFBCindexingFailed,                      FBCindexingFailed                      );
	DEFINE_OSSTATUS( kFBCcommitFailed,                        FBCcommitFailed                        );
	DEFINE_OSSTATUS( kFBCdeletionFailed,                      FBCdeletionFailed                      );
	DEFINE_OSSTATUS( kFBCmoveFailed,                          FBCmoveFailed                          );
	DEFINE_OSSTATUS( kFBCtokenizationFailed,                  FBCtokenizationFailed                  );
	DEFINE_OSSTATUS( kFBCmergingFailed,                       FBCmergingFailed                       );
	DEFINE_OSSTATUS( kFBCindexCreationFailed,                 FBCindexCreationFailed                 );
	DEFINE_OSSTATUS( kFBCaccessorStoreFailed,                 FBCaccessorStoreFailed                 );
	DEFINE_OSSTATUS( kFBCaddDocFailed,                        FBCaddDocFailed                        );
	DEFINE_OSSTATUS( kFBCflushFailed,                         FBCflushFailed                         );
	DEFINE_OSSTATUS( kFBCindexNotFound,                       FBCindexNotFound                       );
	DEFINE_OSSTATUS( kFBCnoSearchSession,                     FBCnoSearchSession                     );
	DEFINE_OSSTATUS( kFBCindexingCanceled,                    FBCindexingCanceled                    );
	DEFINE_OSSTATUS( kFBCaccessCanceled,                      FBCaccessCanceled                      );
	DEFINE_OSSTATUS( kFBCindexFileDestroyed,                  FBCindexFileDestroyed                  );
	DEFINE_OSSTATUS( kFBCindexNotAvailable,                   FBCindexNotAvailable                   );
	DEFINE_OSSTATUS( kFBCsearchFailed,                        FBCsearchFailed                        );
	DEFINE_OSSTATUS( kFBCsomeFilesNotIndexed,                 FBCsomeFilesNotIndexed                 );
	DEFINE_OSSTATUS( kFBCillegalSessionChange,                FBCillegalSessionChange                );
	DEFINE_OSSTATUS( kFBCanalysisNotAvailable,                FBCanalysisNotAvailable                );
	DEFINE_OSSTATUS( kFBCbadIndexFileVersion,                 FBCbadIndexFileVersion                 );
	DEFINE_OSSTATUS( kFBCsummarizationCanceled,               FBCsummarizationCanceled               );
	DEFINE_OSSTATUS( kFBCindexDiskIOFailed,                   FBCindexDiskIOFailed                   );
	DEFINE_OSSTATUS( kFBCbadSearchSession,                    FBCbadSearchSession                    );
	DEFINE_OSSTATUS( kFBCnoSuchHit,                           FBCnoSuchHit                           );


/* QuickTime VR Errors */
	DEFINE_OSSTATUS( notAQTVRMovieErr,                        NotAQTVRMovieErr                       );
	DEFINE_OSSTATUS( constraintReachedErr,                    ConstraintReachedErr                   );
	DEFINE_OSSTATUS( callNotSupportedByNodeErr,               CallNotSupportedByNodeErr              );
	DEFINE_OSSTATUS( selectorNotSupportedByNodeErr,           SelectorNotSupportedByNodeErr          );
	DEFINE_OSSTATUS( invalidNodeIDErr,                        InvalidNodeIDErr                       );
	DEFINE_OSSTATUS( invalidViewStateErr,                     InvalidViewStateErr                    );
	DEFINE_OSSTATUS( timeNotInViewErr,                        TimeNotInViewErr                       );
	DEFINE_OSSTATUS( propertyNotSupportedByNodeErr,           PropertyNotSupportedByNodeErr          );
	DEFINE_OSSTATUS( settingNotSupportedByNodeErr,            SettingNotSupportedByNodeErr           );
	DEFINE_OSSTATUS( limitReachedErr,                         LimitReachedErr                        );
	DEFINE_OSSTATUS( invalidNodeFormatErr,                    InvalidNodeFormatErr                   );
	DEFINE_OSSTATUS( invalidHotSpotIDErr,                     InvalidHotSpotIDErr                    );
	DEFINE_OSSTATUS( noMemoryNodeFailedInitialize,            NoMemoryNodeFailedInitialize           );
	DEFINE_OSSTATUS( streamingNodeNotReadyErr,                StreamingNodeNotReadyErr               );
	DEFINE_OSSTATUS( qtvrLibraryLoadErr,                      QTVRLibraryLoadErr                     );
	DEFINE_OSSTATUS( qtvrUninitialized,                       QTVRUninitialized                      );


/* Appearance Manager Error Codes */
	DEFINE_OSSTATUS( themeInvalidBrushErr,                    ThemeInvalidBrushErr                   );
	DEFINE_OSSTATUS( themeProcessRegisteredErr,               ThemeProcessRegisteredErr              );
	DEFINE_OSSTATUS( themeProcessNotRegisteredErr,            ThemeProcessNotRegisteredErr           );
	DEFINE_OSSTATUS( themeBadTextColorErr,                    ThemeBadTextColorErr                   );
	DEFINE_OSSTATUS( themeHasNoAccentsErr,                    ThemeHasNoAccentsErr                   );
	DEFINE_OSSTATUS( themeBadCursorIndexErr,                  ThemeBadCursorIndexErr                 );
	DEFINE_OSSTATUS( themeScriptFontNotFoundErr,              ThemeScriptFontNotFoundErr             );
	DEFINE_OSSTATUS( themeMonitorDepthNotSupportedErr,        ThemeMonitorDepthNotSupportedErr       );

/* Control Manager Error Codes */
	DEFINE_OSSTATUS( errMessageNotSupported,                  ErrMessageNotSupported                 );
	DEFINE_OSSTATUS( errDataNotSupported,                     ErrDataNotSupported                    );
	DEFINE_OSSTATUS( errControlDoesntSupportFocus,            ErrControlDoesntSupportFocus           );
	DEFINE_OSSTATUS( errUnknownControl,                       ErrUnknownControl                      );
	DEFINE_OSSTATUS( errCouldntSetFocus,                      ErrCouldntSetFocus                     );
	DEFINE_OSSTATUS( errNoRootControl,                        ErrNoRootControl                       );
	DEFINE_OSSTATUS( errRootAlreadyExists,                    ErrRootAlreadyExists                   );
	DEFINE_OSSTATUS( errInvalidPartCode,                      ErrInvalidPartCode                     );
	DEFINE_OSSTATUS( errControlsAlreadyExist,                 ErrControlsAlreadyExist                );
	DEFINE_OSSTATUS( errControlIsNotEmbedder,                 ErrControlIsNotEmbedder                );
	DEFINE_OSSTATUS( errDataSizeMismatch,                     ErrDataSizeMismatch                    );
	DEFINE_OSSTATUS( errControlHiddenOrDisabled,              ErrControlHiddenOrDisabled             );
	DEFINE_OSSTATUS( errCantEmbedIntoSelf,                    ErrCantEmbedIntoSelf                   );
	DEFINE_OSSTATUS( errCantEmbedRoot,                        ErrCantEmbedRoot                       );
	DEFINE_OSSTATUS( errItemNotControl,                       ErrItemNotControl                      );
	DEFINE_OSSTATUS( controlInvalidDataVersionErr,            ControlInvalidDataVersionErr           );
	DEFINE_OSSTATUS( controlPropertyInvalid,                  ControlPropertyInvalid                 );
	DEFINE_OSSTATUS( controlPropertyNotFoundErr,              ControlPropertyNotFoundErr             );
	DEFINE_OSSTATUS( controlHandleInvalidErr,                 ControlHandleInvalidErr                );

/* URLAccess Error Codes */
	DEFINE_OSSTATUS( kURLInvalidURLReferenceError,            URLInvalidURLReferenceError            );
	DEFINE_OSSTATUS( kURLProgressAlreadyDisplayedError,       URLProgressAlreadyDisplayedError       );
	DEFINE_OSSTATUS( kURLDestinationExistsError,              URLDestinationExistsError              );
	DEFINE_OSSTATUS( kURLInvalidURLError,                     URLInvalidURLError                     );
	DEFINE_OSSTATUS( kURLUnsupportedSchemeError,              URLUnsupportedSchemeError              );
	DEFINE_OSSTATUS( kURLServerBusyError,                     URLServerBusyError                     );
	DEFINE_OSSTATUS( kURLAuthenticationError,                 URLAuthenticationError                 );
	DEFINE_OSSTATUS( kURLPropertyNotYetKnownError,            URLPropertyNotYetKnownError            );
	DEFINE_OSSTATUS( kURLUnknownPropertyError,                URLUnknownPropertyError                );
	DEFINE_OSSTATUS( kURLPropertyBufferTooSmallError,         URLPropertyBufferTooSmallError         );
	DEFINE_OSSTATUS( kURLUnsettablePropertyError,             URLUnsettablePropertyError             );
	DEFINE_OSSTATUS( kURLInvalidCallError,                    URLInvalidCallError                    );
	DEFINE_OSSTATUS( kURLFileEmptyError,                      URLFileEmptyError                      );
	DEFINE_OSSTATUS( kURLExtensionFailureError,               URLExtensionFailureError               );
	DEFINE_OSSTATUS( kURLInvalidConfigurationError,           URLInvalidConfigurationError           );
	DEFINE_OSSTATUS( kURLAccessNotAvailableError,             URLAccessNotAvailableError             );
	DEFINE_OSSTATUS( kURL68kNotSupportedError,                URL68kNotSupportedError                );

/* ComponentError codes*/
	DEFINE_OSSTATUS( badComponentInstance,                    BadComponentInstance                   );
	DEFINE_OSSTATUS( badComponentSelector,                    BadComponentSelector                   );
  }

#endif
