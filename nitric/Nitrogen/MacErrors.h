// MacErrors.h

#ifndef NITROGEN_MACERRORS_H
#define NITROGEN_MACERRORS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __MACERRORS__
#include FRAMEWORK_HEADER(CarbonCore,MacErrors.h)
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
  {
   typedef ErrorCode< OSStatus, paramErr >                                ParamErr;
   typedef ErrorCode< OSStatus, noHardwareErr >                           NoHardwareErr;
   typedef ErrorCode< OSStatus, notEnoughHardwareErr >                    NotEnoughHardwareErr;
   typedef ErrorCode< OSStatus, userCanceledErr >                         UserCanceledErr;
   typedef ErrorCode< OSStatus, qErr >                                    QErr;
   typedef ErrorCode< OSStatus, vTypErr >                                 VTypErr;
   typedef ErrorCode< OSStatus, corErr >                                  CorErr;
   typedef ErrorCode< OSStatus, unimpErr >                                UnimpErr;
   typedef ErrorCode< OSStatus, SlpTypeErr >                              SlpTypeErr;
   typedef ErrorCode< OSStatus, seNoDB >                                  SeNoDB;
   typedef ErrorCode< OSStatus, controlErr >                              ControlErr;
   typedef ErrorCode< OSStatus, statusErr >                               StatusErr;
   typedef ErrorCode< OSStatus, readErr >                                 ReadErr;
   typedef ErrorCode< OSStatus, writErr >                                 WritErr;
   typedef ErrorCode< OSStatus, badUnitErr >                              BadUnitErr;
   typedef ErrorCode< OSStatus, unitEmptyErr >                            UnitEmptyErr;
   typedef ErrorCode< OSStatus, openErr >                                 OpenErr;
   typedef ErrorCode< OSStatus, closErr >                                 ClosErr;
   typedef ErrorCode< OSStatus, dRemovErr >                               DRemovErr;
   typedef ErrorCode< OSStatus, dInstErr >                                DInstErr;

   typedef ErrorCode< OSStatus, abortErr >                                AbortErr;
   typedef ErrorCode< OSStatus, iIOAbortErr >                             IIOAbortErr;
   typedef ErrorCode< OSStatus, notOpenErr >                              NotOpenErr;
   typedef ErrorCode< OSStatus, unitTblFullErr >                          UnitTblFullErr;
   typedef ErrorCode< OSStatus, dceExtErr >                               DCEExtErr;
   typedef ErrorCode< OSStatus, slotNumErr >                              SlotNumErr;
   typedef ErrorCode< OSStatus, gcrOnMFMErr >                             GcrOnMFMErr;
   typedef ErrorCode< OSStatus, dirFulErr >                               DirFulErr;
   typedef ErrorCode< OSStatus, dskFulErr >                               DskFulErr;
   typedef ErrorCode< OSStatus, nsvErr >                                  NSVErr;
   typedef ErrorCode< OSStatus, ioErr >                                   IOErr;
   typedef ErrorCode< OSStatus, bdNamErr >                                BdNamErr;
   typedef ErrorCode< OSStatus, fnOpnErr >                                FNOpnErr;
   typedef ErrorCode< OSStatus, eofErr >                                  EOFErr;
   typedef ErrorCode< OSStatus, posErr >                                  PosErr;
   typedef ErrorCode< OSStatus, mFulErr >                                 MFulErr;
   typedef ErrorCode< OSStatus, tmfoErr >                                 TMFOErr;
   typedef ErrorCode< OSStatus, fnfErr >                                  FNFErr;
   typedef ErrorCode< OSStatus, wPrErr >                                  WPrErr;
   typedef ErrorCode< OSStatus, fLckdErr >                                FLckdErr;

   typedef ErrorCode< OSStatus, vLckdErr >                                VLckdErr;
   typedef ErrorCode< OSStatus, fBsyErr >                                 FBsyErr;
   typedef ErrorCode< OSStatus, dupFNErr >                                DupFNErr;
   typedef ErrorCode< OSStatus, opWrErr >                                 OpWrErr;
   typedef ErrorCode< OSStatus, rfNumErr >                                RfNumErr;
   typedef ErrorCode< OSStatus, gfpErr >                                  GFPErr;
   typedef ErrorCode< OSStatus, volOffLinErr >                            VolOffLinErr;
   typedef ErrorCode< OSStatus, permErr >                                 PermErr;
   typedef ErrorCode< OSStatus, volOnLinErr >                             VolOnLinErr;
   typedef ErrorCode< OSStatus, nsDrvErr >                                NSDrvErr;
   typedef ErrorCode< OSStatus, noMacDskErr >                             NoMacDskErr;
   typedef ErrorCode< OSStatus, extFSErr >                                ExtFSErr;
   typedef ErrorCode< OSStatus, fsRnErr >                                 FSRnErr;
   typedef ErrorCode< OSStatus, badMDBErr >                               BadMDBErr;
   typedef ErrorCode< OSStatus, wrPermErr >                               WrPermErr;
   typedef ErrorCode< OSStatus, dirNFErr >                                DirNFErr;
   typedef ErrorCode< OSStatus, tmwdoErr >                                TMWDOErr;
   typedef ErrorCode< OSStatus, badMovErr >                               BadMovErr;
   typedef ErrorCode< OSStatus, wrgVolTypErr >                            WrgVolTypErr;
   typedef ErrorCode< OSStatus, volGoneErr >                              VolGoneErr;

   typedef ErrorCode< OSStatus, fidNotFound >                             FIDNotFound;
   typedef ErrorCode< OSStatus, fidExists >                               FIDExists;
   typedef ErrorCode< OSStatus, notAFileErr >                             NotAFileErr;
   typedef ErrorCode< OSStatus, diffVolErr >                              DiffVolErr;
   typedef ErrorCode< OSStatus, catChangedErr >                           CatChangedErr;
   typedef ErrorCode< OSStatus, desktopDamagedErr >                       DesktopDamagedErr;
   typedef ErrorCode< OSStatus, sameFileErr >                             SameFileErr;
   typedef ErrorCode< OSStatus, badFidErr >                               BadFidErr;
   typedef ErrorCode< OSStatus, notARemountErr >                          NotARemountErr;
   typedef ErrorCode< OSStatus, fileBoundsErr >                           FileBoundsErr;
   typedef ErrorCode< OSStatus, fsDataTooBigErr >                         FSDataTooBigErr;
   typedef ErrorCode< OSStatus, volVMBusyErr >                            VolVMBusyErr;
   typedef ErrorCode< OSStatus, badFCBErr >                               BadFCBErr;
   typedef ErrorCode< OSStatus, errFSUnknownCall >                        ErrFSUnknownCall;
   typedef ErrorCode< OSStatus, errFSBadFSRef >                           ErrFSBadFSRef;
   typedef ErrorCode< OSStatus, errFSBadForkName >                        ErrFSBadForkName;
   typedef ErrorCode< OSStatus, errFSBadBuffer >                          ErrFSBadBuffer;
   typedef ErrorCode< OSStatus, errFSBadForkRef >                         ErrFSBadForkRef;
   typedef ErrorCode< OSStatus, errFSBadInfoBitmap >                      ErrFSBadInfoBitmap;
   typedef ErrorCode< OSStatus, errFSMissingCatInfo >                     ErrFSMissingCatInfo;
   typedef ErrorCode< OSStatus, errFSNotAFolder >                         ErrFSNotAFolder;
   typedef ErrorCode< OSStatus, errFSForkNotFound >                       ErrFSForkNotFound;
   typedef ErrorCode< OSStatus, errFSNameTooLong >                        ErrFSNameTooLong;
   typedef ErrorCode< OSStatus, errFSMissingName >                        ErrFSMissingName;
   typedef ErrorCode< OSStatus, errFSBadPosMode >                         ErrFSBadPosMode;
   typedef ErrorCode< OSStatus, errFSBadAllocFlags >                      ErrFSBadAllocFlags;
   typedef ErrorCode< OSStatus, errFSNoMoreItems >                        ErrFSNoMoreItems;
   typedef ErrorCode< OSStatus, errFSBadItemCount >                       ErrFSBadItemCount;
   typedef ErrorCode< OSStatus, errFSBadSearchParams >                    ErrFSBadSearchParams;
   typedef ErrorCode< OSStatus, errFSRefsDifferent >                      ErrFSRefsDifferent;
   typedef ErrorCode< OSStatus, errFSForkExists >                         ErrFSForkExists;
   typedef ErrorCode< OSStatus, errFSBadIteratorFlags >                   ErrFSBadIteratorFlags;
   typedef ErrorCode< OSStatus, errFSIteratorNotFound >                   ErrFSIteratorNotFound;
   typedef ErrorCode< OSStatus, errFSIteratorNotSupported >               ErrFSIteratorNotSupported;
   typedef ErrorCode< OSStatus, envNotPresent >                           EnvNotPresent;
   typedef ErrorCode< OSStatus, envBadVers >                              EnvBadVers;
   typedef ErrorCode< OSStatus, envVersTooBig >                           EnvVersTooBig;
   typedef ErrorCode< OSStatus, fontDecError >                            FontDecError;
   typedef ErrorCode< OSStatus, fontNotDeclared >                         FontNotDeclared;
   typedef ErrorCode< OSStatus, fontSubErr >                              FontSubErr;
   typedef ErrorCode< OSStatus, fontNotOutlineErr >                       FontNotOutlineErr;
   typedef ErrorCode< OSStatus, firstDskErr >                             FirstDskErr;
   typedef ErrorCode< OSStatus, lastDskErr >                              LastDskErr;
   typedef ErrorCode< OSStatus, noDriveErr >                              NoDriveErr;
   typedef ErrorCode< OSStatus, offLinErr >                               OffLinErr;
   typedef ErrorCode< OSStatus, noNybErr >                                NoNybErr;

   typedef ErrorCode< OSStatus, noAdrMkErr >                              NoAdrMkErr;
   typedef ErrorCode< OSStatus, dataVerErr >                              DataVerErr;
   typedef ErrorCode< OSStatus, badCksmErr >                              BadCksmErr;
   typedef ErrorCode< OSStatus, badBtSlpErr >                             BadBtSlpErr;
   typedef ErrorCode< OSStatus, noDtaMkErr >                              NoDtaMkErr;
   typedef ErrorCode< OSStatus, badDCksum >                               BadDCksum;
   typedef ErrorCode< OSStatus, badDBtSlp >                               BadDBtSlp;
   typedef ErrorCode< OSStatus, wrUnderrun >                              WrUnderrun;
   typedef ErrorCode< OSStatus, cantStepErr >                             CantStepErr;
   typedef ErrorCode< OSStatus, tk0BadErr >                               Tk0BadErr;
   typedef ErrorCode< OSStatus, initIWMErr >                              InitIWMErr;
   typedef ErrorCode< OSStatus, twoSideErr >                              TwoSideErr;
   typedef ErrorCode< OSStatus, spdAdjErr >                               SpdAdjErr;
   typedef ErrorCode< OSStatus, seekErr >                                 SeekErr;
   typedef ErrorCode< OSStatus, sectNFErr >                               SectNFErr;
   typedef ErrorCode< OSStatus, fmt1Err >                                 Fmt1Err;
   typedef ErrorCode< OSStatus, fmt2Err >                                 Fmt2Err;
   typedef ErrorCode< OSStatus, verErr >                                  VerErr;
   typedef ErrorCode< OSStatus, clkRdErr >                                ClkRdErr;
   typedef ErrorCode< OSStatus, clkWrErr >                                ClkWrErr;
   typedef ErrorCode< OSStatus, prWrErr >                                 PrWrErr;
   typedef ErrorCode< OSStatus, prInitErr >                               PrInitErr;
   typedef ErrorCode< OSStatus, rcvrErr >                                 RcvrErr;
   typedef ErrorCode< OSStatus, breakRecd >                               BreakRecd;

/*Scrap Manager errors*/
   typedef ErrorCode< OSStatus, noScrapErr >                              NoScrapErr;
   typedef ErrorCode< OSStatus, noTypeErr >                               NoTypeErr;

/* ENET error codes */
   typedef ErrorCode< OSStatus, eLenErr >                                 ELenErr;
   typedef ErrorCode< OSStatus, eMultiErr >                               EMultiErr;

   typedef ErrorCode< OSStatus, ddpSktErr >                               DDPSktErr;
   typedef ErrorCode< OSStatus, ddpLenErr >                               DDPLenErr;
   typedef ErrorCode< OSStatus, noBridgeErr >                             NoBridgeErr;
   typedef ErrorCode< OSStatus, lapProtErr >                              LAPProtErr;
   typedef ErrorCode< OSStatus, excessCollsns >                           ExcessCollsns;
   typedef ErrorCode< OSStatus, portNotPwr >                              PortNotPwr;
   typedef ErrorCode< OSStatus, portInUse >                               PortInUse;
   typedef ErrorCode< OSStatus, portNotCf >                               PortNotCf;

/* Memory Manager errors*/
   typedef ErrorCode< OSStatus, memROZWarn >                              MemROZWarn;
   typedef ErrorCode< OSStatus, memROZError >                             MemROZError;
   typedef ErrorCode< OSStatus, memROZErr >                               MemROZErr;
   typedef ErrorCode< OSStatus, memFullErr >                              MemFullErr;
   typedef ErrorCode< OSStatus, nilHandleErr >                            NilHandleErr;
   typedef ErrorCode< OSStatus, memWZErr >                                MemWZErr;
   typedef ErrorCode< OSStatus, memPurErr >                               MemPurErr;
   typedef ErrorCode< OSStatus, memAdrErr >                               MemAdrErr;
   typedef ErrorCode< OSStatus, memAZErr >                                MemAZErr;
   typedef ErrorCode< OSStatus, memPCErr >                                MemPCErr;
   typedef ErrorCode< OSStatus, memBCErr >                                MemBCErr;
   typedef ErrorCode< OSStatus, memSCErr >                                MemSCErr;
   typedef ErrorCode< OSStatus, memLockedErr >                            MemLockedErr;

/* Printing Errors */
   typedef ErrorCode< OSStatus, iMemFullErr >                             IMemFullErr;
   typedef ErrorCode< OSStatus, iIOAbort >                                IIOAbort;


   typedef ErrorCode< OSStatus, resourceInMemory >                        ResourceInMemory;
   typedef ErrorCode< OSStatus, writingPastEnd >                          WritingPastEnd;
   typedef ErrorCode< OSStatus, inputOutOfBounds >                        InputOutOfBounds;
   typedef ErrorCode< OSStatus, resNotFound >                             ResNotFound;
   typedef ErrorCode< OSStatus, resFNotFound >                            ResFNotFound;
   typedef ErrorCode< OSStatus, addResFailed >                            AddResFailed;
   typedef ErrorCode< OSStatus, addRefFailed >                            AddRefFailed;
   typedef ErrorCode< OSStatus, rmvResFailed >                            RmvResFailed;
   typedef ErrorCode< OSStatus, rmvRefFailed >                            RmvRefFailed;
   typedef ErrorCode< OSStatus, resAttrErr >                              ResAttrErr;
   typedef ErrorCode< OSStatus, mapReadErr >                              MapReadErr;
   typedef ErrorCode< OSStatus, CantDecompress >                          CantDecompress;
   typedef ErrorCode< OSStatus, badExtResource >                          BadExtResource;
   typedef ErrorCode< OSStatus, noMemForPictPlaybackErr >                 NoMemForPictPlaybackErr;
   typedef ErrorCode< OSStatus, rgnOverflowErr >                          RgnOverflowErr;
   typedef ErrorCode< OSStatus, rgnTooBigError >                          RgnTooBigError;
   typedef ErrorCode< OSStatus, pixMapTooDeepErr >                        PixMapTooDeepErr;
   typedef ErrorCode< OSStatus, insufficientStackErr >                    InsufficientStackErr;
   typedef ErrorCode< OSStatus, nsStackErr >                              NSStackErr;

   typedef ErrorCode< OSStatus, evtNotEnb >                               EvtNotEnb;

/* OffScreen QuickDraw Errors */
   typedef ErrorCode< OSStatus, cMatchErr >                               CMatchErr;
   typedef ErrorCode< OSStatus, cTempMemErr >                             CTempMemErr;
   typedef ErrorCode< OSStatus, cNoMemErr >                               CNoMemErr;
   typedef ErrorCode< OSStatus, cRangeErr >                               CRangeErr;
   typedef ErrorCode< OSStatus, cProtectErr >                             CProtectErr;
   typedef ErrorCode< OSStatus, cDevErr >                                 CDevErr;
   typedef ErrorCode< OSStatus, cResErr >                                 CResErr;
   typedef ErrorCode< OSStatus, cDepthErr >                               CDepthErr;
   typedef ErrorCode< OSStatus, rgnTooBigErr >                            RgnTooBigErr;
   typedef ErrorCode< OSStatus, updPixMemErr >                            UpdPixMemErr;
   typedef ErrorCode< OSStatus, pictInfoVersionErr >                      PictInfoVersionErr;
   typedef ErrorCode< OSStatus, pictInfoIDErr >                           PictInfoIDErr;
   typedef ErrorCode< OSStatus, pictInfoVerbErr >                         PictInfoVerbErr;
   typedef ErrorCode< OSStatus, cantLoadPickMethodErr >                   CantLoadPickMethodErr;
   typedef ErrorCode< OSStatus, colorsRequestedErr >                      ColorsRequestedErr;
   typedef ErrorCode< OSStatus, pictureDataErr >                          PictureDataErr;

/* ColorSync Result codes */
   typedef ErrorCode< OSStatus, cmProfileError >                          CMProfileError;
   typedef ErrorCode< OSStatus, cmMethodError >                           CMMethodError;
   typedef ErrorCode< OSStatus, cmMethodNotFound >                        CMMethodNotFound;
   typedef ErrorCode< OSStatus, cmProfileNotFound >                       CMProfileNotFound;
   typedef ErrorCode< OSStatus, cmProfilesIdentical >                     CMProfilesIdentical;
   typedef ErrorCode< OSStatus, cmCantConcatenateError >                  CMCantConcatenateError;
   typedef ErrorCode< OSStatus, cmCantXYZ >                               CMCantXYZ;
   typedef ErrorCode< OSStatus, cmCantDeleteProfile >                     CMCantDeleteProfile;
   typedef ErrorCode< OSStatus, cmUnsupportedDataType >                   CMUnsupportedDataType;
   typedef ErrorCode< OSStatus, cmNoCurrentProfile >                      CMNoCurrentProfile;


/*Sound Manager errors*/
   typedef ErrorCode< OSStatus, noHardware >                              NoHardware;
   typedef ErrorCode< OSStatus, notEnoughHardware >                       NotEnoughHardware;
   typedef ErrorCode< OSStatus, queueFull >                               QueueFull;
   typedef ErrorCode< OSStatus, resProblem >                              ResProblem;
   typedef ErrorCode< OSStatus, badChannel >                              BadChannel;
   typedef ErrorCode< OSStatus, badFormat >                               BadFormat;
   typedef ErrorCode< OSStatus, notEnoughBufferSpace >                    NotEnoughBufferSpace;
   typedef ErrorCode< OSStatus, badFileFormat >                           BadFileFormat;
   typedef ErrorCode< OSStatus, channelBusy >                             ChannelBusy;
   typedef ErrorCode< OSStatus, buffersTooSmall >                         BuffersTooSmall;
   typedef ErrorCode< OSStatus, channelNotBusy >                          ChannelNotBusy;
   typedef ErrorCode< OSStatus, noMoreRealTime >                          NoMoreRealTime;
   typedef ErrorCode< OSStatus, siVBRCompressionNotSupported >            SIVBRCompressionNotSupported;
   typedef ErrorCode< OSStatus, siNoSoundInHardware >                     SINoSoundInHardware;
   typedef ErrorCode< OSStatus, siBadSoundInDevice >                      SIBadSoundInDevice;
   typedef ErrorCode< OSStatus, siNoBufferSpecified >                     SINoBufferSpecified;
   typedef ErrorCode< OSStatus, siInvalidCompression >                    SIInvalidCompression;
   typedef ErrorCode< OSStatus, siHardDriveTooSlow >                      SIHardDriveTooSlow;
   typedef ErrorCode< OSStatus, siInvalidSampleRate >                     SIInvalidSampleRate;
   typedef ErrorCode< OSStatus, siInvalidSampleSize >                     SIInvalidSampleSize;
   typedef ErrorCode< OSStatus, siDeviceBusyErr >                         SIDeviceBusyErr;
   typedef ErrorCode< OSStatus, siBadDeviceName >                         SIBadDeviceName;
   typedef ErrorCode< OSStatus, siBadRefNum >                             SIBadRefNum;
   typedef ErrorCode< OSStatus, siInputDeviceErr >                        SIInputDeviceErr;
   typedef ErrorCode< OSStatus, siUnknownInfoType >                       SIUnknownInfoType;
   typedef ErrorCode< OSStatus, siUnknownQuality >                        SIUnknownQuality;

/*Speech Manager errors*/
   typedef ErrorCode< OSStatus, noSynthFound >                            NoSynthFound;
   typedef ErrorCode< OSStatus, synthOpenFailed >                         SynthOpenFailed;
   typedef ErrorCode< OSStatus, synthNotReady >                           SynthNotReady;
   typedef ErrorCode< OSStatus, bufTooSmall >                             BufTooSmall;
   typedef ErrorCode< OSStatus, voiceNotFound >                           VoiceNotFound;
   typedef ErrorCode< OSStatus, incompatibleVoice >                       IncompatibleVoice;
   typedef ErrorCode< OSStatus, badDictFormat >                           BadDictFormat;
   typedef ErrorCode< OSStatus, badInputText >                            BadInputText;

/* Midi Manager Errors: */
   typedef ErrorCode< OSStatus, midiNoClientErr >                         MIDINoClientErr;
   typedef ErrorCode< OSStatus, midiNoPortErr >                           MIDINoPortErr;
   typedef ErrorCode< OSStatus, midiTooManyPortsErr >                     MIDITooManyPortsErr;
   typedef ErrorCode< OSStatus, midiTooManyConsErr >                      MIDITooManyConsErr;
   typedef ErrorCode< OSStatus, midiVConnectErr >                         MIDIVConnectErr;
   typedef ErrorCode< OSStatus, midiVConnectMade >                        MIDIVConnectMade;
   typedef ErrorCode< OSStatus, midiVConnectRmvd >                        MIDIVConnectRmvd;
   typedef ErrorCode< OSStatus, midiNoConErr >                            MIDINoConErr;
   typedef ErrorCode< OSStatus, midiWriteErr >                            MIDIWriteErr;
   typedef ErrorCode< OSStatus, midiNameLenErr >                          MIDINameLenErr;
   typedef ErrorCode< OSStatus, midiDupIDErr >                            MIDIDupIDErr;
   typedef ErrorCode< OSStatus, midiInvalidCmdErr >                       MIDIInvalidCmdErr;


   typedef ErrorCode< OSStatus, nmTypErr >                                NMTypErr;


   typedef ErrorCode< OSStatus, siInitSDTblErr >                          SIInitSDTblErr;
   typedef ErrorCode< OSStatus, siInitVBLQsErr >                          SIInitVBLQsErr;
   typedef ErrorCode< OSStatus, siInitSPTblErr >                          SIInitSPTblErr;
   typedef ErrorCode< OSStatus, sdmJTInitErr >                            SDMJTInitErr;
   typedef ErrorCode< OSStatus, sdmInitErr >                              SDMInitErr;
   typedef ErrorCode< OSStatus, sdmSRTInitErr >                           SDMSRTInitErr;
   typedef ErrorCode< OSStatus, sdmPRAMInitErr >                          SDMPRAMInitErr;
   typedef ErrorCode< OSStatus, sdmPriInitErr >                           SDMPriInitErr;

   typedef ErrorCode< OSStatus, smSDMInitErr >                            SMSDMInitErr;
   typedef ErrorCode< OSStatus, smSRTInitErr >                            SMSRTInitErr;
   typedef ErrorCode< OSStatus, smPRAMInitErr >                           SMPRAMInitErr;
   typedef ErrorCode< OSStatus, smPriInitErr >                            SMPriInitErr;
   typedef ErrorCode< OSStatus, smEmptySlot >                             SMEmptySlot;
   typedef ErrorCode< OSStatus, smCRCFail >                               SMCRCFail;
   typedef ErrorCode< OSStatus, smFormatErr >                             SMFormatErr;
   typedef ErrorCode< OSStatus, smRevisionErr >                           SMRevisionErr;
   typedef ErrorCode< OSStatus, smNoDir >                                 SMNoDir;
   typedef ErrorCode< OSStatus, smDisabledSlot >                          SMDisabledSlot;
   typedef ErrorCode< OSStatus, smNosInfoArray >                          SMNosInfoArray;


   typedef ErrorCode< OSStatus, smResrvErr >                              SMResrvErr;
   typedef ErrorCode< OSStatus, smUnExBusErr >                            SMUnExBusErr;
   typedef ErrorCode< OSStatus, smBLFieldBad >                            SMBLFieldBad;
   typedef ErrorCode< OSStatus, smFHBlockRdErr >                          SMFHBlockRdErr;
   typedef ErrorCode< OSStatus, smFHBlkDispErr >                          SMFHBlkDispErr;
   typedef ErrorCode< OSStatus, smDisposePErr >                           SMDisposePErr;
   typedef ErrorCode< OSStatus, smNoBoardSRsrc >                          SMNoBoardSRsrc;
   typedef ErrorCode< OSStatus, smGetPRErr >                              SMGetPRErr;
   typedef ErrorCode< OSStatus, smNoBoardId >                             SMNoBoardId;
   typedef ErrorCode< OSStatus, smInitStatVErr >                          SMInitStatVErr;
   typedef ErrorCode< OSStatus, smInitTblVErr >                           SMInitTblVErr;
   typedef ErrorCode< OSStatus, smNoJmpTbl >                              SMNoJmpTbl;
   typedef ErrorCode< OSStatus, smReservedSlot >                          SMReservedSlot;
   typedef ErrorCode< OSStatus, smBadBoardId >                            SMBadBoardId;
   typedef ErrorCode< OSStatus, smBusErrTO >                              SMBusErrTO;

   typedef ErrorCode< OSStatus, svTempDisable >                           SVTempDisable;
   typedef ErrorCode< OSStatus, svDisabled >                              SVDisabled;
   typedef ErrorCode< OSStatus, smBadRefId >                              SMBadRefId;
   typedef ErrorCode< OSStatus, smBadsList >                              SMBadsList;
   typedef ErrorCode< OSStatus, smReservedErr >                           SMReservedErr;
   typedef ErrorCode< OSStatus, smCodeRevErr >                            SMCodeRevErr;

   typedef ErrorCode< OSStatus, smCPUErr >                                SMCPUErr;
   typedef ErrorCode< OSStatus, smsPointerNil >                           SMSPointerNil;
   typedef ErrorCode< OSStatus, smNilsBlockErr >                          SMNilsBlockErr;
   typedef ErrorCode< OSStatus, smSlotOOBErr >                            SMSlotOOBErr;
   typedef ErrorCode< OSStatus, smSelOOBErr >                             SMSelOOBErr;
   typedef ErrorCode< OSStatus, smNewPErr >                               SMNewPErr;
   typedef ErrorCode< OSStatus, smBlkMoveErr >                            SMBlkMoveErr;
   typedef ErrorCode< OSStatus, smCkStatusErr >                           SMCkStatusErr;
   typedef ErrorCode< OSStatus, smGetDrvrNamErr >                         SMGetDrvrNamErr;
   typedef ErrorCode< OSStatus, smDisDrvrNamErr >                         SMDisDrvrNamErr;
   typedef ErrorCode< OSStatus, smNoMoresRsrcs >                          SMNoMoresRsrcs;
   typedef ErrorCode< OSStatus, smsGetDrvrErr >                           SMSGetDrvrErr;
   typedef ErrorCode< OSStatus, smBadsPtrErr >                            SMBadsPtrErr;
   typedef ErrorCode< OSStatus, smByteLanesErr >                          SMByteLanesErr;
   typedef ErrorCode< OSStatus, smOffsetErr >                             SMOffsetErr;
   typedef ErrorCode< OSStatus, smNoGoodOpens >                           SMNoGoodOpens;
   typedef ErrorCode< OSStatus, smSRTOvrFlErr >                           SMSRTOvrFlErr;
   typedef ErrorCode< OSStatus, smRecNotFnd >                             SMRecNotFnd;


/*Dictionary Manager errors*/
   typedef ErrorCode< OSStatus, notBTree >                                NotBTree;
   typedef ErrorCode< OSStatus, btNoSpace >                               BTNoSpace;
   typedef ErrorCode< OSStatus, btDupRecErr >                             BTDupRecErr;
   typedef ErrorCode< OSStatus, btRecNotFnd >                             BTRecNotFnd;
   typedef ErrorCode< OSStatus, btKeyLenErr >                             BTKeyLenErr;
   typedef ErrorCode< OSStatus, btKeyAttrErr >                            BTKeyAttrErr;
   typedef ErrorCode< OSStatus, unknownInsertModeErr >                    UnknownInsertModeErr;
   typedef ErrorCode< OSStatus, recordDataTooBigErr >                     RecordDataTooBigErr;
   typedef ErrorCode< OSStatus, invalidIndexErr >                         InvalidIndexErr;


/* Error codes from FSM functions */
   typedef ErrorCode< OSStatus, fsmFFSNotFoundErr >                       FSMFFSNotFoundErr;
   typedef ErrorCode< OSStatus, fsmBusyFFSErr >                           FSMBusyFFSErr;
   typedef ErrorCode< OSStatus, fsmBadFFSNameErr >                        FSMBadFFSNameErr;
   typedef ErrorCode< OSStatus, fsmBadFSDLenErr >                         FSMBadFSDLenErr;
   typedef ErrorCode< OSStatus, fsmDuplicateFSIDErr >                     FSMDuplicateFSIDErr;
   typedef ErrorCode< OSStatus, fsmBadFSDVersionErr >                     FSMBadFSDVersionErr;
   typedef ErrorCode< OSStatus, fsmNoAlternateStackErr >                  FSMNoAlternateStackErr;
   typedef ErrorCode< OSStatus, fsmUnknownFSMMessageErr >                 FSMUnknownFSMMessageErr;


/* Edition Mgr errors*/
   typedef ErrorCode< OSStatus, editionMgrInitErr >                       EditionMgrInitErr;
   typedef ErrorCode< OSStatus, badSectionErr >                           BadSectionErr;
   typedef ErrorCode< OSStatus, notRegisteredSectionErr >                 NotRegisteredSectionErr;
   typedef ErrorCode< OSStatus, badEditionFileErr >                       BadEditionFileErr;
   typedef ErrorCode< OSStatus, badSubPartErr >                           BadSubPartErr;
   typedef ErrorCode< OSStatus, multiplePublisherWrn >                    MultiplePublisherWrn;
   typedef ErrorCode< OSStatus, containerNotFoundWrn >                    ContainerNotFoundWrn;
   typedef ErrorCode< OSStatus, containerAlreadyOpenWrn >                 ContainerAlreadyOpenWrn;
   typedef ErrorCode< OSStatus, notThePublisherWrn >                      NotThePublisherWrn;

   typedef ErrorCode< OSStatus, teScrapSizeErr >                          TEScrapSizeErr;
   typedef ErrorCode< OSStatus, hwParamErr >                              HWParamErr;
   typedef ErrorCode< OSStatus, driverHardwareGoneErr >                   DriverHardwareGoneErr;

/*Process Manager errors*/
   typedef ErrorCode< OSStatus, procNotFound >                            ProcNotFound;
   typedef ErrorCode< OSStatus, memFragErr >                              MemFragErr;
   typedef ErrorCode< OSStatus, appModeErr >                              AppModeErr;
   typedef ErrorCode< OSStatus, protocolErr >                             ProtocolErr;
   typedef ErrorCode< OSStatus, hardwareConfigErr >                       HardwareConfigErr;
   typedef ErrorCode< OSStatus, appMemFullErr >                           AppMemFullErr;
   typedef ErrorCode< OSStatus, appIsDaemon >                             AppIsDaemon;
   typedef ErrorCode< OSStatus, bufferIsSmall >                           BufferIsSmall;
   typedef ErrorCode< OSStatus, noOutstandingHLE >                        NoOutstandingHLE;
   typedef ErrorCode< OSStatus, connectionInvalid >                       ConnectionInvalid;
   typedef ErrorCode< OSStatus, noUserInteractionAllowed >                NoUserInteractionAllowed;
   typedef ErrorCode< OSStatus, wrongApplicationPlatform >                WrongApplicationPlatform;
   typedef ErrorCode< OSStatus, appVersionTooOld >                        AppVersionTooOld;
   typedef ErrorCode< OSStatus, notAppropriateForClassic >                NotAppropriateForClassic;

/* Thread Manager Error Codes */
   typedef ErrorCode< OSStatus, threadTooManyReqsErr >                    ThreadTooManyReqsErr;
   typedef ErrorCode< OSStatus, threadNotFoundErr >                       ThreadNotFoundErr;
   typedef ErrorCode< OSStatus, threadProtocolErr >                       ThreadProtocolErr;

/*MemoryDispatch errors*/
   typedef ErrorCode< OSStatus, notEnoughMemoryErr >                      NotEnoughMemoryErr;
   typedef ErrorCode< OSStatus, notHeldErr >                              NotHeldErr;
   typedef ErrorCode< OSStatus, cannotMakeContiguousErr >                 CannotMakeContiguousErr;
   typedef ErrorCode< OSStatus, notLockedErr >                            NotLockedErr;
   typedef ErrorCode< OSStatus, interruptsMaskedErr >                     InterruptsMaskedErr;
   typedef ErrorCode< OSStatus, cannotDeferErr >                          CannotDeferErr;
   typedef ErrorCode< OSStatus, noMMUErr >                                NoMMUErr;

/* FileMapping errors */
   typedef ErrorCode< OSStatus, vmInvalidBackingFileIDErr >               VMInvalidBackingFileIDErr;
   typedef ErrorCode< OSStatus, vmMappingPrivilegesErr >                  VMMappingPrivilegesErr;
   typedef ErrorCode< OSStatus, vmBusyBackingFileErr >                    VMBusyBackingFileErr;
   typedef ErrorCode< OSStatus, vmNoMoreBackingFilesErr >                 VMNoMoreBackingFilesErr;
   typedef ErrorCode< OSStatus, vmInvalidFileViewIDErr >                  VMInvalidFileViewIDErr;
   typedef ErrorCode< OSStatus, vmFileViewAccessErr >                     VMFileViewAccessErr;
   typedef ErrorCode< OSStatus, vmNoMoreFileViewsErr >                    VMNoMoreFileViewsErr;
   typedef ErrorCode< OSStatus, vmAddressNotInFileViewErr >               VMAddressNotInFileViewErr;
   typedef ErrorCode< OSStatus, vmInvalidOwningProcessErr >               VMInvalidOwningProcessErr;

/* Database access error codes */
   typedef ErrorCode< OSStatus, rcDBNull >                                RCDBNull;
   typedef ErrorCode< OSStatus, rcDBValue >                               RCDBValue;
   typedef ErrorCode< OSStatus, rcDBError >                               RCDBError;
   typedef ErrorCode< OSStatus, rcDBBadType >                             RCDBBadType;
   typedef ErrorCode< OSStatus, rcDBBreak >                               RCDBBreak;
   typedef ErrorCode< OSStatus, rcDBExec >                                RCDBExec;
   typedef ErrorCode< OSStatus, rcDBBadSessID >                           RCDBBadSessID;
   typedef ErrorCode< OSStatus, rcDBBadSessNum >                          RCDBBadSessNum;
   typedef ErrorCode< OSStatus, rcDBBadDDEV >                             RCDBBadDDEV;
   typedef ErrorCode< OSStatus, rcDBAsyncNotSupp >                        RCDBAsyncNotSupp;
   typedef ErrorCode< OSStatus, rcDBBadAsyncPB >                          RCDBBadAsyncPB;
   typedef ErrorCode< OSStatus, rcDBNoHandler >                           RCDBNoHandler;
   typedef ErrorCode< OSStatus, rcDBWrongVersion >                        RCDBWrongVersion;
   typedef ErrorCode< OSStatus, rcDBPackNotInited >                       RCDBPackNotInited;


/*Help Mgr error range: -850 to -874*/
   typedef ErrorCode< OSStatus, hmHelpDisabled >                          HMHelpDisabled;
   typedef ErrorCode< OSStatus, hmBalloonAborted >                        HMBalloonAborted;
   typedef ErrorCode< OSStatus, hmSameAsLastBalloon >                     HMSameAsLastBalloon;
   typedef ErrorCode< OSStatus, hmHelpManagerNotInited >                  HMHelpManagerNotInited;
   typedef ErrorCode< OSStatus, hmSkippedBalloon >                        HMSkippedBalloon;
   typedef ErrorCode< OSStatus, hmWrongVersion >                          HMWrongVersion;
   typedef ErrorCode< OSStatus, hmUnknownHelpType >                       HMUnknownHelpType;
   typedef ErrorCode< OSStatus, hmOperationUnsupported >                  HMOperationUnsupported;
   typedef ErrorCode< OSStatus, hmNoBalloonUp >                           HMNoBalloonUp;
   typedef ErrorCode< OSStatus, hmCloseViewActive >                       HMCloseViewActive;



/*PPC errors*/
   typedef ErrorCode< OSStatus, notInitErr >                              NotInitErr;
   typedef ErrorCode< OSStatus, nameTypeErr >                             NameTypeErr;
   typedef ErrorCode< OSStatus, noPortErr >                               NoPortErr;
   typedef ErrorCode< OSStatus, noGlobalsErr >                            NoGlobalsErr;
   typedef ErrorCode< OSStatus, localOnlyErr >                            LocalOnlyErr;
   typedef ErrorCode< OSStatus, destPortErr >                             DestPortErr;
   typedef ErrorCode< OSStatus, sessTableErr >                            SessTableErr;
   typedef ErrorCode< OSStatus, noSessionErr >                            NoSessionErr;
   typedef ErrorCode< OSStatus, badReqErr >                               BadReqErr;
   typedef ErrorCode< OSStatus, portNameExistsErr >                       PortNameExistsErr;
   typedef ErrorCode< OSStatus, noUserNameErr >                           NoUserNameErr;
   typedef ErrorCode< OSStatus, userRejectErr >                           UserRejectErr;
   typedef ErrorCode< OSStatus, noMachineNameErr >                        NoMachineNameErr;
   typedef ErrorCode< OSStatus, noToolboxNameErr >                        NoToolboxNameErr;
   typedef ErrorCode< OSStatus, noResponseErr >                           NoResponseErr;
   typedef ErrorCode< OSStatus, portClosedErr >                           PortClosedErr;
   typedef ErrorCode< OSStatus, sessClosedErr >                           SessClosedErr;
   typedef ErrorCode< OSStatus, badPortNameErr >                          BadPortNameErr;
   typedef ErrorCode< OSStatus, noDefaultUserErr >                        NoDefaultUserErr;
   typedef ErrorCode< OSStatus, notLoggedInErr >                          NotLoggedInErr;
   typedef ErrorCode< OSStatus, noUserRefErr >                            NoUserRefErr;
   typedef ErrorCode< OSStatus, networkErr >                              NetworkErr;
   typedef ErrorCode< OSStatus, noInformErr >                             NoInformErr;
   typedef ErrorCode< OSStatus, authFailErr >                             AuthFailErr;
   typedef ErrorCode< OSStatus, noUserRecErr >                            NoUserRecErr;
   typedef ErrorCode< OSStatus, badServiceMethodErr >                     BadServiceMethodErr;
   typedef ErrorCode< OSStatus, badLocNameErr >                           BadLocNameErr;
   typedef ErrorCode< OSStatus, guestNotAllowedErr >                      GuestNotAllowedErr;

/* Font Mgr errors*/
   typedef ErrorCode< OSStatus, kFMIterationCompleted >                   FMIterationCompleted;
   typedef ErrorCode< OSStatus, kFMInvalidFontFamilyErr >                 FMInvalidFontFamilyErr;
   typedef ErrorCode< OSStatus, kFMInvalidFontErr >                       FMInvalidFontErr;
   typedef ErrorCode< OSStatus, kFMIterationScopeModifiedErr >            FMIterationScopeModifiedErr;
   typedef ErrorCode< OSStatus, kFMFontTableAccessErr >                   FMFontTableAccessErr;
   typedef ErrorCode< OSStatus, kFMFontContainerAccessErr >               FMFontContainerAccessErr;

   typedef ErrorCode< OSStatus, noMaskFoundErr >                          NoMaskFoundErr;

   typedef ErrorCode< OSStatus, nbpBuffOvr >                              NBPBuffOvr;
   typedef ErrorCode< OSStatus, nbpNoConfirm >                            NBPNoConfirm;
   typedef ErrorCode< OSStatus, nbpConfDiff >                             NBPConfDiff;
   typedef ErrorCode< OSStatus, nbpDuplicate >                            NBPDuplicate;
   typedef ErrorCode< OSStatus, nbpNotFound >                             NBPNotFound;
   typedef ErrorCode< OSStatus, nbpNISErr >                               NBPNISErr;

   typedef ErrorCode< OSStatus, aspBadVersNum >                           ASPBadVersNum;
   typedef ErrorCode< OSStatus, aspBufTooSmall >                          ASPBufTooSmall;
   typedef ErrorCode< OSStatus, aspNoMoreSess >                           ASPNoMoreSess;
   typedef ErrorCode< OSStatus, aspNoServers >                            ASPNoServers;
   typedef ErrorCode< OSStatus, aspParamErr >                             ASPParamErr;
   typedef ErrorCode< OSStatus, aspServerBusy >                           ASPServerBusy;
   typedef ErrorCode< OSStatus, aspSessClosed >                           ASPSessClosed;
   typedef ErrorCode< OSStatus, aspSizeErr >                              ASPSizeErr;
   typedef ErrorCode< OSStatus, aspTooMany >                              ASPTooMany;
   typedef ErrorCode< OSStatus, aspNoAck >                                ASPNoAck;

   typedef ErrorCode< OSStatus, reqFailed >                               ReqFailed;
   typedef ErrorCode< OSStatus, tooManyReqs >                             TooManyReqs;
   typedef ErrorCode< OSStatus, tooManySkts >                             TooManySkts;
   typedef ErrorCode< OSStatus, badATPSkt >                               BadATPSkt;
   typedef ErrorCode< OSStatus, badBuffNum >                              BadBuffNum;
   typedef ErrorCode< OSStatus, noRelErr >                                NoRelErr;
   typedef ErrorCode< OSStatus, cbNotFound >                              CbNotFound;
   typedef ErrorCode< OSStatus, noSendResp >                              NoSendResp;
   typedef ErrorCode< OSStatus, noDataArea >                              NoDataArea;
   typedef ErrorCode< OSStatus, reqAborted >                              ReqAborted;

/* ADSP Error Codes */
   typedef ErrorCode< OSStatus, errRefNum >                               ErrRefNum;
   typedef ErrorCode< OSStatus, errAborted >                              ErrAborted;
   typedef ErrorCode< OSStatus, errState >                                ErrState;
   typedef ErrorCode< OSStatus, errOpening >                              ErrOpening;
   typedef ErrorCode< OSStatus, errAttention >                            ErrAttention;
   typedef ErrorCode< OSStatus, errFwdReset >                             ErrFwdReset;
   typedef ErrorCode< OSStatus, errDSPQueueSize >                         ErrDSPQueueSize;
   typedef ErrorCode< OSStatus, errOpenDenied >                           ErrOpenDenied;


/* Apple event manager error messages */
   typedef ErrorCode< OSStatus, errAECoercionFail >                       ErrAECoercionFail;
   typedef ErrorCode< OSStatus, errAEDescNotFound >                       ErrAEDescNotFound;
   typedef ErrorCode< OSStatus, errAECorruptData >                        ErrAECorruptData;
   typedef ErrorCode< OSStatus, errAEWrongDataType >                      ErrAEWrongDataType;
   typedef ErrorCode< OSStatus, errAENotAEDesc >                          ErrAENotAEDesc;
   typedef ErrorCode< OSStatus, errAEBadListItem >                        ErrAEBadListItem;
   typedef ErrorCode< OSStatus, errAENewerVersion >                       ErrAENewerVersion;
   typedef ErrorCode< OSStatus, errAENotAppleEvent >                      ErrAENotAppleEvent;
   typedef ErrorCode< OSStatus, errAEEventNotHandled >                    ErrAEEventNotHandled;
   typedef ErrorCode< OSStatus, errAEReplyNotValid >                      ErrAEReplyNotValid;
   typedef ErrorCode< OSStatus, errAEUnknownSendMode >                    ErrAEUnknownSendMode;
   typedef ErrorCode< OSStatus, errAEWaitCanceled >                       ErrAEWaitCanceled;
   typedef ErrorCode< OSStatus, errAETimeout >                            ErrAETimeout;
   typedef ErrorCode< OSStatus, errAENoUserInteraction >                  ErrAENoUserInteraction;
   typedef ErrorCode< OSStatus, errAENotASpecialFunction >                ErrAENotASpecialFunction;
   typedef ErrorCode< OSStatus, errAEParamMissed >                        ErrAEParamMissed;
   typedef ErrorCode< OSStatus, errAEUnknownAddressType >                 ErrAEUnknownAddressType;
   typedef ErrorCode< OSStatus, errAEHandlerNotFound >                    ErrAEHandlerNotFound;
   typedef ErrorCode< OSStatus, errAEReplyNotArrived >                    ErrAEReplyNotArrived;
   typedef ErrorCode< OSStatus, errAEIllegalIndex >                       ErrAEIllegalIndex;
   typedef ErrorCode< OSStatus, errAEImpossibleRange >                    ErrAEImpossibleRange;
   typedef ErrorCode< OSStatus, errAEWrongNumberArgs >                    ErrAEWrongNumberArgs;
   typedef ErrorCode< OSStatus, errAEAccessorNotFound >                   ErrAEAccessorNotFound;
   typedef ErrorCode< OSStatus, errAENoSuchLogical >                      ErrAENoSuchLogical;
   typedef ErrorCode< OSStatus, errAEBadTestKey >                         ErrAEBadTestKey;
   typedef ErrorCode< OSStatus, errAENotAnObjSpec >                       ErrAENotAnObjSpec;
   typedef ErrorCode< OSStatus, errAENoSuchObject >                       ErrAENoSuchObject;
   typedef ErrorCode< OSStatus, errAENegativeCount >                      ErrAENegativeCount;
   typedef ErrorCode< OSStatus, errAEEmptyListContainer >                 ErrAEEmptyListContainer;
   typedef ErrorCode< OSStatus, errAEUnknownObjectType >                  ErrAEUnknownObjectType;
   typedef ErrorCode< OSStatus, errAERecordingIsAlreadyOn >               ErrAERecordingIsAlreadyOn;
   typedef ErrorCode< OSStatus, errAEReceiveTerminate >                   ErrAEReceiveTerminate;
   typedef ErrorCode< OSStatus, errAEReceiveEscapeCurrent >               ErrAEReceiveEscapeCurrent;
   typedef ErrorCode< OSStatus, errAEEventFiltered >                      ErrAEEventFiltered;
   typedef ErrorCode< OSStatus, errAEDuplicateHandler >                   ErrAEDuplicateHandler;
   typedef ErrorCode< OSStatus, errAEStreamBadNesting >                   ErrAEStreamBadNesting;
   typedef ErrorCode< OSStatus, errAEStreamAlreadyConverted>              ErrAEStreamAlreadyConverted;
   typedef ErrorCode< OSStatus, errAEDescIsNull >                         ErrAEDescIsNull;
   typedef ErrorCode< OSStatus, errAEBuildSyntaxError >                   ErrAEBuildSyntaxError;
   typedef ErrorCode< OSStatus, errAEBufferTooSmall >                     ErrAEBufferTooSmall;

   typedef ErrorCode< OSStatus, errOSASystemError >                       ErrOSASystemError;
   typedef ErrorCode< OSStatus, errOSAInvalidID >                         ErrOSAInvalidID;
   typedef ErrorCode< OSStatus, errOSABadStorageType >                    ErrOSABadStorageType;
   typedef ErrorCode< OSStatus, errOSAScriptError >                       ErrOSAScriptError;
   typedef ErrorCode< OSStatus, errOSABadSelector >                       ErrOSABadSelector;
   typedef ErrorCode< OSStatus, errOSASourceNotAvailable >                ErrOSASourceNotAvailable;
   typedef ErrorCode< OSStatus, errOSANoSuchDialect >                     ErrOSANoSuchDialect;
   typedef ErrorCode< OSStatus, errOSADataFormatObsolete >                ErrOSADataFormatObsolete;
   typedef ErrorCode< OSStatus, errOSADataFormatTooNew >                  ErrOSADataFormatTooNew;
   typedef ErrorCode< OSStatus, errOSACorruptData >                       ErrOSACorruptData;
   typedef ErrorCode< OSStatus, errOSARecordingIsAlreadyOn >              ErrOSARecordingIsAlreadyOn;
   typedef ErrorCode< OSStatus, errOSAComponentMismatch >                 ErrOSAComponentMismatch;
   typedef ErrorCode< OSStatus, errOSACantOpenComponent >                 ErrOSACantOpenComponent;



/* AppleEvent error definitions */
   typedef ErrorCode< OSStatus, errOffsetInvalid >                        ErrOffsetInvalid;
   typedef ErrorCode< OSStatus, errOffsetIsOutsideOfView >                ErrOffsetIsOutsideOfView;
   typedef ErrorCode< OSStatus, errTopOfDocument >                        ErrTopOfDocument;
   typedef ErrorCode< OSStatus, errTopOfBody >                            ErrTopOfBody;
   typedef ErrorCode< OSStatus, errEndOfDocument >                        ErrEndOfDocument;
   typedef ErrorCode< OSStatus, errEndOfBody >                            ErrEndOfBody;


/* Drag Manager error codes */
   typedef ErrorCode< OSStatus, badDragRefErr >                           BadDragRefErr;
   typedef ErrorCode< OSStatus, badDragItemErr >                          BadDragItemErr;
   typedef ErrorCode< OSStatus, badDragFlavorErr >                        BadDragFlavorErr;
   typedef ErrorCode< OSStatus, duplicateFlavorErr >                      DuplicateFlavorErr;
   typedef ErrorCode< OSStatus, cantGetFlavorErr >                        CantGetFlavorErr;
   typedef ErrorCode< OSStatus, duplicateHandlerErr >                     DuplicateHandlerErr;
   typedef ErrorCode< OSStatus, handlerNotFoundErr >                      HandlerNotFoundErr;
   typedef ErrorCode< OSStatus, dragNotAcceptedErr >                      DragNotAcceptedErr;
   typedef ErrorCode< OSStatus, unsupportedForPlatformErr >               UnsupportedForPlatformErr;
   typedef ErrorCode< OSStatus, noSuitableDisplaysErr >                   NoSuitableDisplaysErr;
   typedef ErrorCode< OSStatus, badImageRgnErr >                          BadImageRgnErr;
   typedef ErrorCode< OSStatus, badImageErr >                             BadImageErr;
   typedef ErrorCode< OSStatus, nonDragOriginatorErr >                    NonDragOriginatorErr;


/*QuickTime errors*/
   typedef ErrorCode< OSStatus, couldNotResolveDataRef >                  CouldNotResolveDataRef;
   typedef ErrorCode< OSStatus, badImageDescription >                     BadImageDescription;
   typedef ErrorCode< OSStatus, badPublicMovieAtom >                      BadPublicMovieAtom;
   typedef ErrorCode< OSStatus, cantFindHandler >                         CantFindHandler;
   typedef ErrorCode< OSStatus, cantOpenHandler >                         CantOpenHandler;
   typedef ErrorCode< OSStatus, badComponentType >                        BadComponentType;
   typedef ErrorCode< OSStatus, noMediaHandler >                          NoMediaHandler;
   typedef ErrorCode< OSStatus, noDataHandler >                           NoDataHandler;
   typedef ErrorCode< OSStatus, invalidMedia >                            InvalidMedia;
   typedef ErrorCode< OSStatus, invalidTrack >                            InvalidTrack;
   typedef ErrorCode< OSStatus, invalidMovie >                            InvalidMovie;
   typedef ErrorCode< OSStatus, invalidSampleTable >                      InvalidSampleTable;
   typedef ErrorCode< OSStatus, invalidDataRef >                          InvalidDataRef;
   typedef ErrorCode< OSStatus, invalidHandler >                          InvalidHandler;
   typedef ErrorCode< OSStatus, invalidDuration >                         InvalidDuration;
   typedef ErrorCode< OSStatus, invalidTime >                             InvalidTime;
   typedef ErrorCode< OSStatus, cantPutPublicMovieAtom >                  CantPutPublicMovieAtom;
   typedef ErrorCode< OSStatus, badEditList >                             BadEditList;
   typedef ErrorCode< OSStatus, mediaTypesDontMatch >                     MediaTypesDontMatch;
   typedef ErrorCode< OSStatus, progressProcAborted >                     ProgressProcAborted;
   typedef ErrorCode< OSStatus, movieToolboxUninitialized >               MovieToolboxUninitialized;
   typedef ErrorCode< OSStatus, noRecordOfApp >                           NoRecordOfApp;
   typedef ErrorCode< OSStatus, wfFileNotFound >                          WFFileNotFound;
   typedef ErrorCode< OSStatus, cantCreateSingleForkFile >                CantCreateSingleForkFile;
   typedef ErrorCode< OSStatus, invalidEditState >                        InvalidEditState;
   typedef ErrorCode< OSStatus, nonMatchingEditState >                    NonMatchingEditState;
   typedef ErrorCode< OSStatus, staleEditState >                          StaleEditState;
   typedef ErrorCode< OSStatus, userDataItemNotFound >                    UserDataItemNotFound;
   typedef ErrorCode< OSStatus, maxSizeToGrowTooSmall >                   MaxSizeToGrowTooSmall;
   typedef ErrorCode< OSStatus, badTrackIndex >                           BadTrackIndex;
   typedef ErrorCode< OSStatus, trackIDNotFound >                         TrackIDNotFound;
   typedef ErrorCode< OSStatus, trackNotInMovie >                         TrackNotInMovie;
   typedef ErrorCode< OSStatus, timeNotInTrack >                          TimeNotInTrack;
   typedef ErrorCode< OSStatus, timeNotInMedia >                          TimeNotInMedia;
   typedef ErrorCode< OSStatus, badEditIndex >                            BadEditIndex;
   typedef ErrorCode< OSStatus, internalQuickTimeError >                  InternalQuickTimeError;
   typedef ErrorCode< OSStatus, cantEnableTrack >                         CantEnableTrack;
   typedef ErrorCode< OSStatus, invalidRect >                             InvalidRect;
   typedef ErrorCode< OSStatus, invalidSampleNum >                        InvalidSampleNum;
   typedef ErrorCode< OSStatus, invalidChunkNum >                         InvalidChunkNum;
   typedef ErrorCode< OSStatus, invalidSampleDescIndex >                  InvalidSampleDescIndex;
   typedef ErrorCode< OSStatus, invalidChunkCache >                       InvalidChunkCache;
   typedef ErrorCode< OSStatus, invalidSampleDescription >                InvalidSampleDescription;
   typedef ErrorCode< OSStatus, dataNotOpenForRead >                      DataNotOpenForRead;
   typedef ErrorCode< OSStatus, dataNotOpenForWrite >                     DataNotOpenForWrite;
   typedef ErrorCode< OSStatus, dataAlreadyOpenForWrite >                 DataAlreadyOpenForWrite;
   typedef ErrorCode< OSStatus, dataAlreadyClosed >                       DataAlreadyClosed;
   typedef ErrorCode< OSStatus, endOfDataReached >                        EndOfDataReached;
   typedef ErrorCode< OSStatus, dataNoDataRef >                           DataNoDataRef;
   typedef ErrorCode< OSStatus, noMovieFound >                            NoMovieFound;
   typedef ErrorCode< OSStatus, invalidDataRefContainer >                 InvalidDataRefContainer;
   typedef ErrorCode< OSStatus, badDataRefIndex >                         BadDataRefIndex;
   typedef ErrorCode< OSStatus, noDefaultDataRef >                        NoDefaultDataRef;
   typedef ErrorCode< OSStatus, couldNotUseAnExistingSample>              CouldNotUseAnExistingSample;
   typedef ErrorCode< OSStatus, featureUnsupported >                      FeatureUnsupported;
   typedef ErrorCode< OSStatus, noVideoTrackInMovieErr >                  NoVideoTrackInMovieErr;
   typedef ErrorCode< OSStatus, noSoundTrackInMovieErr >                  NoSoundTrackInMovieErr;
   typedef ErrorCode< OSStatus, soundSupportNotAvailableErr>              SoundSupportNotAvailableErr;
   typedef ErrorCode< OSStatus, unsupportedAuxiliaryImportData >          UnsupportedAuxiliaryImportData;
   typedef ErrorCode< OSStatus, auxiliaryExportDataUnavailable >          AuxiliaryExportDataUnavailable;
   typedef ErrorCode< OSStatus, samplesAlreadyInMediaErr >                SamplesAlreadyInMediaErr;
   typedef ErrorCode< OSStatus, noSourceTreeFoundErr >                    NoSourceTreeFoundErr;
   typedef ErrorCode< OSStatus, sourceNotFoundErr >                       SourceNotFoundErr;
   typedef ErrorCode< OSStatus, movieTextNotFoundErr >                    MovieTextNotFoundErr;
   typedef ErrorCode< OSStatus, missingRequiredParameterErr>              MissingRequiredParameterErr;
   typedef ErrorCode< OSStatus, invalidSpriteWorldPropertyErr >           InvalidSpriteWorldPropertyErr;
   typedef ErrorCode< OSStatus, invalidSpritePropertyErr >                InvalidSpritePropertyErr;
   typedef ErrorCode< OSStatus, gWorldsNotSameDepthAndSizeErr >           GWorldsNotSameDepthAndSizeErr;
   typedef ErrorCode< OSStatus, invalidSpriteIndexErr >                   InvalidSpriteIndexErr;
   typedef ErrorCode< OSStatus, invalidImageIndexErr >                    InvalidImageIndexErr;
   typedef ErrorCode< OSStatus, invalidSpriteIDErr >                      InvalidSpriteIDErr;

   typedef ErrorCode< OSStatus, internalComponentErr >                    InternalComponentErr;
   typedef ErrorCode< OSStatus, notImplementedMusicOSErr >                NotImplementedMusicOSErr;
   typedef ErrorCode< OSStatus, cantSendToSynthesizerOSErr >              CantSendToSynthesizerOSErr;
   typedef ErrorCode< OSStatus, cantReceiveFromSynthesizerOSErr >         CantReceiveFromSynthesizerOSErr;
   typedef ErrorCode< OSStatus, illegalVoiceAllocationOSErr>              IllegalVoiceAllocationOSErr;
   typedef ErrorCode< OSStatus, illegalPartOSErr >                        IllegalPartOSErr;
   typedef ErrorCode< OSStatus, illegalChannelOSErr >                     IllegalChannelOSErr;
   typedef ErrorCode< OSStatus, illegalKnobOSErr >                        IllegalKnobOSErr;
   typedef ErrorCode< OSStatus, illegalKnobValueOSErr >                   IllegalKnobValueOSErr;
   typedef ErrorCode< OSStatus, illegalInstrumentOSErr >                  IllegalInstrumentOSErr;
   typedef ErrorCode< OSStatus, illegalControllerOSErr >                  IllegalControllerOSErr;
   typedef ErrorCode< OSStatus, midiManagerAbsentOSErr >                  MIDIManagerAbsentOSErr;
   typedef ErrorCode< OSStatus, synthesizerNotRespondingOSErr >           SynthesizerNotRespondingOSErr;
   typedef ErrorCode< OSStatus, synthesizerOSErr >                        SynthesizerOSErr;
   typedef ErrorCode< OSStatus, illegalNoteChannelOSErr >                 IllegalNoteChannelOSErr;
   typedef ErrorCode< OSStatus, noteChannelNotAllocatedOSErr >            NoteChannelNotAllocatedOSErr;
   typedef ErrorCode< OSStatus, tunePlayerFullOSErr >                     TunePlayerFullOSErr;
   typedef ErrorCode< OSStatus, tuneParseOSErr >                          TuneParseOSErr;
   typedef ErrorCode< OSStatus, noExportProcAvailableErr >                NoExportProcAvailableErr;
   typedef ErrorCode< OSStatus, videoOutputInUseErr >                     VideoOutputInUseErr;

   typedef ErrorCode< OSStatus, componentDllLoadErr >                     ComponentDllLoadErr;
   typedef ErrorCode< OSStatus, componentDllEntryNotFoundErr >            ComponentDllEntryNotFoundErr;
   typedef ErrorCode< OSStatus, qtmlDllLoadErr >                          QTMLDllLoadErr;
   typedef ErrorCode< OSStatus, qtmlDllEntryNotFoundErr >                 QTMLDllEntryNotFoundErr;
   typedef ErrorCode< OSStatus, qtmlUninitialized >                       QTMLUninitialized;
   typedef ErrorCode< OSStatus, unsupportedOSErr >                        UnsupportedOSErr;
   typedef ErrorCode< OSStatus, unsupportedProcessorErr >                 UnsupportedProcessorErr;

   typedef ErrorCode< OSStatus, cannotFindAtomErr >                       CannotFindAtomErr;
   typedef ErrorCode< OSStatus, notLeafAtomErr >                          NotLeafAtomErr;
   typedef ErrorCode< OSStatus, atomsNotOfSameTypeErr >                   AtomsNotOfSameTypeErr;
   typedef ErrorCode< OSStatus, atomIndexInvalidErr >                     AtomIndexInvalidErr;
   typedef ErrorCode< OSStatus, duplicateAtomTypeAndIDErr >               DuplicateAtomTypeAndIDErr;
   typedef ErrorCode< OSStatus, invalidAtomErr >                          InvalidAtomErr;
   typedef ErrorCode< OSStatus, invalidAtomContainerErr >                 InvalidAtomContainerErr;
   typedef ErrorCode< OSStatus, invalidAtomTypeErr >                      InvalidAtomTypeErr;
   typedef ErrorCode< OSStatus, cannotBeLeafAtomErr >                     CannotBeLeafAtomErr;
   typedef ErrorCode< OSStatus, pathTooLongErr >                          PathTooLongErr;
   typedef ErrorCode< OSStatus, emptyPathErr >                            EmptyPathErr;
   typedef ErrorCode< OSStatus, noPathMappingErr >                        NoPathMappingErr;
   typedef ErrorCode< OSStatus, pathNotVerifiedErr >                      PathNotVerifiedErr;
   typedef ErrorCode< OSStatus, unknownFormatErr >                        UnknownFormatErr;
   typedef ErrorCode< OSStatus, wackBadFileErr >                          WACKBadFileErr;
   typedef ErrorCode< OSStatus, wackForkNotFoundErr >                     WACKForkNotFoundErr;
   typedef ErrorCode< OSStatus, wackBadMetaDataErr >                      WACKBadMetaDataErr;
   typedef ErrorCode< OSStatus, qfcbNotFoundErr >                         QFCBNotFoundErr;
   typedef ErrorCode< OSStatus, qfcbNotCreatedErr >                       QFCBNotCreatedErr;
   typedef ErrorCode< OSStatus, AAPNotCreatedErr >                        AAPNotCreatedErr;
   typedef ErrorCode< OSStatus, AAPNotFoundErr >                          AAPNotFoundErr;
   typedef ErrorCode< OSStatus, ASDBadHeaderErr >                         ASDBadHeaderErr;
   typedef ErrorCode< OSStatus, ASDBadForkErr >                           ASDBadForkErr;
   typedef ErrorCode< OSStatus, ASDEntryNotFoundErr >                     ASDEntryNotFoundErr;
   typedef ErrorCode< OSStatus, fileOffsetTooBigErr >                     FileOffsetTooBigErr;
   typedef ErrorCode< OSStatus, notAllowedToSaveMovieErr >                NotAllowedToSaveMovieErr;
   typedef ErrorCode< OSStatus, qtNetworkAlreadyAllocatedErr >            QTNetworkAlreadyAllocatedErr;
   typedef ErrorCode< OSStatus, urlDataHHTTPProtocolErr >                 URLDataHHTTPProtocolErr;
   typedef ErrorCode< OSStatus, urlDataHHTTPNoNetDriverErr >              URLDataHHTTPNoNetDriverErr;
   typedef ErrorCode< OSStatus, urlDataHHTTPURLErr >                      URLDataHHTTPURLErr;
   typedef ErrorCode< OSStatus, urlDataHHTTPRedirectErr >                 URLDataHHTTPRedirectErr;
   typedef ErrorCode< OSStatus, urlDataHFTPProtocolErr >                  URLDataHFTPProtocolErr;
   typedef ErrorCode< OSStatus, urlDataHFTPShutdownErr >                  URLDataHFTPShutdownErr;
   typedef ErrorCode< OSStatus, urlDataHFTPBadUserErr >                   URLDataHFTPBadUserErr;
   typedef ErrorCode< OSStatus, urlDataHFTPBadPasswordErr >               URLDataHFTPBadPasswordErr;
   typedef ErrorCode< OSStatus, urlDataHFTPServerErr >                    URLDataHFTPServerErr;
   typedef ErrorCode< OSStatus, urlDataHFTPDataConnectionErr >            URLDataHFTPDataConnectionErr;
   typedef ErrorCode< OSStatus, urlDataHFTPNoDirectoryErr >               URLDataHFTPNoDirectoryErr;
   typedef ErrorCode< OSStatus, urlDataHFTPQuotaErr >                     URLDataHFTPQuotaErr;
   typedef ErrorCode< OSStatus, urlDataHFTPPermissionsErr >               URLDataHFTPPermissionsErr;
   typedef ErrorCode< OSStatus, urlDataHFTPFilenameErr >                  URLDataHFTPFilenameErr;
   typedef ErrorCode< OSStatus, urlDataHFTPNoNetDriverErr >               URLDataHFTPNoNetDriverErr;
   typedef ErrorCode< OSStatus, urlDataHFTPBadNameListErr >               URLDataHFTPBadNameListErr;
   typedef ErrorCode< OSStatus, urlDataHFTPNeedPasswordErr >              URLDataHFTPNeedPasswordErr;
   typedef ErrorCode< OSStatus, urlDataHFTPNoPasswordErr >                URLDataHFTPNoPasswordErr;
   typedef ErrorCode< OSStatus, urlDataHFTPServerDisconnectedErr >        URLDataHFTPServerDisconnectedErr;
   typedef ErrorCode< OSStatus, urlDataHFTPURLErr >                       URLDataHFTPURLErr;
   typedef ErrorCode< OSStatus, notEnoughDataErr >                        NotEnoughDataErr;
   typedef ErrorCode< OSStatus, qtActionNotHandledErr >                   QTActionNotHandledErr;
   typedef ErrorCode< OSStatus, qtXMLParseErr >                           QTXMLParseErr;
   typedef ErrorCode< OSStatus, qtXMLApplicationErr >                     QTXMLApplicationErr;


   typedef ErrorCode< OSStatus, digiUnimpErr >                            DigiUnimpErr;
   typedef ErrorCode< OSStatus, qtParamErr >                              QTParamErr;
   typedef ErrorCode< OSStatus, matrixErr >                               MatrixErr;
   typedef ErrorCode< OSStatus, notExactMatrixErr >                       NotExactMatrixErr;
   typedef ErrorCode< OSStatus, noMoreKeyColorsErr >                      NoMoreKeyColorsErr;
   typedef ErrorCode< OSStatus, notExactSizeErr >                         NotExactSizeErr;
   typedef ErrorCode< OSStatus, badDepthErr >                             BadDepthErr;
   typedef ErrorCode< OSStatus, noDMAErr >                                NoDMAErr;
   typedef ErrorCode< OSStatus, badCallOrderErr >                         BadCallOrderErr;


/*  Kernel Error Codes  */
   typedef ErrorCode< OSStatus, kernelIncompleteErr >                     KernelIncompleteErr;
   typedef ErrorCode< OSStatus, kernelCanceledErr >                       KernelCanceledErr;
   typedef ErrorCode< OSStatus, kernelOptionsErr >                        KernelOptionsErr;
   typedef ErrorCode< OSStatus, kernelPrivilegeErr >                      KernelPrivilegeErr;
   typedef ErrorCode< OSStatus, kernelUnsupportedErr >                    KernelUnsupportedErr;
   typedef ErrorCode< OSStatus, kernelObjectExistsErr >                   KernelObjectExistsErr;
   typedef ErrorCode< OSStatus, kernelWritePermissionErr >                KernelWritePermissionErr;
   typedef ErrorCode< OSStatus, kernelReadPermissionErr >                 KernelReadPermissionErr;
   typedef ErrorCode< OSStatus, kernelExecutePermissionErr >              KernelExecutePermissionErr;
   typedef ErrorCode< OSStatus, kernelDeletePermissionErr >               KernelDeletePermissionErr;
   typedef ErrorCode< OSStatus, kernelExecutionLevelErr >                 KernelExecutionLevelErr;
   typedef ErrorCode< OSStatus, kernelAttributeErr >                      KernelAttributeErr;
   typedef ErrorCode< OSStatus, kernelAsyncSendLimitErr >                 KernelAsyncSendLimitErr;
   typedef ErrorCode< OSStatus, kernelAsyncReceiveLimitErr >              KernelAsyncReceiveLimitErr;
   typedef ErrorCode< OSStatus, kernelTimeoutErr >                        KernelTimeoutErr;
   typedef ErrorCode< OSStatus, kernelInUseErr >                          KernelInUseErr;
   typedef ErrorCode< OSStatus, kernelTerminatedErr >                     KernelTerminatedErr;
   typedef ErrorCode< OSStatus, kernelExceptionErr >                      KernelExceptionErr;
   typedef ErrorCode< OSStatus, kernelIDErr >                             KernelIDErr;
   typedef ErrorCode< OSStatus, kernelAlreadyFreeErr >                    KernelAlreadyFreeErr;
   typedef ErrorCode< OSStatus, kernelReturnValueErr >                    KernelReturnValueErr;
   typedef ErrorCode< OSStatus, kernelUnrecoverableErr >                  KernelUnrecoverableErr;



/* Text Services Mgr error codes */
   typedef ErrorCode< OSStatus, tsmComponentNoErr >                       TSMComponentNoErr;
   typedef ErrorCode< OSStatus, tsmUnsupScriptLanguageErr >               TSMUnsupScriptLanguageErr;
   typedef ErrorCode< OSStatus, tsmInputMethodNotFoundErr >               TSMInputMethodNotFoundErr;
   typedef ErrorCode< OSStatus, tsmNotAnAppErr >                          TSMNotAnAppErr;
   typedef ErrorCode< OSStatus, tsmAlreadyRegisteredErr >                 TSMAlreadyRegisteredErr;
   typedef ErrorCode< OSStatus, tsmNeverRegisteredErr >                   TSMNeverRegisteredErr;
   typedef ErrorCode< OSStatus, tsmInvalidDocIDErr >                      TSMInvalidDocIDErr;
   typedef ErrorCode< OSStatus, tsmTSMDocBusyErr >                        TSMTSMDocBusyErr;
   typedef ErrorCode< OSStatus, tsmDocNotActiveErr >                      TSMDocNotActiveErr;
   typedef ErrorCode< OSStatus, tsmNoOpenTSErr >                          TSMNoOpenTSErr;
   typedef ErrorCode< OSStatus, tsmCantOpenComponentErr >                 TSMCantOpenComponentErr;
   typedef ErrorCode< OSStatus, tsmTextServiceNotFoundErr >               TSMTextServiceNotFoundErr;
   typedef ErrorCode< OSStatus, tsmDocumentOpenErr >                      TSMDocumentOpenErr;
   typedef ErrorCode< OSStatus, tsmUseInputWindowErr >                    TSMUseInputWindowErr;
   typedef ErrorCode< OSStatus, tsmTSHasNoMenuErr >                       TSMTSHasNoMenuErr;
   typedef ErrorCode< OSStatus, tsmTSNotOpenErr >                         TSMTSNotOpenErr;
   typedef ErrorCode< OSStatus, tsmComponentAlreadyOpenErr >              TSMComponentAlreadyOpenErr;
   typedef ErrorCode< OSStatus, tsmInputMethodIsOldErr >                  TSMInputMethodIsOldErr;
   typedef ErrorCode< OSStatus, tsmScriptHasNoIMErr >                     TSMScriptHasNoIMErr;
   typedef ErrorCode< OSStatus, tsmUnsupportedTypeErr >                   TSMUnsupportedTypeErr;
   typedef ErrorCode< OSStatus, tsmUnknownErr >                           TSMUnknownErr;
   typedef ErrorCode< OSStatus, tsmInvalidContext >                       TSMInvalidContext;
   typedef ErrorCode< OSStatus, tsmNoHandler >                            TSMNoHandler;
   typedef ErrorCode< OSStatus, tsmNoMoreTokens >                         TSMNoMoreTokens;
   typedef ErrorCode< OSStatus, tsmNoStem >                               TSMNoStem;
   typedef ErrorCode< OSStatus, tsmDefaultIsNotInputMethodErr >           TSMDefaultIsNotInputMethodErr;


/* Mixed Mode error codes */
   typedef ErrorCode< OSStatus, mmInternalError >                         MMInternalError;

/* NameRegistry error codes */
   typedef ErrorCode< OSStatus, nrLockedErr >                             NRLockedErr;
   typedef ErrorCode< OSStatus, nrNotEnoughMemoryErr >                    NRNotEnoughMemoryErr;
   typedef ErrorCode< OSStatus, nrInvalidNodeErr >                        NRInvalidNodeErr;
   typedef ErrorCode< OSStatus, nrNotFoundErr >                           NRNotFoundErr;
   typedef ErrorCode< OSStatus, nrNotCreatedErr >                         NRNotCreatedErr;
   typedef ErrorCode< OSStatus, nrNameErr >                               NRNameErr;
   typedef ErrorCode< OSStatus, nrNotSlotDeviceErr >                      NRNotSlotDeviceErr;
   typedef ErrorCode< OSStatus, nrDataTruncatedErr >                      NRDataTruncatedErr;
   typedef ErrorCode< OSStatus, nrPowerErr >                              NRPowerErr;
   typedef ErrorCode< OSStatus, nrPowerSwitchAbortErr >                   NRPowerSwitchAbortErr;
   typedef ErrorCode< OSStatus, nrTypeMismatchErr >                       NRTypeMismatchErr;
   typedef ErrorCode< OSStatus, nrNotModifiedErr >                        NRNotModifiedErr;
   typedef ErrorCode< OSStatus, nrOverrunErr >                            NROverrunErr;
   typedef ErrorCode< OSStatus, nrResultCodeBase >                        NRResultCodeBase;
   typedef ErrorCode< OSStatus, nrPathNotFound >                          NRPathNotFound;
   typedef ErrorCode< OSStatus, nrPathBufferTooSmall >                    NRPathBufferTooSmall;
   typedef ErrorCode< OSStatus, nrInvalidEntryIterationOp >               NRInvalidEntryIterationOp;
   typedef ErrorCode< OSStatus, nrPropertyAlreadyExists >                 NRPropertyAlreadyExists;
   typedef ErrorCode< OSStatus, nrIterationDone >                         NRIterationDone;
   typedef ErrorCode< OSStatus, nrExitedIteratorScope >                   NRExitedIteratorScope;
   typedef ErrorCode< OSStatus, nrTransactionAborted >                    NRTransactionAborted;
   typedef ErrorCode< OSStatus, nrCallNotSupported >                      NRCallNotSupported;

/* Icon Services error codes */
   typedef ErrorCode< OSStatus, invalidIconRefErr >                       InvalidIconRefErr;
   typedef ErrorCode< OSStatus, noSuchIconErr >                           NoSuchIconErr;
   typedef ErrorCode< OSStatus, noIconDataAvailableErr >                  NoIconDataAvailableErr;


/* Dynamic AppleScript errors */
   typedef ErrorCode< OSStatus, errOSACantCoerce >                        ErrOSACantCoerce;
   typedef ErrorCode< OSStatus, errOSACantAccess >                        ErrOSACantAccess;
   typedef ErrorCode< OSStatus, errOSACantAssign >                        ErrOSACantAssign;
   typedef ErrorCode< OSStatus, errOSAGeneralError >                      ErrOSAGeneralError;
   typedef ErrorCode< OSStatus, errOSADivideByZero >                      ErrOSADivideByZero;
   typedef ErrorCode< OSStatus, errOSANumericOverflow >                   ErrOSANumericOverflow;
   typedef ErrorCode< OSStatus, errOSACantLaunch >                        ErrOSACantLaunch;
   typedef ErrorCode< OSStatus, errOSAAppNotHighLevelEventAware >         ErrOSAAppNotHighLevelEventAware;
   typedef ErrorCode< OSStatus, errOSACorruptTerminology >                ErrOSACorruptTerminology;
   typedef ErrorCode< OSStatus, errOSAStackOverflow >                     ErrOSAStackOverflow;
   typedef ErrorCode< OSStatus, errOSAInternalTableOverflow>              ErrOSAInternalTableOverflow;
   typedef ErrorCode< OSStatus, errOSADataBlockTooLarge >                 ErrOSADataBlockTooLarge;
   typedef ErrorCode< OSStatus, errOSACantGetTerminology >                ErrOSACantGetTerminology;
   typedef ErrorCode< OSStatus, errOSACantCreate >                        ErrOSACantCreate;

/* Static AppleScript errors */
   typedef ErrorCode< OSStatus, errOSATypeError >                         ErrOSATypeError;
   typedef ErrorCode< OSStatus, ::OSAMessageNotUnderstood >               OSAMessageNotUnderstood;
   typedef ErrorCode< OSStatus, ::OSAUndefinedHandler >                   OSAUndefinedHandler;
   typedef ErrorCode< OSStatus, ::OSAIllegalAccess >                      OSAIllegalAccess;
   typedef ErrorCode< OSStatus, ::OSAIllegalIndex >                       OSAIllegalIndex;
   typedef ErrorCode< OSStatus, ::OSAIllegalRange >                       OSAIllegalRange;
   typedef ErrorCode< OSStatus, ::OSAIllegalAssign >                      OSAIllegalAssign;
   typedef ErrorCode< OSStatus, ::OSASyntaxError >                        OSASyntaxError;
   typedef ErrorCode< OSStatus, ::OSASyntaxTypeError >                    OSASyntaxTypeError;
   typedef ErrorCode< OSStatus, ::OSATokenTooLong >                       OSATokenTooLong;
   typedef ErrorCode< OSStatus, ::OSAMissingParameter >                   OSAMissingParameter;
   typedef ErrorCode< OSStatus, ::OSAParameterMismatch >                  OSAParameterMismatch;
   typedef ErrorCode< OSStatus, ::OSADuplicateParameter >                 OSADuplicateParameter;
   typedef ErrorCode< OSStatus, ::OSADuplicateProperty >                  OSADuplicateProperty;
   typedef ErrorCode< OSStatus, ::OSADuplicateHandler >                   OSADuplicateHandler;
   typedef ErrorCode< OSStatus, ::OSAUndefinedVariable >                  OSAUndefinedVariable;
   typedef ErrorCode< OSStatus, ::OSAInconsistentDeclarations>            OSAInconsistentDeclarations;
   typedef ErrorCode< OSStatus, ::OSAControlFlowError >                   OSAControlFlowError;

/* Runtime errors: */
   typedef ErrorCode< OSStatus, errASCantConsiderAndIgnore >              ErrASCantConsiderAndIgnore;
   typedef ErrorCode< OSStatus, errASCantCompareMoreThan32k>              ErrASCantCompareMoreThan32k;
   typedef ErrorCode< OSStatus, errASTerminologyNestingTooDeep >          ErrASTerminologyNestingTooDeep;
   typedef ErrorCode< OSStatus, errASIllegalFormalParameter>              ErrASIllegalFormalParameter;
   typedef ErrorCode< OSStatus, errASParameterNotForEvent >               ErrASParameterNotForEvent;
   typedef ErrorCode< OSStatus, errASNoResultReturned >                   ErrASNoResultReturned;
   typedef ErrorCode< OSStatus, errASInconsistentNames >                  ErrASInconsistentNames;


/* Code Fragment Manager errors */
   typedef ErrorCode< OSStatus, cfragFirstErrCode >                       CFragFirstErrCode;
   typedef ErrorCode< OSStatus, cfragContextIDErr >                       CFragContextIDErr;
   typedef ErrorCode< OSStatus, cfragConnectionIDErr >                    CFragConnectionIDErr;
   typedef ErrorCode< OSStatus, cfragNoSymbolErr >                        CFragNoSymbolErr;
   typedef ErrorCode< OSStatus, cfragNoSectionErr >                       CFragNoSectionErr;
   typedef ErrorCode< OSStatus, cfragNoLibraryErr >                       CFragNoLibraryErr;
   typedef ErrorCode< OSStatus, cfragDupRegistrationErr >                 CFragDupRegistrationErr;
   typedef ErrorCode< OSStatus, cfragFragmentFormatErr >                  CFragFragmentFormatErr;
   typedef ErrorCode< OSStatus, cfragUnresolvedErr >                      CFragUnresolvedErr;
   typedef ErrorCode< OSStatus, cfragNoPositionErr >                      CFragNoPositionErr;
   typedef ErrorCode< OSStatus, cfragNoPrivateMemErr >                    CFragNoPrivateMemErr;
   typedef ErrorCode< OSStatus, cfragNoClientMemErr >                     CFragNoClientMemErr;
   typedef ErrorCode< OSStatus, cfragNoIDsErr >                           CFragNoIDsErr;
   typedef ErrorCode< OSStatus, cfragInitOrderErr >                       CFragInitOrderErr;
   typedef ErrorCode< OSStatus, cfragImportTooOldErr >                    CFragImportTooOldErr;
   typedef ErrorCode< OSStatus, cfragImportTooNewErr >                    CFragImportTooNewErr;
   typedef ErrorCode< OSStatus, cfragInitLoopErr >                        CFragInitLoopErr;
   typedef ErrorCode< OSStatus, cfragInitAtBootErr >                      CFragInitAtBootErr;
   typedef ErrorCode< OSStatus, cfragLibConnErr >                         CFragLibConnErr;
   typedef ErrorCode< OSStatus, cfragCFMStartupErr >                      CFragCFMStartupErr;
   typedef ErrorCode< OSStatus, cfragCFMInternalErr >                     CFragCFMInternalErr;
   typedef ErrorCode< OSStatus, cfragFragmentCorruptErr >                 CFragFragmentCorruptErr;
   typedef ErrorCode< OSStatus, cfragInitFunctionErr >                    CFragInitFunctionErr;
   typedef ErrorCode< OSStatus, cfragNoApplicationErr >                   CFragNoApplicationErr;
   typedef ErrorCode< OSStatus, cfragArchitectureErr >                    CFragArchitectureErr;
   typedef ErrorCode< OSStatus, cfragFragmentUsageErr >                   CFragFragmentUsageErr;
   typedef ErrorCode< OSStatus, cfragFileSizeErr >                        CFragFileSizeErr;
   typedef ErrorCode< OSStatus, cfragNotClosureErr >                      CFragNotClosureErr;
   typedef ErrorCode< OSStatus, cfragNoRegistrationErr >                  CFragNoRegistrationErr;
   typedef ErrorCode< OSStatus, cfragContainerIDErr >                     CFragContainerIDErr;
   typedef ErrorCode< OSStatus, cfragClosureIDErr >                       CFragClosureIDErr;
   typedef ErrorCode< OSStatus, cfragAbortClosureErr >                    CFragAbortClosureErr;
   typedef ErrorCode< OSStatus, cfragOutputLengthErr >                    CFragOutputLengthErr;
   typedef ErrorCode< OSStatus, cfragLastErrCode >                        CFragLastErrCode;

/*Component Manager & component errors*/
   typedef ErrorCode< OSStatus, invalidComponentID >                      InvalidComponentID;
   typedef ErrorCode< OSStatus, validInstancesExist >                     ValidInstancesExist;
   typedef ErrorCode< OSStatus, componentNotCaptured >                    ComponentNotCaptured;
   typedef ErrorCode< OSStatus, componentDontRegister >                   ComponentDontRegister;
   typedef ErrorCode< OSStatus, unresolvedComponentDLLErr >               UnresolvedComponentDLLErr;
   typedef ErrorCode< OSStatus, retryComponentRegistrationErr >           RetryComponentRegistrationErr;

/*Translation manager & Translation components*/
   typedef ErrorCode< OSStatus, invalidTranslationPathErr >               InvalidTranslationPathErr;
   typedef ErrorCode< OSStatus, couldNotParseSourceFileErr >              CouldNotParseSourceFileErr;
   typedef ErrorCode< OSStatus, noTranslationPathErr >                    NoTranslationPathErr;
   typedef ErrorCode< OSStatus, badTranslationSpecErr >                   BadTranslationSpecErr;
   typedef ErrorCode< OSStatus, noPrefAppErr >                            NoPrefAppErr;

   typedef ErrorCode< OSStatus, buf2SmallErr >                            Buf2SmallErr;
   typedef ErrorCode< OSStatus, noMPPErr >                                NoMPPErr;
   typedef ErrorCode< OSStatus, ckSumErr >                                CKSumErr;
   typedef ErrorCode< OSStatus, extractErr >                              ExtractErr;
   typedef ErrorCode< OSStatus, readQErr >                                ReadQErr;
   typedef ErrorCode< OSStatus, atpLenErr >                               ATPLenErr;
   typedef ErrorCode< OSStatus, atpBadRsp >                               ATPBadRsp;
   typedef ErrorCode< OSStatus, recNotFnd >                               RecNotFnd;
   typedef ErrorCode< OSStatus, sktClosedErr >                            SktClosedErr;


/* OpenTransport errors*/
   typedef ErrorCode< OSStatus, kOTNoError >                              OTNoError;
   typedef ErrorCode< OSStatus, kOTOutOfMemoryErr >                       OTOutOfMemoryErr;
   typedef ErrorCode< OSStatus, kOTNotFoundErr >                          OTNotFoundErr;
   typedef ErrorCode< OSStatus, kOTDuplicateFoundErr >                    OTDuplicateFoundErr;
   typedef ErrorCode< OSStatus, kOTBadAddressErr >                        OTBadAddressErr;
   typedef ErrorCode< OSStatus, kOTBadOptionErr >                         OTBadOptionErr;
   typedef ErrorCode< OSStatus, kOTAccessErr >                            OTAccessErr;
   typedef ErrorCode< OSStatus, kOTBadReferenceErr >                      OTBadReferenceErr;
   typedef ErrorCode< OSStatus, kOTNoAddressErr >                         OTNoAddressErr;
   typedef ErrorCode< OSStatus, kOTOutStateErr >                          OTOutStateErr;
   typedef ErrorCode< OSStatus, kOTBadSequenceErr >                       OTBadSequenceErr;
   typedef ErrorCode< OSStatus, kOTSysErrorErr >                          OTSysErrorErr;
   typedef ErrorCode< OSStatus, kOTLookErr >                              OTLookErr;
   typedef ErrorCode< OSStatus, kOTBadDataErr >                           OTBadDataErr;
   typedef ErrorCode< OSStatus, kOTBufferOverflowErr >                    OTBufferOverflowErr;
   typedef ErrorCode< OSStatus, kOTFlowErr >                              OTFlowErr;
   typedef ErrorCode< OSStatus, kOTNoDataErr >                            OTNoDataErr;
   typedef ErrorCode< OSStatus, kOTNoDisconnectErr >                      OTNoDisconnectErr;
   typedef ErrorCode< OSStatus, kOTNoUDErrErr >                           OTNoUDErrErr;
   typedef ErrorCode< OSStatus, kOTBadFlagErr >                           OTBadFlagErr;
   typedef ErrorCode< OSStatus, kOTNoReleaseErr >                         OTNoReleaseErr;
   typedef ErrorCode< OSStatus, kOTNotSupportedErr >                      OTNotSupportedErr;
   typedef ErrorCode< OSStatus, kOTStateChangeErr >                       OTStateChangeErr;
   typedef ErrorCode< OSStatus, kOTNoStructureTypeErr >                   OTNoStructureTypeErr;
   typedef ErrorCode< OSStatus, kOTBadNameErr >                           OTBadNameErr;
   typedef ErrorCode< OSStatus, kOTBadQLenErr >                           OTBadQLenErr;
   typedef ErrorCode< OSStatus, kOTAddressBusyErr >                       OTAddressBusyErr;
   typedef ErrorCode< OSStatus, kOTIndOutErr >                            OTIndOutErr;
   typedef ErrorCode< OSStatus, kOTProviderMismatchErr >                  OTProviderMismatchErr;
   typedef ErrorCode< OSStatus, kOTResQLenErr >                           OTResQLenErr;
   typedef ErrorCode< OSStatus, kOTResAddressErr >                        OTResAddressErr;
   typedef ErrorCode< OSStatus, kOTQFullErr >                             OTQFullErr;
   typedef ErrorCode< OSStatus, kOTProtocolErr >                          OTProtocolErr;
   typedef ErrorCode< OSStatus, kOTBadSyncErr >                           OTBadSyncErr;
   typedef ErrorCode< OSStatus, kOTCanceledErr >                          OTCanceledErr;
   typedef ErrorCode< OSStatus, kEPERMErr >                               EPERMErr;
   typedef ErrorCode< OSStatus, kENOENTErr >                              ENOENTErr;
   typedef ErrorCode< OSStatus, kENORSRCErr >                             ENORSRCErr;
   typedef ErrorCode< OSStatus, kEINTRErr >                               EINTRErr;
   typedef ErrorCode< OSStatus, kEIOErr >                                 EIOErr;
   typedef ErrorCode< OSStatus, kENXIOErr >                               ENXIOErr;
   typedef ErrorCode< OSStatus, kEBADFErr >                               EBADFErr;
   typedef ErrorCode< OSStatus, kEAGAINErr >                              EAGAINErr;
   typedef ErrorCode< OSStatus, kENOMEMErr >                              ENOMEMErr;
   typedef ErrorCode< OSStatus, kEACCESErr >                              EACCESErr;
   typedef ErrorCode< OSStatus, kEFAULTErr >                              EFAULTErr;
   typedef ErrorCode< OSStatus, kEBUSYErr >                               EBUSYErr;
   typedef ErrorCode< OSStatus, kEEXISTErr >                              EEXISTErr;
   typedef ErrorCode< OSStatus, kENODEVErr >                              ENODEVErr;
   typedef ErrorCode< OSStatus, kEINVALErr >                              EINVALErr;
   typedef ErrorCode< OSStatus, kENOTTYErr >                              ENOTTYErr;
   typedef ErrorCode< OSStatus, kEPIPEErr >                               EPIPEErr;
   typedef ErrorCode< OSStatus, kERANGEErr >                              ERANGEErr;
   typedef ErrorCode< OSStatus, kEWOULDBLOCKErr >                         EWOULDBLOCKErr;
   typedef ErrorCode< OSStatus, kEDEADLKErr >                             EDEADLKErr;
   typedef ErrorCode< OSStatus, kEALREADYErr >                            EALREADYErr;
   typedef ErrorCode< OSStatus, kENOTSOCKErr >                            ENOTSOCKErr;
   typedef ErrorCode< OSStatus, kEDESTADDRREQErr >                        EDESTADDRREQErr;
   typedef ErrorCode< OSStatus, kEMSGSIZEErr >                            EMSGSIZEErr;
   typedef ErrorCode< OSStatus, kEPROTOTYPEErr >                          EPROTOTYPEErr;
   typedef ErrorCode< OSStatus, kENOPROTOOPTErr >                         ENOPROTOOPTErr;
   typedef ErrorCode< OSStatus, kEPROTONOSUPPORTErr >                     EPROTONOSUPPORTErr;
   typedef ErrorCode< OSStatus, kESOCKTNOSUPPORTErr >                     ESOCKTNOSUPPORTErr;
   typedef ErrorCode< OSStatus, kEOPNOTSUPPErr >                          EOPNOTSUPPErr;
   typedef ErrorCode< OSStatus, kEADDRINUSEErr >                          EADDRINUSEErr;
   typedef ErrorCode< OSStatus, kEADDRNOTAVAILErr >                       EADDRNOTAVAILErr;
   typedef ErrorCode< OSStatus, kENETDOWNErr >                            ENETDOWNErr;
   typedef ErrorCode< OSStatus, kENETUNREACHErr >                         ENETUNREACHErr;
   typedef ErrorCode< OSStatus, kENETRESETErr >                           ENETRESETErr;
   typedef ErrorCode< OSStatus, kECONNABORTEDErr >                        ECONNABORTEDErr;
   typedef ErrorCode< OSStatus, kECONNRESETErr >                          ECONNRESETErr;
   typedef ErrorCode< OSStatus, kENOBUFSErr >                             ENOBUFSErr;
   typedef ErrorCode< OSStatus, kEISCONNErr >                             EISCONNErr;
   typedef ErrorCode< OSStatus, kENOTCONNErr >                            ENOTCONNErr;
   typedef ErrorCode< OSStatus, kESHUTDOWNErr >                           ESHUTDOWNErr;
   typedef ErrorCode< OSStatus, kETOOMANYREFSErr >                        ETOOMANYREFSErr;
   typedef ErrorCode< OSStatus, kETIMEDOUTErr >                           ETIMEDOUTErr;
   typedef ErrorCode< OSStatus, kECONNREFUSEDErr >                        ECONNREFUSEDErr;
   typedef ErrorCode< OSStatus, kEHOSTDOWNErr >                           EHOSTDOWNErr;
   typedef ErrorCode< OSStatus, kEHOSTUNREACHErr >                        EHOSTUNREACHErr;
   typedef ErrorCode< OSStatus, kEPROTOErr >                              EPROTOErr;
   typedef ErrorCode< OSStatus, kETIMEErr >                               ETIMEErr;
   typedef ErrorCode< OSStatus, kENOSRErr >                               ENOSRErr;
   typedef ErrorCode< OSStatus, kEBADMSGErr >                             EBADMSGErr;
   typedef ErrorCode< OSStatus, kECANCELErr >                             ECANCELErr;
   typedef ErrorCode< OSStatus, kENOSTRErr >                              ENOSTRErr;
   typedef ErrorCode< OSStatus, kENODATAErr >                             ENODATAErr;
   typedef ErrorCode< OSStatus, kEINPROGRESSErr >                         EINPROGRESSErr;
   typedef ErrorCode< OSStatus, kESRCHErr >                               ESRCHErr;
   typedef ErrorCode< OSStatus, kENOMSGErr >                              ENOMSGErr;
   typedef ErrorCode< OSStatus, kOTClientNotInittedErr >                  OTClientNotInittedErr;
   typedef ErrorCode< OSStatus, kOTPortHasDiedErr >                       OTPortHasDiedErr;
   typedef ErrorCode< OSStatus, kOTPortWasEjectedErr >                    OTPortWasEjectedErr;
   typedef ErrorCode< OSStatus, kOTBadConfigurationErr >                  OTBadConfigurationErr;
   typedef ErrorCode< OSStatus, kOTConfigurationChangedErr >              OTConfigurationChangedErr;
   typedef ErrorCode< OSStatus, kOTUserRequestedErr >                     OTUserRequestedErr;
   typedef ErrorCode< OSStatus, kOTPortLostConnection >                   OTPortLostConnection;


/* Color Picker errors*/
   typedef ErrorCode< OSStatus, firstPickerError >                        FirstPickerError;
   typedef ErrorCode< OSStatus, invalidPickerType >                       InvalidPickerType;
   typedef ErrorCode< OSStatus, requiredFlagsDontMatch >                  RequiredFlagsDontMatch;
   typedef ErrorCode< OSStatus, pickerResourceError >                     PickerResourceError;
   typedef ErrorCode< OSStatus, cantLoadPicker >                          CantLoadPicker;
   typedef ErrorCode< OSStatus, cantCreatePickerWindow >                  CantCreatePickerWindow;
   typedef ErrorCode< OSStatus, cantLoadPackage >                         CantLoadPackage;
   typedef ErrorCode< OSStatus, pickerCantLive >                          PickerCantLive;
   typedef ErrorCode< OSStatus, colorSyncNotInstalled >                   ColorSyncNotInstalled;
   typedef ErrorCode< OSStatus, badProfileError >                         BadProfileError;
   typedef ErrorCode< OSStatus, noHelpForItem >                           NoHelpForItem;



/* NSL error codes*/
   typedef ErrorCode< OSStatus, kNSL68kContextNotSupported >              NSL68kContextNotSupported;
   typedef ErrorCode< OSStatus, kNSLSchedulerError >                      NSLSchedulerError;
   typedef ErrorCode< OSStatus, kNSLBadURLSyntax >                        NSLBadURLSyntax;
   typedef ErrorCode< OSStatus, kNSLNoCarbonLib >                         NSLNoCarbonLib;
   typedef ErrorCode< OSStatus, kNSLUILibraryNotAvailable >               NSLUILibraryNotAvailable;
   typedef ErrorCode< OSStatus, kNSLNotImplementedYet >                   NSLNotImplementedYet;
   typedef ErrorCode< OSStatus, kNSLErrNullPtrError >                     NSLErrNullPtrError;
   typedef ErrorCode< OSStatus, kNSLSomePluginsFailedToLoad>              NSLSomePluginsFailedToLoad;
   typedef ErrorCode< OSStatus, kNSLNullNeighborhoodPtr >                 NSLNullNeighborhoodPtr;
   typedef ErrorCode< OSStatus, kNSLNoPluginsForSearch >                  NSLNoPluginsForSearch;
   typedef ErrorCode< OSStatus, kNSLSearchAlreadyInProgress>              NSLSearchAlreadyInProgress;
   typedef ErrorCode< OSStatus, kNSLNoPluginsFound >                      NSLNoPluginsFound;
   typedef ErrorCode< OSStatus, kNSLPluginLoadFailed >                    NSLPluginLoadFailed;
   typedef ErrorCode< OSStatus, kNSLBadProtocolTypeErr >                  NSLBadProtocolTypeErr;
   typedef ErrorCode< OSStatus, kNSLNullListPtr >                         NSLNullListPtr;
   typedef ErrorCode< OSStatus, kNSLBadClientInfoPtr >                    NSLBadClientInfoPtr;
   typedef ErrorCode< OSStatus, kNSLCannotContinueLookup >                NSLCannotContinueLookup;
   typedef ErrorCode< OSStatus, kNSLBufferTooSmallForData >               NSLBufferTooSmallForData;
   typedef ErrorCode< OSStatus, kNSLNoContextAvailable >                  NSLNoContextAvailable;
   typedef ErrorCode< OSStatus, kNSLRequestBufferAlreadyInList >          NSLRequestBufferAlreadyInList;
   typedef ErrorCode< OSStatus, kNSLInvalidPluginSpec >                   NSLInvalidPluginSpec;
   typedef ErrorCode< OSStatus, kNSLNoSupportForService >                 NSLNoSupportForService;
   typedef ErrorCode< OSStatus, kNSLBadNetConnection >                    NSLBadNetConnection;
   typedef ErrorCode< OSStatus, kNSLBadDataTypeErr >                      NSLBadDataTypeErr;
   typedef ErrorCode< OSStatus, kNSLBadServiceTypeErr >                   NSLBadServiceTypeErr;
   typedef ErrorCode< OSStatus, kNSLBadReferenceErr >                     NSLBadReferenceErr;
   typedef ErrorCode< OSStatus, kNSLNoElementsInList >                    NSLNoElementsInList;
   typedef ErrorCode< OSStatus, kNSLInsufficientOTVer >                   NSLInsufficientOTVer;
   typedef ErrorCode< OSStatus, kNSLInsufficientSysVer >                  NSLInsufficientSysVer;
   typedef ErrorCode< OSStatus, kNSLNotInitialized >                      NSLNotInitialized;
   typedef ErrorCode< OSStatus, kNSLInitializationFailed >                NSLInitializationFailed;



/* desktop printing error codes*/
   typedef ErrorCode< OSStatus, kDTPHoldJobErr >                          DTPHoldJobErr;
   typedef ErrorCode< OSStatus, kDTPStopQueueErr >                        DTPStopQueueErr;
   typedef ErrorCode< OSStatus, kDTPTryAgainErr >                         DTPTryAgainErr;
   typedef ErrorCode< OSStatus, kDTPAbortJobErr >                         DTPAbortJobErr;


/* ColorSync Result codes */
/* Profile Access Errors */
   typedef ErrorCode< OSStatus, cmElementTagNotFound >                    CMElementTagNotFound;
   typedef ErrorCode< OSStatus, cmIndexRangeErr >                         CMIndexRangeErr;
   typedef ErrorCode< OSStatus, cmCantDeleteElement >                     CMCantDeleteElement;
   typedef ErrorCode< OSStatus, cmFatalProfileErr >                       CMFatalProfileErr;
   typedef ErrorCode< OSStatus, cmInvalidProfile >                        CMInvalidProfile;
   typedef ErrorCode< OSStatus, cmInvalidProfileLocation >                CMInvalidProfileLocation;
   typedef ErrorCode< OSStatus, cmCantCopyModifiedV1Profile>              CMCantCopyModifiedV1Profile;
/* Profile Search Errors */
   typedef ErrorCode< OSStatus, cmInvalidSearch >                         CMInvalidSearch;
   typedef ErrorCode< OSStatus, cmSearchError >                           CMSearchError;
   typedef ErrorCode< OSStatus, cmErrIncompatibleProfile >                CMErrIncompatibleProfile;
   typedef ErrorCode< OSStatus, cmInvalidColorSpace >                     CMInvalidColorSpace;
   typedef ErrorCode< OSStatus, cmInvalidSrcMap >                         CMInvalidSrcMap;
   typedef ErrorCode< OSStatus, cmInvalidDstMap >                         CMInvalidDstMap;
   typedef ErrorCode< OSStatus, cmNoGDevicesError >                       CMNoGDevicesError;
   typedef ErrorCode< OSStatus, cmInvalidProfileComment >                 CMInvalidProfileComment;
   typedef ErrorCode< OSStatus, cmRangeOverFlow >                         CMRangeOverFlow;
   typedef ErrorCode< OSStatus, cmNamedColorNotFound >                    CMNamedColorNotFound;
   typedef ErrorCode< OSStatus, cmCantGamutCheckError >                   CMCantGamutCheckError;

/* new Folder Manager error codes */
   typedef ErrorCode< OSStatus, badFolderDescErr >                        BadFolderDescErr;
   typedef ErrorCode< OSStatus, duplicateFolderDescErr >                  DuplicateFolderDescErr;
   typedef ErrorCode< OSStatus, noMoreFolderDescErr >                     NoMoreFolderDescErr;
   typedef ErrorCode< OSStatus, invalidFolderTypeErr >                    InvalidFolderTypeErr;
   typedef ErrorCode< OSStatus, duplicateRoutingErr >                     DuplicateRoutingErr;
   typedef ErrorCode< OSStatus, routingNotFoundErr >                      RoutingNotFoundErr;
   typedef ErrorCode< OSStatus, badRoutingSizeErr >                       BadRoutingSizeErr;


/* Core Foundation errors*/
   typedef ErrorCode< OSStatus, coreFoundationUnknownErr >                CoreFoundationUnknownErr;

/* ScrapMgr error codes (CarbonLib 1.0 and later)*/
   typedef ErrorCode< OSStatus, internalScrapErr >                        InternalScrapErr;
   typedef ErrorCode< OSStatus, duplicateScrapFlavorErr >                 DuplicateScrapFlavorErr;
   typedef ErrorCode< OSStatus, badScrapRefErr >                          BadScrapRefErr;
   typedef ErrorCode< OSStatus, processStateIncorrectErr >                ProcessStateIncorrectErr;
   typedef ErrorCode< OSStatus, scrapPromiseNotKeptErr >                  ScrapPromiseNotKeptErr;
   typedef ErrorCode< OSStatus, noScrapPromiseKeeperErr >                 NoScrapPromiseKeeperErr;
   typedef ErrorCode< OSStatus, nilScrapFlavorDataErr >                   NilScrapFlavorDataErr;
   typedef ErrorCode< OSStatus, scrapFlavorFlagsMismatchErr>              ScrapFlavorFlagsMismatchErr;
   typedef ErrorCode< OSStatus, scrapFlavorSizeMismatchErr >              ScrapFlavorSizeMismatchErr;
   typedef ErrorCode< OSStatus, illegalScrapFlavorFlagsErr >              IllegalScrapFlavorFlagsErr;
   typedef ErrorCode< OSStatus, illegalScrapFlavorTypeErr >               IllegalScrapFlavorTypeErr;
   typedef ErrorCode< OSStatus, illegalScrapFlavorSizeErr >               IllegalScrapFlavorSizeErr;
   typedef ErrorCode< OSStatus, scrapFlavorNotFoundErr >                  ScrapFlavorNotFoundErr;
   typedef ErrorCode< OSStatus, needClearScrapErr >                       NeedClearScrapErr;


/*  AFP Protocol Errors */
   typedef ErrorCode< OSStatus, afpAccessDenied >                         AFPAccessDenied;
   typedef ErrorCode< OSStatus, afpAuthContinue >                         AFPAuthContinue;
   typedef ErrorCode< OSStatus, afpBadUAM >                               AFPBadUAM;
   typedef ErrorCode< OSStatus, afpBadVersNum >                           AFPBadVersNum;
   typedef ErrorCode< OSStatus, afpBitmapErr >                            AFPBitmapErr;
   typedef ErrorCode< OSStatus, afpCantMove >                             AFPCantMove;
   typedef ErrorCode< OSStatus, afpDenyConflict >                         AFPDenyConflict;
   typedef ErrorCode< OSStatus, afpDirNotEmpty >                          AFPDirNotEmpty;
   typedef ErrorCode< OSStatus, afpDiskFull >                             AFPDiskFull;
   typedef ErrorCode< OSStatus, afpEofError >                             AFPEofError;
   typedef ErrorCode< OSStatus, afpFileBusy >                             AFPFileBusy;
   typedef ErrorCode< OSStatus, afpFlatVol >                              AFPFlatVol;
   typedef ErrorCode< OSStatus, afpItemNotFound >                         AFPItemNotFound;
   typedef ErrorCode< OSStatus, afpLockErr >                              AFPLockErr;
   typedef ErrorCode< OSStatus, afpMiscErr >                              AFPMiscErr;
   typedef ErrorCode< OSStatus, afpNoMoreLocks >                          AFPNoMoreLocks;
   typedef ErrorCode< OSStatus, afpNoServer >                             AFPNoServer;
   typedef ErrorCode< OSStatus, afpObjectExists >                         AFPObjectExists;
   typedef ErrorCode< OSStatus, afpObjectNotFound >                       AFPObjectNotFound;
   typedef ErrorCode< OSStatus, afpParmErr >                              AFPParmErr;
   typedef ErrorCode< OSStatus, afpRangeNotLocked >                       AFPRangeNotLocked;
   typedef ErrorCode< OSStatus, afpRangeOverlap >                         AFPRangeOverlap;
   typedef ErrorCode< OSStatus, afpSessClosed >                           AFPSessClosed;
   typedef ErrorCode< OSStatus, afpUserNotAuth >                          AFPUserNotAuth;
   typedef ErrorCode< OSStatus, afpCallNotSupported >                     AFPCallNotSupported;
   typedef ErrorCode< OSStatus, afpObjectTypeErr >                        AFPObjectTypeErr;
   typedef ErrorCode< OSStatus, afpTooManyFilesOpen >                     AFPTooManyFilesOpen;
   typedef ErrorCode< OSStatus, afpServerGoingDown >                      AFPServerGoingDown;
   typedef ErrorCode< OSStatus, afpCantRename >                           AFPCantRename;
   typedef ErrorCode< OSStatus, afpDirNotFound >                          AFPDirNotFound;
   typedef ErrorCode< OSStatus, afpIconTypeError >                        AFPIconTypeError;
   typedef ErrorCode< OSStatus, afpVolLocked >                            AFPVolLocked;
   typedef ErrorCode< OSStatus, afpObjectLocked >                         AFPObjectLocked;
   typedef ErrorCode< OSStatus, afpContainsSharedErr >                    AFPContainsSharedErr;
   typedef ErrorCode< OSStatus, afpIDNotFound >                           AFPIDNotFound;
   typedef ErrorCode< OSStatus, afpIDExists >                             AFPIDExists;
   typedef ErrorCode< OSStatus, afpDiffVolErr >                           AFPDiffVolErr;
   typedef ErrorCode< OSStatus, afpCatalogChanged >                       AFPCatalogChanged;
   typedef ErrorCode< OSStatus, afpSameObjectErr >                        AFPSameObjectErr;
   typedef ErrorCode< OSStatus, afpBadIDErr >                             AFPBadIDErr;
   typedef ErrorCode< OSStatus, afpPwdSameErr >                           AFPPwdSameErr;
   typedef ErrorCode< OSStatus, afpPwdTooShortErr >                       AFPPwdTooShortErr;
   typedef ErrorCode< OSStatus, afpPwdExpiredErr >                        AFPPwdExpiredErr;
   typedef ErrorCode< OSStatus, afpInsideSharedErr >                      AFPInsideSharedErr;
   typedef ErrorCode< OSStatus, afpInsideTrashErr >                       AFPInsideTrashErr;
   typedef ErrorCode< OSStatus, afpPwdNeedsChangeErr >                    AFPPwdNeedsChangeErr;
   typedef ErrorCode< OSStatus, afpPwdPolicyErr >                         AFPPwdPolicyErr;
   typedef ErrorCode< OSStatus, afpAlreadyLoggedInErr >                   AFPAlreadyLoggedInErr;
   typedef ErrorCode< OSStatus, afpCallNotAllowed >                       AFPCallNotAllowed;

/*  AppleShare Client Errors */
   typedef ErrorCode< OSStatus, afpBadDirIDType >                         AFPBadDirIDType;
   typedef ErrorCode< OSStatus, afpCantMountMoreSrvre >                   AFPCantMountMoreSrvre;
   typedef ErrorCode< OSStatus, afpAlreadyMounted >                       AFPAlreadyMounted;
   typedef ErrorCode< OSStatus, afpSameNodeErr >                          AFPSameNodeErr;

/*Text Engines, TSystemTextEngines, HIEditText error coded*/
   typedef ErrorCode< OSStatus, errUnknownAttributeTag >                  ErrUnknownAttributeTag;
   typedef ErrorCode< OSStatus, errMarginWilllNotFit >                    ErrMarginWilllNotFit;
   typedef ErrorCode< OSStatus, errNotInImagingMode >                     ErrNotInImagingMode;
   typedef ErrorCode< OSStatus, errAlreadyInImagingMode >                 ErrAlreadyInImagingMode;
   typedef ErrorCode< OSStatus, errEngineNotFound >                       ErrEngineNotFound;
   typedef ErrorCode< OSStatus, errIteratorReachedEnd >                   ErrIteratorReachedEnd;
   typedef ErrorCode< OSStatus, errInvalidRange >                         ErrInvalidRange;
   typedef ErrorCode< OSStatus, errOffsetNotOnElementBounday >            ErrOffsetNotOnElementBounday;
   typedef ErrorCode< OSStatus, errNoHiliteText >                         ErrNoHiliteText;
   typedef ErrorCode< OSStatus, errEmptyScrap >                           ErrEmptyScrap;
   typedef ErrorCode< OSStatus, errReadOnlyText >                         ErrReadOnlyText;
   typedef ErrorCode< OSStatus, errUnknownElement >                       ErrUnknownElement;
   typedef ErrorCode< OSStatus, errNonContiuousAttribute >                ErrNonContiuousAttribute;
   typedef ErrorCode< OSStatus, errCannotUndo >                           ErrCannotUndo;

/* HTMLRendering OSStaus codes*/
   typedef ErrorCode< OSStatus, hrHTMLRenderingLibNotInstalledErr >       HRHTMLRenderingLibNotInstalledErr;
   typedef ErrorCode< OSStatus, hrMiscellaneousExceptionErr>              HRMiscellaneousExceptionErr;
   typedef ErrorCode< OSStatus, hrUnableToResizeHandleErr >               HRUnableToResizeHandleErr;
   typedef ErrorCode< OSStatus, hrURLNotHandledErr >                      HRURLNotHandledErr;

/* IAExtractor result codes */
   typedef ErrorCode< OSStatus, errIANoErr >                              ErrIANoErr;
   typedef ErrorCode< OSStatus, errIAUnknownErr >                         ErrIAUnknownErr;
   typedef ErrorCode< OSStatus, errIAAllocationErr >                      ErrIAAllocationErr;
   typedef ErrorCode< OSStatus, errIAParamErr >                           ErrIAParamErr;
   typedef ErrorCode< OSStatus, errIANoMoreItems >                        ErrIANoMoreItems;
   typedef ErrorCode< OSStatus, errIABufferTooSmall >                     ErrIABufferTooSmall;
   typedef ErrorCode< OSStatus, errIACanceled >                           ErrIACanceled;
   typedef ErrorCode< OSStatus, errIAInvalidDocument >                    ErrIAInvalidDocument;
   typedef ErrorCode< OSStatus, errIATextExtractionErr >                  ErrIATextExtractionErr;
   typedef ErrorCode< OSStatus, errIAEndOfTextRun >                       ErrIAEndOfTextRun;

/* QuickTime Streaming Errors */
   typedef ErrorCode< OSStatus, qtsBadSelectorErr >                       QTSBadSelectorErr;
   typedef ErrorCode< OSStatus, qtsBadStateErr >                          QTSBadStateErr;
   typedef ErrorCode< OSStatus, qtsBadDataErr >                           QTSBadDataErr;
   typedef ErrorCode< OSStatus, qtsUnsupportedDataTypeErr >               QTSUnsupportedDataTypeErr;
   typedef ErrorCode< OSStatus, qtsUnsupportedRateErr >                   QTSUnsupportedRateErr;
   typedef ErrorCode< OSStatus, qtsUnsupportedFeatureErr >                QTSUnsupportedFeatureErr;
   typedef ErrorCode< OSStatus, qtsTooMuchDataErr >                       QTSTooMuchDataErr;
   typedef ErrorCode< OSStatus, qtsUnknownValueErr >                      QTSUnknownValueErr;
   typedef ErrorCode< OSStatus, qtsTimeoutErr >                           QTSTimeoutErr;
   typedef ErrorCode< OSStatus, qtsConnectionFailedErr >                  QTSConnectionFailedErr;
   typedef ErrorCode< OSStatus, qtsAddressBusyErr >                       QTSAddressBusyErr;

/*Gestalt error codes*/
   typedef ErrorCode< OSStatus, gestaltUnknownErr >                       GestaltUnknownErr;
   typedef ErrorCode< OSStatus, gestaltUndefSelectorErr >                 GestaltUndefSelectorErr;
   typedef ErrorCode< OSStatus, gestaltDupSelectorErr >                   GestaltDupSelectorErr;
   typedef ErrorCode< OSStatus, gestaltLocationErr >                      GestaltLocationErr;


/* Menu Manager error codes*/
   typedef ErrorCode< OSStatus, menuPropertyInvalidErr >                  MenuPropertyInvalidErr;
   typedef ErrorCode< OSStatus, menuPropertyInvalid >                     MenuPropertyInvalid;
   typedef ErrorCode< OSStatus, menuPropertyNotFoundErr >                 MenuPropertyNotFoundErr;
   typedef ErrorCode< OSStatus, menuNotFoundErr >                         MenuNotFoundErr;
   typedef ErrorCode< OSStatus, menuUsesSystemDefErr >                    MenuUsesSystemDefErr;
   typedef ErrorCode< OSStatus, menuItemNotFoundErr >                     MenuItemNotFoundErr;
   typedef ErrorCode< OSStatus, menuInvalidErr >                          MenuInvalidErr;


/* Window Manager error codes*/
   typedef ErrorCode< OSStatus, errInvalidWindowPtr >                     ErrInvalidWindowPtr;
   typedef ErrorCode< OSStatus, errInvalidWindowRef >                     ErrInvalidWindowRef;
   typedef ErrorCode< OSStatus, errUnsupportedWindowAttributesForClass >  ErrUnsupportedWindowAttributesForClass;
   typedef ErrorCode< OSStatus, errWindowDoesNotHaveProxy >               ErrWindowDoesNotHaveProxy;
   typedef ErrorCode< OSStatus, errInvalidWindowProperty >                ErrInvalidWindowProperty;
   typedef ErrorCode< OSStatus, errWindowPropertyNotFound >               ErrWindowPropertyNotFound;
   typedef ErrorCode< OSStatus, errUnrecognizedWindowClass >              ErrUnrecognizedWindowClass;
   typedef ErrorCode< OSStatus, errCorruptWindowDescription>              ErrCorruptWindowDescription;
   typedef ErrorCode< OSStatus, errUserWantsToDragWindow >                ErrUserWantsToDragWindow;
   typedef ErrorCode< OSStatus, errWindowsAlreadyInitialized >            ErrWindowsAlreadyInitialized;
   typedef ErrorCode< OSStatus, errFloatingWindowsNotInitialized >        ErrFloatingWindowsNotInitialized;
   typedef ErrorCode< OSStatus, errWindowNotFound >                       ErrWindowNotFound;
   typedef ErrorCode< OSStatus, errWindowDoesNotFitOnscreen>              ErrWindowDoesNotFitOnscreen;
   typedef ErrorCode< OSStatus, windowAttributeImmutableErr>              WindowAttributeImmutableErr;
   typedef ErrorCode< OSStatus, windowAttributesConflictErr>              WindowAttributesConflictErr;
   typedef ErrorCode< OSStatus, windowManagerInternalErr >                WindowManagerInternalErr;
   typedef ErrorCode< OSStatus, windowWrongStateErr >                     WindowWrongStateErr;
   typedef ErrorCode< OSStatus, windowGroupInvalidErr >                   WindowGroupInvalidErr;
   typedef ErrorCode< OSStatus, windowAppModalStateAlreadyExistsErr >     WindowAppModalStateAlreadyExistsErr;
   typedef ErrorCode< OSStatus, windowNoAppModalStateErr >                WindowNoAppModalStateErr;
   typedef ErrorCode< OSStatus, errWindowDoesntSupportFocus>              ErrWindowDoesntSupportFocus;
   typedef ErrorCode< OSStatus, errWindowRegionCodeInvalid >              ErrWindowRegionCodeInvalid;

/* Dialog Mgr error codes*/
   typedef ErrorCode< OSStatus, dialogNoTimeoutErr >                      DialogNoTimeoutErr;

/* NavigationLib error codes*/
   typedef ErrorCode< OSStatus, kNavWrongDialogStateErr >                 NavWrongDialogStateErr;
   typedef ErrorCode< OSStatus, kNavWrongDialogClassErr >                 NavWrongDialogClassErr;
   typedef ErrorCode< OSStatus, kNavInvalidSystemConfigErr >              NavInvalidSystemConfigErr;
   typedef ErrorCode< OSStatus, kNavCustomControlMessageFailedErr >       NavCustomControlMessageFailedErr;
   typedef ErrorCode< OSStatus, kNavInvalidCustomControlMessageErr >      NavInvalidCustomControlMessageErr;
   typedef ErrorCode< OSStatus, kNavMissingKindStringErr >                NavMissingKindStringErr;


/* Collection Manager errors */
   typedef ErrorCode< OSStatus, collectionItemLockedErr >                 CollectionItemLockedErr;
   typedef ErrorCode< OSStatus, collectionItemNotFoundErr >               CollectionItemNotFoundErr;
   typedef ErrorCode< OSStatus, collectionIndexRangeErr >                 CollectionIndexRangeErr;
   typedef ErrorCode< OSStatus, collectionVersionErr >                    CollectionVersionErr;


/* QuickTime Streaming Server Errors */
   typedef ErrorCode< OSStatus, kQTSSUnknownErr >                         QTSSUnknownErr;


/* Display Manager error codes (-6220...-6269)*/
   typedef ErrorCode< OSStatus, kDMGenErr >                               DMGenErr;
   typedef ErrorCode< OSStatus, kDMMirroringOnAlready >                   DMMirroringOnAlready;
   typedef ErrorCode< OSStatus, kDMWrongNumberOfDisplays >                DMWrongNumberOfDisplays;
   typedef ErrorCode< OSStatus, kDMMirroringBlocked >                     DMMirroringBlocked;
   typedef ErrorCode< OSStatus, kDMCantBlock >                            DMCantBlock;
   typedef ErrorCode< OSStatus, kDMMirroringNotOn >                       DMMirroringNotOn;
   typedef ErrorCode< OSStatus, kSysSWTooOld >                            SysSWTooOld;
   typedef ErrorCode< OSStatus, kDMSWNotInitializedErr >                  DMSWNotInitializedErr;
   typedef ErrorCode< OSStatus, kDMDriverNotDisplayMgrAwareErr >          DMDriverNotDisplayMgrAwareErr;
   typedef ErrorCode< OSStatus, kDMDisplayNotFoundErr >                   DMDisplayNotFoundErr;
   typedef ErrorCode< OSStatus, kDMNotFoundErr >                          DMNotFoundErr;
   typedef ErrorCode< OSStatus, kDMDisplayAlreadyInstalledErr >           DMDisplayAlreadyInstalledErr;
   typedef ErrorCode< OSStatus, kDMMainDisplayCannotMoveErr>              DMMainDisplayCannotMoveErr;
   typedef ErrorCode< OSStatus, kDMNoDeviceTableclothErr >                DMNoDeviceTableclothErr;
   typedef ErrorCode< OSStatus, kDMFoundErr >                             DMFoundErr;


/* Language Analysis error codes */
   typedef ErrorCode< OSStatus, laTooSmallBufferErr >                     LATooSmallBufferErr;
   typedef ErrorCode< OSStatus, laEnvironmentBusyErr >                    LAEnvironmentBusyErr;
   typedef ErrorCode< OSStatus, laEnvironmentNotFoundErr >                LAEnvironmentNotFoundErr;
   typedef ErrorCode< OSStatus, laEnvironmentExistErr >                   LAEnvironmentExistErr;
   typedef ErrorCode< OSStatus, laInvalidPathErr >                        LAInvalidPathErr;
   typedef ErrorCode< OSStatus, laNoMoreMorphemeErr >                     LANoMoreMorphemeErr;
   typedef ErrorCode< OSStatus, laFailAnalysisErr >                       LAFailAnalysisErr;
   typedef ErrorCode< OSStatus, laTextOverFlowErr >                       LATextOverFlowErr;
   typedef ErrorCode< OSStatus, laDictionaryNotOpenedErr >                LADictionaryNotOpenedErr;
   typedef ErrorCode< OSStatus, laDictionaryUnknownErr >                  LADictionaryUnknownErr;
   typedef ErrorCode< OSStatus, laDictionaryTooManyErr >                  LADictionaryTooManyErr;
   typedef ErrorCode< OSStatus, laPropertyValueErr >                      LAPropertyValueErr;
   typedef ErrorCode< OSStatus, laPropertyUnknownErr >                    LAPropertyUnknownErr;
   typedef ErrorCode< OSStatus, laPropertyIsReadOnlyErr >                 LAPropertyIsReadOnlyErr;
   typedef ErrorCode< OSStatus, laPropertyNotFoundErr >                   LAPropertyNotFoundErr;
   typedef ErrorCode< OSStatus, laPropertyErr >                           LAPropertyErr;
   typedef ErrorCode< OSStatus, laEngineNotFoundErr >                     LAEngineNotFoundErr;


   typedef ErrorCode< OSStatus, kUSBNoErr >                               USBNoErr;
   typedef ErrorCode< OSStatus, kUSBNoTran >                              USBNoTran;
   typedef ErrorCode< OSStatus, kUSBNoDelay >                             USBNoDelay;
   typedef ErrorCode< OSStatus, kUSBPending >                             USBPending;

/* USB Hardware Errors */
   typedef ErrorCode< OSStatus, kUSBNotSent2Err >                         USBNotSent2Err;
   typedef ErrorCode< OSStatus, kUSBNotSent1Err >                         USBNotSent1Err;
   typedef ErrorCode< OSStatus, kUSBBufUnderRunErr >                      USBBufUnderRunErr;
   typedef ErrorCode< OSStatus, kUSBBufOvrRunErr >                        USBBufOvrRunErr;
   typedef ErrorCode< OSStatus, kUSBRes2Err >                             USBRes2Err;
   typedef ErrorCode< OSStatus, kUSBRes1Err >                             USBRes1Err;
   typedef ErrorCode< OSStatus, kUSBUnderRunErr >                         USBUnderRunErr;
   typedef ErrorCode< OSStatus, kUSBOverRunErr >                          USBOverRunErr;
   typedef ErrorCode< OSStatus, kUSBWrongPIDErr >                         USBWrongPIDErr;
   typedef ErrorCode< OSStatus, kUSBPIDCheckErr >                         USBPIDCheckErr;
   typedef ErrorCode< OSStatus, kUSBNotRespondingErr >                    USBNotRespondingErr;
   typedef ErrorCode< OSStatus, kUSBEndpointStallErr >                    USBEndpointStallErr;
   typedef ErrorCode< OSStatus, kUSBDataToggleErr >                       USBDataToggleErr;
   typedef ErrorCode< OSStatus, kUSBBitstufErr >                          USBBitstufErr;
   typedef ErrorCode< OSStatus, kUSBCRCErr >                              USBCRCErr;
   typedef ErrorCode< OSStatus, kUSBLinkErr >                             USBLinkErr;


/* USB Manager Errors */
   typedef ErrorCode< OSStatus, kUSBQueueFull >                           USBQueueFull;
   typedef ErrorCode< OSStatus, kUSBNotHandled >                          USBNotHandled;
   typedef ErrorCode< OSStatus, kUSBUnknownNotification >                 USBUnknownNotification;
   typedef ErrorCode< OSStatus, kUSBBadDispatchTable >                    USBBadDispatchTable;


/* USB Services Errors */
   typedef ErrorCode< OSStatus, kUSBPortDisabled >                        USBPortDisabled;
   typedef ErrorCode< OSStatus, kUSBQueueAborted >                        USBQueueAborted;
   typedef ErrorCode< OSStatus, kUSBTimedOut >                            USBTimedOut;
   typedef ErrorCode< OSStatus, kUSBDeviceDisconnected >                  USBDeviceDisconnected;
   typedef ErrorCode< OSStatus, kUSBDeviceNotSuspended >                  USBDeviceNotSuspended;
   typedef ErrorCode< OSStatus, kUSBDeviceSuspended >                     USBDeviceSuspended;
   typedef ErrorCode< OSStatus, kUSBInvalidBuffer >                       USBInvalidBuffer;
   typedef ErrorCode< OSStatus, kUSBDevicePowerProblem >                  USBDevicePowerProblem;
   typedef ErrorCode< OSStatus, kUSBDeviceBusy >                          USBDeviceBusy;
   typedef ErrorCode< OSStatus, kUSBUnknownInterfaceErr >                 USBUnknownInterfaceErr;
   typedef ErrorCode< OSStatus, kUSBPipeStalledError >                    USBPipeStalledError;
   typedef ErrorCode< OSStatus, kUSBPipeIdleError >                       USBPipeIdleError;
   typedef ErrorCode< OSStatus, kUSBNoBandwidthError >                    USBNoBandwidthError;
   typedef ErrorCode< OSStatus, kUSBAbortedError >                        USBAbortedError;
   typedef ErrorCode< OSStatus, kUSBFlagsError >                          USBFlagsError;
   typedef ErrorCode< OSStatus, kUSBCompletionError >                     USBCompletionError;
   typedef ErrorCode< OSStatus, kUSBPBLengthError >                       USBPBLengthError;
   typedef ErrorCode< OSStatus, kUSBPBVersionError >                      USBPBVersionError;
   typedef ErrorCode< OSStatus, kUSBNotFound >                            USBNotFound;
   typedef ErrorCode< OSStatus, kUSBOutOfMemoryErr >                      USBOutOfMemoryErr;
   typedef ErrorCode< OSStatus, kUSBDeviceErr >                           USBDeviceErr;
   typedef ErrorCode< OSStatus, kUSBNoDeviceErr >                         USBNoDeviceErr;
   typedef ErrorCode< OSStatus, kUSBAlreadyOpenErr >                      USBAlreadyOpenErr;
   typedef ErrorCode< OSStatus, kUSBTooManyTransactionsErr >              USBTooManyTransactionsErr;
   typedef ErrorCode< OSStatus, kUSBUnknownRequestErr >                   USBUnknownRequestErr;
   typedef ErrorCode< OSStatus, kUSBRqErr >                               USBRqErr;
   typedef ErrorCode< OSStatus, kUSBIncorrectTypeErr >                    USBIncorrectTypeErr;
   typedef ErrorCode< OSStatus, kUSBTooManyPipesErr >                     USBTooManyPipesErr;
   typedef ErrorCode< OSStatus, kUSBUnknownPipeErr >                      USBUnknownPipeErr;
   typedef ErrorCode< OSStatus, kUSBUnknownDeviceErr >                    USBUnknownDeviceErr;
   typedef ErrorCode< OSStatus, kUSBInternalErr >                         USBInternalErr;


/* DictionaryMgr error codes */
   typedef ErrorCode< OSStatus, dcmParamErr >                             DCMParamErr;
   typedef ErrorCode< OSStatus, dcmNotDictionaryErr >                     DCMNotDictionaryErr;
   typedef ErrorCode< OSStatus, dcmBadDictionaryErr >                     DCMBadDictionaryErr;
   typedef ErrorCode< OSStatus, dcmPermissionErr >                        DCMPermissionErr;
   typedef ErrorCode< OSStatus, dcmDictionaryNotOpenErr >                 DCMDictionaryNotOpenErr;
   typedef ErrorCode< OSStatus, dcmDictionaryBusyErr >                    DCMDictionaryBusyErr;
   typedef ErrorCode< OSStatus, dcmBlockFullErr >                         DCMBlockFullErr;
   typedef ErrorCode< OSStatus, dcmNoRecordErr >                          DCMNoRecordErr;
   typedef ErrorCode< OSStatus, dcmDupRecordErr >                         DCMDupRecordErr;
   typedef ErrorCode< OSStatus, dcmNecessaryFieldErr >                    DCMNecessaryFieldErr;
   typedef ErrorCode< OSStatus, dcmBadFieldInfoErr >                      DCMBadFieldInfoErr;
   typedef ErrorCode< OSStatus, dcmBadFieldTypeErr >                      DCMBadFieldTypeErr;
   typedef ErrorCode< OSStatus, dcmNoFieldErr >                           DCMNoFieldErr;
   typedef ErrorCode< OSStatus, dcmBadKeyErr >                            DCMBadKeyErr;
   typedef ErrorCode< OSStatus, dcmTooManyKeyErr >                        DCMTooManyKeyErr;
   typedef ErrorCode< OSStatus, dcmBadDataSizeErr >                       DCMBadDataSizeErr;
   typedef ErrorCode< OSStatus, dcmBadFindMethodErr >                     DCMBadFindMethodErr;
   typedef ErrorCode< OSStatus, dcmBadPropertyErr >                       DCMBadPropertyErr;
   typedef ErrorCode< OSStatus, dcmProtectedErr >                         DCMProtectedErr;
   typedef ErrorCode< OSStatus, dcmNoAccessMethodErr >                    DCMNoAccessMethodErr;
   typedef ErrorCode< OSStatus, dcmBadFeatureErr >                        DCMBadFeatureErr;
   typedef ErrorCode< OSStatus, dcmIterationCompleteErr >                 DCMIterationCompleteErr;
   typedef ErrorCode< OSStatus, dcmBufferOverflowErr >                    DCMBufferOverflowErr;


/* Apple Remote Access error codes*/
   typedef ErrorCode< OSStatus, kRAInvalidParameter >                     RAInvalidParameter;
   typedef ErrorCode< OSStatus, kRAInvalidPort >                          RAInvalidPort;
   typedef ErrorCode< OSStatus, kRAStartupFailed >                        RAStartupFailed;
   typedef ErrorCode< OSStatus, kRAPortSetupFailed >                      RAPortSetupFailed;
   typedef ErrorCode< OSStatus, kRAOutOfMemory >                          RAOutOfMemory;
   typedef ErrorCode< OSStatus, kRANotSupported >                         RANotSupported;
   typedef ErrorCode< OSStatus, kRAMissingResources >                     RAMissingResources;
   typedef ErrorCode< OSStatus, kRAIncompatiblePrefs >                    RAIncompatiblePrefs;
   typedef ErrorCode< OSStatus, kRANotConnected >                         RANotConnected;
   typedef ErrorCode< OSStatus, kRAConnectionCanceled >                   RAConnectionCanceled;
   typedef ErrorCode< OSStatus, kRAUnknownUser >                          RAUnknownUser;
   typedef ErrorCode< OSStatus, kRAInvalidPassword >                      RAInvalidPassword;
   typedef ErrorCode< OSStatus, kRAInternalError >                        RAInternalError;
   typedef ErrorCode< OSStatus, kRAInstallationDamaged >                  RAInstallationDamaged;
   typedef ErrorCode< OSStatus, kRAPortBusy >                             RAPortBusy;
   typedef ErrorCode< OSStatus, kRAUnknownPortState >                     RAUnknownPortState;
   typedef ErrorCode< OSStatus, kRAInvalidPortState >                     RAInvalidPortState;
   typedef ErrorCode< OSStatus, kRAInvalidSerialProtocol >                RAInvalidSerialProtocol;
   typedef ErrorCode< OSStatus, kRAUserLoginDisabled >                    RAUserLoginDisabled;
   typedef ErrorCode< OSStatus, kRAUserPwdChangeRequired >                RAUserPwdChangeRequired;
   typedef ErrorCode< OSStatus, kRAUserPwdEntryRequired >                 RAUserPwdEntryRequired;
   typedef ErrorCode< OSStatus, kRAUserInteractionRequired >              RAUserInteractionRequired;
   typedef ErrorCode< OSStatus, kRAInitOpenTransportFailed >              RAInitOpenTransportFailed;
   typedef ErrorCode< OSStatus, kRARemoteAccessNotReady >                 RARemoteAccessNotReady;
   typedef ErrorCode< OSStatus, kRATCPIPInactive >                        RATCPIPInactive;
   typedef ErrorCode< OSStatus, kRATCPIPNotConfigured >                   RATCPIPNotConfigured;
   typedef ErrorCode< OSStatus, kRANotPrimaryInterface >                  RANotPrimaryInterface;
   typedef ErrorCode< OSStatus, kRAConfigurationDBInitErr >               RAConfigurationDBInitErr;
   typedef ErrorCode< OSStatus, kRAPPPProtocolRejected >                  RAPPPProtocolRejected;
   typedef ErrorCode< OSStatus, kRAPPPAuthenticationFailed >              RAPPPAuthenticationFailed;
   typedef ErrorCode< OSStatus, kRAPPPNegotiationFailed >                 RAPPPNegotiationFailed;
   typedef ErrorCode< OSStatus, kRAPPPUserDisconnected >                  RAPPPUserDisconnected;
   typedef ErrorCode< OSStatus, kRAPPPPeerDisconnected >                  RAPPPPeerDisconnected;
   typedef ErrorCode< OSStatus, kRAPeerNotResponding >                    RAPeerNotResponding;
   typedef ErrorCode< OSStatus, kRAATalkInactive >                        RAATalkInactive;
   typedef ErrorCode< OSStatus, kRAExtAuthenticationFailed >              RAExtAuthenticationFailed;
   typedef ErrorCode< OSStatus, kRANCPRejectedbyPeer >                    RANCPRejectedbyPeer;
   typedef ErrorCode< OSStatus, kRADuplicateIPAddr >                      RADuplicateIPAddr;
   typedef ErrorCode< OSStatus, kRACallBackFailed >                       RACallBackFailed;
   typedef ErrorCode< OSStatus, kRANotEnabled >                           RANotEnabled;

/* ATSUnicode error codes*/
   typedef ErrorCode< OSStatus, kATSUInvalidTextLayoutErr >               ATSUInvalidTextLayoutErr;
   typedef ErrorCode< OSStatus, kATSUInvalidStyleErr >                    ATSUInvalidStyleErr;
   typedef ErrorCode< OSStatus, kATSUInvalidTextRangeErr >                ATSUInvalidTextRangeErr;
   typedef ErrorCode< OSStatus, kATSUFontsMatched >                       ATSUFontsMatched;
   typedef ErrorCode< OSStatus, kATSUFontsNotMatched >                    ATSUFontsNotMatched;
   typedef ErrorCode< OSStatus, kATSUNoCorrespondingFontErr>              ATSUNoCorrespondingFontErr;
   typedef ErrorCode< OSStatus, kATSUInvalidFontErr >                     ATSUInvalidFontErr;
   typedef ErrorCode< OSStatus, kATSUInvalidAttributeValueErr >           ATSUInvalidAttributeValueErr;
   typedef ErrorCode< OSStatus, kATSUInvalidAttributeSizeErr >            ATSUInvalidAttributeSizeErr;
   typedef ErrorCode< OSStatus, kATSUInvalidAttributeTagErr>              ATSUInvalidAttributeTagErr;
   typedef ErrorCode< OSStatus, kATSUInvalidCacheErr >                    ATSUInvalidCacheErr;
   typedef ErrorCode< OSStatus, kATSUNotSetErr >                          ATSUNotSetErr;
   typedef ErrorCode< OSStatus, kATSUNoStyleRunsAssignedErr>              ATSUNoStyleRunsAssignedErr;
   typedef ErrorCode< OSStatus, kATSUQuickDrawTextErr >                   ATSUQuickDrawTextErr;
   typedef ErrorCode< OSStatus, kATSULowLevelErr >                        ATSULowLevelErr;
   typedef ErrorCode< OSStatus, kATSUNoFontCmapAvailableErr>              ATSUNoFontCmapAvailableErr;
   typedef ErrorCode< OSStatus, kATSUNoFontScalerAvailableErr >           ATSUNoFontScalerAvailableErr;
   typedef ErrorCode< OSStatus, kATSUCoordinateOverflowErr >              ATSUCoordinateOverflowErr;
   typedef ErrorCode< OSStatus, kATSULineBreakInWord >                    ATSULineBreakInWord;
   typedef ErrorCode< OSStatus, kATSUBusyObjectErr >                      ATSUBusyObjectErr;
   typedef ErrorCode< OSStatus, kATSUInvalidFontFallbacksErr >            ATSUInvalidFontFallbacksErr;
   typedef ErrorCode< OSStatus, kATSULastErr >                            ATSULastErr;


/* general text errors*/
   typedef ErrorCode< OSStatus, kTextUnsupportedEncodingErr>              TextUnsupportedEncodingErr;
   typedef ErrorCode< OSStatus, kTextMalformedInputErr >                  TextMalformedInputErr;
   typedef ErrorCode< OSStatus, kTextUndefinedElementErr >                TextUndefinedElementErr;
   typedef ErrorCode< OSStatus, kTECMissingTableErr >                     TECMissingTableErr;
   typedef ErrorCode< OSStatus, kTECTableChecksumErr >                    TECTableChecksumErr;
   typedef ErrorCode< OSStatus, kTECTableFormatErr >                      TECTableFormatErr;
   typedef ErrorCode< OSStatus, kTECCorruptConverterErr >                 TECCorruptConverterErr;
   typedef ErrorCode< OSStatus, kTECNoConversionPathErr >                 TECNoConversionPathErr;
   typedef ErrorCode< OSStatus, kTECBufferBelowMinimumSizeErr >           TECBufferBelowMinimumSizeErr;
   typedef ErrorCode< OSStatus, kTECArrayFullErr >                        TECArrayFullErr;
   typedef ErrorCode< OSStatus, kTECBadTextRunErr >                       TECBadTextRunErr;
   typedef ErrorCode< OSStatus, kTECPartialCharErr >                      TECPartialCharErr;
   typedef ErrorCode< OSStatus, kTECUnmappableElementErr >                TECUnmappableElementErr;
   typedef ErrorCode< OSStatus, kTECIncompleteElementErr >                TECIncompleteElementErr;
   typedef ErrorCode< OSStatus, kTECDirectionErr >                        TECDirectionErr;
   typedef ErrorCode< OSStatus, kTECGlobalsUnavailableErr >               TECGlobalsUnavailableErr;
   typedef ErrorCode< OSStatus, kTECItemUnavailableErr >                  TECItemUnavailableErr;

/* text conversion status codes*/
   typedef ErrorCode< OSStatus, kTECUsedFallbacksStatus >                 TECUsedFallbacksStatus;
   typedef ErrorCode< OSStatus, kTECNeedFlushStatus >                     TECNeedFlushStatus;
   typedef ErrorCode< OSStatus, kTECOutputBufferFullStatus >              TECOutputBufferFullStatus;

/* deprecated error & status codes for low-level converter*/
   typedef ErrorCode< OSStatus, unicodeChecksumErr >                      UnicodeChecksumErr;
   typedef ErrorCode< OSStatus, unicodeNoTableErr >                       UnicodeNoTableErr;
   typedef ErrorCode< OSStatus, unicodeVariantErr >                       UnicodeVariantErr;
   typedef ErrorCode< OSStatus, unicodeFallbacksErr >                     UnicodeFallbacksErr;
   typedef ErrorCode< OSStatus, unicodePartConvertErr >                   UnicodePartConvertErr;
   typedef ErrorCode< OSStatus, unicodeBufErr >                           UnicodeBufErr;
   typedef ErrorCode< OSStatus, unicodeCharErr >                          UnicodeCharErr;
   typedef ErrorCode< OSStatus, unicodeElementErr >                       UnicodeElementErr;
   typedef ErrorCode< OSStatus, unicodeNotFoundErr >                      UnicodeNotFoundErr;
   typedef ErrorCode< OSStatus, unicodeTableFormatErr >                   UnicodeTableFormatErr;
   typedef ErrorCode< OSStatus, unicodeDirectionErr >                     UnicodeDirectionErr;
   typedef ErrorCode< OSStatus, unicodeContextualErr >                    UnicodeContextualErr;
   typedef ErrorCode< OSStatus, unicodeTextEncodingDataErr >              UnicodeTextEncodingDataErr;


/* UTCUtils Status Codes */
   typedef ErrorCode< OSStatus, kUTCUnderflowErr >                        UTCUnderflowErr;
   typedef ErrorCode< OSStatus, kUTCOverflowErr >                         UTCOverflowErr;
   typedef ErrorCode< OSStatus, kIllegalClockValueErr >                   IllegalClockValueErr;


   typedef ErrorCode< OSStatus, codecErr >                                CodecErr;
   typedef ErrorCode< OSStatus, noCodecErr >                              NoCodecErr;
   typedef ErrorCode< OSStatus, codecUnimpErr >                           CodecUnimpErr;
   typedef ErrorCode< OSStatus, codecSizeErr >                            CodecSizeErr;
   typedef ErrorCode< OSStatus, codecScreenBufErr >                       CodecScreenBufErr;
   typedef ErrorCode< OSStatus, codecImageBufErr >                        CodecImageBufErr;
   typedef ErrorCode< OSStatus, codecSpoolErr >                           CodecSpoolErr;
   typedef ErrorCode< OSStatus, codecAbortErr >                           CodecAbortErr;
   typedef ErrorCode< OSStatus, codecWouldOffscreenErr >                  CodecWouldOffscreenErr;
   typedef ErrorCode< OSStatus, codecBadDataErr >                         CodecBadDataErr;
   typedef ErrorCode< OSStatus, codecDataVersErr >                        CodecDataVersErr;
   typedef ErrorCode< OSStatus, codecExtensionNotFoundErr >               CodecExtensionNotFoundErr;
   typedef ErrorCode< OSStatus, scTypeNotFoundErr >                       ScTypeNotFoundErr;
   typedef ErrorCode< OSStatus, codecConditionErr >                       CodecConditionErr;
   typedef ErrorCode< OSStatus, codecOpenErr >                            CodecOpenErr;
   typedef ErrorCode< OSStatus, codecCantWhenErr >                        CodecCantWhenErr;
   typedef ErrorCode< OSStatus, codecCantQueueErr >                       CodecCantQueueErr;
   typedef ErrorCode< OSStatus, codecNothingToBlitErr >                   CodecNothingToBlitErr;
   typedef ErrorCode< OSStatus, codecNoMemoryPleaseWaitErr >              CodecNoMemoryPleaseWaitErr;
   typedef ErrorCode< OSStatus, codecDisabledErr >                        CodecDisabledErr;
   typedef ErrorCode< OSStatus, codecNeedToFlushChainErr >                CodecNeedToFlushChainErr;
   typedef ErrorCode< OSStatus, lockPortBitsBadSurfaceErr >               LockPortBitsBadSurfaceErr;
   typedef ErrorCode< OSStatus, lockPortBitsWindowMovedErr >              LockPortBitsWindowMovedErr;
   typedef ErrorCode< OSStatus, lockPortBitsWindowResizedErr >            LockPortBitsWindowResizedErr;
   typedef ErrorCode< OSStatus, lockPortBitsWindowClippedErr >            LockPortBitsWindowClippedErr;
   typedef ErrorCode< OSStatus, lockPortBitsBadPortErr >                  LockPortBitsBadPortErr;
   typedef ErrorCode< OSStatus, lockPortBitsSurfaceLostErr >              LockPortBitsSurfaceLostErr;
   typedef ErrorCode< OSStatus, codecParameterDialogConfirm>              CodecParameterDialogConfirm;
   typedef ErrorCode< OSStatus, codecNeedAccessKeyErr >                   CodecNeedAccessKeyErr;
   typedef ErrorCode< OSStatus, codecOffscreenFailedErr >                 CodecOffscreenFailedErr;
   typedef ErrorCode< OSStatus, codecDroppedFrameErr >                    CodecDroppedFrameErr;
   typedef ErrorCode< OSStatus, directXObjectAlreadyExists >              DirectXObjectAlreadyExists;
   typedef ErrorCode< OSStatus, lockPortBitsWrongGDeviceErr>              LockPortBitsWrongGDeviceErr;
   typedef ErrorCode< OSStatus, codecOffscreenFailedPleaseRetryErr >      CodecOffscreenFailedPleaseRetryErr;


/* PCCard error codes */
   typedef ErrorCode< OSStatus, kBadAdapterErr >                          BadAdapterErr;
   typedef ErrorCode< OSStatus, kBadAttributeErr >                        BadAttributeErr;
   typedef ErrorCode< OSStatus, kBadBaseErr >                             BadBaseErr;
   typedef ErrorCode< OSStatus, kBadEDCErr >                              BadEDCErr;
   typedef ErrorCode< OSStatus, kBadIRQErr >                              BadIRQErr;
   typedef ErrorCode< OSStatus, kBadOffsetErr >                           BadOffsetErr;
   typedef ErrorCode< OSStatus, kBadPageErr >                             BadPageErr;
   typedef ErrorCode< OSStatus, kBadSizeErr >                             BadSizeErr;
   typedef ErrorCode< OSStatus, kBadSocketErr >                           BadSocketErr;
   typedef ErrorCode< OSStatus, kBadTypeErr >                             BadTypeErr;
   typedef ErrorCode< OSStatus, kBadVccErr >                              BadVccErr;
   typedef ErrorCode< OSStatus, kBadVppErr >                              BadVppErr;
   typedef ErrorCode< OSStatus, kBadWindowErr >                           BadWindowErr;
   typedef ErrorCode< OSStatus, kBadArgLengthErr >                        BadArgLengthErr;
   typedef ErrorCode< OSStatus, kBadArgsErr >                             BadArgsErr;
   typedef ErrorCode< OSStatus, kBadHandleErr >                           BadHandleErr;
   typedef ErrorCode< OSStatus, kBadCISErr >                              BadCISErr;
   typedef ErrorCode< OSStatus, kBadSpeedErr >                            BadSpeedErr;
   typedef ErrorCode< OSStatus, kReadFailureErr >                         ReadFailureErr;
   typedef ErrorCode< OSStatus, kWriteFailureErr >                        WriteFailureErr;
   typedef ErrorCode< OSStatus, kGeneralFailureErr >                      GeneralFailureErr;
   typedef ErrorCode< OSStatus, kNoCardErr >                              NoCardErr;
   typedef ErrorCode< OSStatus, kUnsupportedFunctionErr >                 UnsupportedFunctionErr;
   typedef ErrorCode< OSStatus, kUnsupportedModeErr >                     UnsupportedModeErr;
   typedef ErrorCode< OSStatus, kBusyErr >                                BusyErr;
   typedef ErrorCode< OSStatus, kWriteProtectedErr >                      WriteProtectedErr;
   typedef ErrorCode< OSStatus, kConfigurationLockedErr >                 ConfigurationLockedErr;
   typedef ErrorCode< OSStatus, kInUseErr >                               InUseErr;
   typedef ErrorCode< OSStatus, kNoMoreItemsErr >                         NoMoreItemsErr;
   typedef ErrorCode< OSStatus, kOutOfResourceErr >                       OutOfResourceErr;
   typedef ErrorCode< OSStatus, kNoCardSevicesSocketsErr >                NoCardSevicesSocketsErr;
   typedef ErrorCode< OSStatus, kInvalidRegEntryErr >                     InvalidRegEntryErr;
   typedef ErrorCode< OSStatus, kBadLinkErr >                             BadLinkErr;
   typedef ErrorCode< OSStatus, kBadDeviceErr >                           BadDeviceErr;
   typedef ErrorCode< OSStatus, k16BitCardErr >                           K16BitCardErr;
   typedef ErrorCode< OSStatus, kCardBusCardErr >                         CardBusCardErr;
   typedef ErrorCode< OSStatus, kPassCallToChainErr >                     PassCallToChainErr;
   typedef ErrorCode< OSStatus, kCantConfigureCardErr >                   CantConfigureCardErr;
   typedef ErrorCode< OSStatus, kPostCardEventErr >                       PostCardEventErr;
   typedef ErrorCode< OSStatus, kInvalidDeviceNumber >                    InvalidDeviceNumber;
   typedef ErrorCode< OSStatus, kUnsupportedVsErr >                       UnsupportedVsErr;
   typedef ErrorCode< OSStatus, kInvalidCSClientErr >                     InvalidCSClientErr;
   typedef ErrorCode< OSStatus, kBadTupleDataErr >                        BadTupleDataErr;
   typedef ErrorCode< OSStatus, kBadCustomIFIDErr >                       BadCustomIFIDErr;
   typedef ErrorCode< OSStatus, kNoIOWindowRequestedErr >                 NoIOWindowRequestedErr;
   typedef ErrorCode< OSStatus, kNoMoreTimerClientsErr >                  NoMoreTimerClientsErr;
   typedef ErrorCode< OSStatus, kNoMoreInterruptSlotsErr >                NoMoreInterruptSlotsErr;
   typedef ErrorCode< OSStatus, kNoClientTableErr >                       NoClientTableErr;
   typedef ErrorCode< OSStatus, kUnsupportedCardErr >                     UnsupportedCardErr;
   typedef ErrorCode< OSStatus, kNoCardEnablersFoundErr >                 NoCardEnablersFoundErr;
   typedef ErrorCode< OSStatus, kNoEnablerForCardErr >                    NoEnablerForCardErr;
   typedef ErrorCode< OSStatus, kNoCompatibleNameErr >                    NoCompatibleNameErr;
   typedef ErrorCode< OSStatus, kClientRequestDenied >                    ClientRequestDenied;
   typedef ErrorCode< OSStatus, kNotReadyErr >                            NotReadyErr;
   typedef ErrorCode< OSStatus, kTooManyIOWindowsErr >                    TooManyIOWindowsErr;
   typedef ErrorCode< OSStatus, kAlreadySavedStateErr >                   AlreadySavedStateErr;
   typedef ErrorCode< OSStatus, kAttemptDupCardEntryErr >                 AttemptDupCardEntryErr;
   typedef ErrorCode< OSStatus, kCardPowerOffErr >                        CardPowerOffErr;
   typedef ErrorCode< OSStatus, kNotZVCapableErr >                        NotZVCapableErr;
   typedef ErrorCode< OSStatus, kNoCardBusCISErr >                        NoCardBusCISErr;


   typedef ErrorCode< OSStatus, noDeviceForChannel >                      NoDeviceForChannel;
   typedef ErrorCode< OSStatus, grabTimeComplete >                        GrabTimeComplete;
   typedef ErrorCode< OSStatus, cantDoThatInCurrentMode >                 CantDoThatInCurrentMode;
   typedef ErrorCode< OSStatus, notEnoughMemoryToGrab >                   NotEnoughMemoryToGrab;
   typedef ErrorCode< OSStatus, notEnoughDiskSpaceToGrab >                NotEnoughDiskSpaceToGrab;
   typedef ErrorCode< OSStatus, couldntGetRequiredComponent>              CouldntGetRequiredComponent;
   typedef ErrorCode< OSStatus, badSGChannel >                            BadSGChannel;
   typedef ErrorCode< OSStatus, seqGrabInfoNotAvailable >                 SeqGrabInfoNotAvailable;
   typedef ErrorCode< OSStatus, deviceCantMeetRequest >                   DeviceCantMeetRequest;
   typedef ErrorCode< OSStatus, badControllerHeight >                     BadControllerHeight;
   typedef ErrorCode< OSStatus, editingNotAllowed >                       EditingNotAllowed;
   typedef ErrorCode< OSStatus, controllerBoundsNotExact >                ControllerBoundsNotExact;
   typedef ErrorCode< OSStatus, cannotSetWidthOfAttachedController >      CannotSetWidthOfAttachedController;
   typedef ErrorCode< OSStatus, controllerHasFixedHeight >                ControllerHasFixedHeight;
   typedef ErrorCode< OSStatus, cannotMoveAttachedController >            CannotMoveAttachedController;

/* AERegistry Errors */
   typedef ErrorCode< OSStatus, errAEBadKeyForm >                         ErrAEBadKeyForm;
   typedef ErrorCode< OSStatus, errAECantHandleClass >                    ErrAECantHandleClass;
   typedef ErrorCode< OSStatus, errAECantSupplyType >                     ErrAECantSupplyType;
   typedef ErrorCode< OSStatus, errAECantUndo >                           ErrAECantUndo;
   typedef ErrorCode< OSStatus, errAEEventFailed >                        ErrAEEventFailed;
   typedef ErrorCode< OSStatus, errAEIndexTooLarge >                      ErrAEIndexTooLarge;
   typedef ErrorCode< OSStatus, errAEInTransaction >                      ErrAEInTransaction;
   typedef ErrorCode< OSStatus, errAELocalOnly >                          ErrAELocalOnly;
   typedef ErrorCode< OSStatus, errAENoSuchTransaction >                  ErrAENoSuchTransaction;
   typedef ErrorCode< OSStatus, errAENotAnElement >                       ErrAENotAnElement;
   typedef ErrorCode< OSStatus, errAENotASingleObject >                   ErrAENotASingleObject;
   typedef ErrorCode< OSStatus, errAENotModifiable >                      ErrAENotModifiable;
   typedef ErrorCode< OSStatus, errAENoUserSelection >                    ErrAENoUserSelection;
   typedef ErrorCode< OSStatus, errAEPrivilegeError >                     ErrAEPrivilegeError;
   typedef ErrorCode< OSStatus, errAEReadDenied >                         ErrAEReadDenied;
   typedef ErrorCode< OSStatus, errAETypeError >                          ErrAETypeError;
   typedef ErrorCode< OSStatus, errAEWriteDenied >                        ErrAEWriteDenied;
   typedef ErrorCode< OSStatus, errAENotAnEnumMember >                    ErrAENotAnEnumMember;
   typedef ErrorCode< OSStatus, errAECantPutThatThere >                   ErrAECantPutThatThere;
   typedef ErrorCode< OSStatus, errAEPropertiesClash >                    ErrAEPropertiesClash;

/* TELErr */
   typedef ErrorCode< OSStatus, telGenericError >                         TelGenericError;
   typedef ErrorCode< OSStatus, telNoErr >                                TelNoErr;
   typedef ErrorCode< OSStatus, telNoTools >                              TelNoTools;
   typedef ErrorCode< OSStatus, telBadTermErr >                           TelBadTermErr;
   typedef ErrorCode< OSStatus, telBadDNErr >                             TelBadDNErr;
   typedef ErrorCode< OSStatus, telBadCAErr >                             TelBadCAErr;
   typedef ErrorCode< OSStatus, telBadHandErr >                           TelBadHandErr;
   typedef ErrorCode< OSStatus, telBadProcErr >                           TelBadProcErr;
   typedef ErrorCode< OSStatus, telCAUnavail >                            TelCAUnavail;
   typedef ErrorCode< OSStatus, telNoMemErr >                             TelNoMemErr;
   typedef ErrorCode< OSStatus, telNoOpenErr >                            TelNoOpenErr;
   typedef ErrorCode< OSStatus, telBadHTypeErr >                          TelBadHTypeErr;
   typedef ErrorCode< OSStatus, telHTypeNotSupp >                         TelHTypeNotSupp;
   typedef ErrorCode< OSStatus, telBadLevelErr >                          TelBadLevelErr;
   typedef ErrorCode< OSStatus, telBadVTypeErr >                          TelBadVTypeErr;
   typedef ErrorCode< OSStatus, telVTypeNotSupp >                         TelVTypeNotSupp;
   typedef ErrorCode< OSStatus, telBadAPattErr >                          TelBadAPattErr;
   typedef ErrorCode< OSStatus, telAPattNotSupp >                         TelAPattNotSupp;
   typedef ErrorCode< OSStatus, telBadIndex >                             TelBadIndex;
   typedef ErrorCode< OSStatus, telIndexNotSupp >                         TelIndexNotSupp;
   typedef ErrorCode< OSStatus, telBadStateErr >                          TelBadStateErr;
   typedef ErrorCode< OSStatus, telStateNotSupp >                         TelStateNotSupp;
   typedef ErrorCode< OSStatus, telBadIntExt >                            TelBadIntExt;
   typedef ErrorCode< OSStatus, telIntExtNotSupp >                        TelIntExtNotSupp;
   typedef ErrorCode< OSStatus, telBadDNDType >                           TelBadDNDType;
   typedef ErrorCode< OSStatus, telDNDTypeNotSupp >                       TelDNDTypeNotSupp;
   typedef ErrorCode< OSStatus, telFeatNotSub >                           TelFeatNotSub;
   typedef ErrorCode< OSStatus, telFeatNotAvail >                         TelFeatNotAvail;
   typedef ErrorCode< OSStatus, telFeatActive >                           TelFeatActive;
   typedef ErrorCode< OSStatus, telFeatNotSupp >                          TelFeatNotSupp;
   typedef ErrorCode< OSStatus, telConfLimitErr >                         TelConfLimitErr;
   typedef ErrorCode< OSStatus, telConfNoLimit >                          TelConfNoLimit;
   typedef ErrorCode< OSStatus, telConfErr >                              TelConfErr;
   typedef ErrorCode< OSStatus, telConfRej >                              TelConfRej;
   typedef ErrorCode< OSStatus, telTransferErr >                          TelTransferErr;
   typedef ErrorCode< OSStatus, telTransferRej >                          TelTransferRej;
   typedef ErrorCode< OSStatus, telCBErr >                                TelCBErr;
   typedef ErrorCode< OSStatus, telConfLimitExceeded >                    TelConfLimitExceeded;
   typedef ErrorCode< OSStatus, telBadDNType >                            TelBadDNType;
   typedef ErrorCode< OSStatus, telBadPageID >                            TelBadPageID;
   typedef ErrorCode< OSStatus, telBadIntercomID >                        TelBadIntercomID;
   typedef ErrorCode< OSStatus, telBadFeatureID >                         TelBadFeatureID;
   typedef ErrorCode< OSStatus, telBadFwdType >                           TelBadFwdType;
   typedef ErrorCode< OSStatus, telBadPickupGroupID >                     TelBadPickupGroupID;
   typedef ErrorCode< OSStatus, telBadParkID >                            TelBadParkID;
   typedef ErrorCode< OSStatus, telBadSelect >                            TelBadSelect;
   typedef ErrorCode< OSStatus, telBadBearerType >                        TelBadBearerType;
   typedef ErrorCode< OSStatus, telBadRate >                              TelBadRate;
   typedef ErrorCode< OSStatus, telDNTypeNotSupp >                        TelDNTypeNotSupp;
   typedef ErrorCode< OSStatus, telFwdTypeNotSupp >                       TelFwdTypeNotSupp;
   typedef ErrorCode< OSStatus, telBadDisplayMode >                       TelBadDisplayMode;
   typedef ErrorCode< OSStatus, telDisplayModeNotSupp >                   TelDisplayModeNotSupp;
   typedef ErrorCode< OSStatus, telNoCallbackRef >                        TelNoCallbackRef;
   typedef ErrorCode< OSStatus, telAlreadyOpen >                          TelAlreadyOpen;
   typedef ErrorCode< OSStatus, telStillNeeded >                          TelStillNeeded;
   typedef ErrorCode< OSStatus, telTermNotOpen >                          TelTermNotOpen;
   typedef ErrorCode< OSStatus, telCANotAcceptable >                      TelCANotAcceptable;
   typedef ErrorCode< OSStatus, telCANotRejectable >                      TelCANotRejectable;
   typedef ErrorCode< OSStatus, telCANotDeflectable >                     TelCANotDeflectable;
   typedef ErrorCode< OSStatus, telPBErr >                                TelPBErr;
   typedef ErrorCode< OSStatus, telBadFunction >                          TelBadFunction;
   typedef ErrorCode< OSStatus, telNoSuchTool >                           TelNoSuchTool;
   typedef ErrorCode< OSStatus, telUnknownErr >                           TelUnknownErr;
   typedef ErrorCode< OSStatus, telNoCommFolder >                         TelNoCommFolder;
   typedef ErrorCode< OSStatus, telInitFailed >                           TelInitFailed;
   typedef ErrorCode< OSStatus, telBadCodeResource >                      TelBadCodeResource;
   typedef ErrorCode< OSStatus, telDeviceNotFound >                       TelDeviceNotFound;
   typedef ErrorCode< OSStatus, telBadProcID >                            TelBadProcID;
   typedef ErrorCode< OSStatus, telValidateFailed >                       TelValidateFailed;
   typedef ErrorCode< OSStatus, telAutoAnsNotOn >                         TelAutoAnsNotOn;
   typedef ErrorCode< OSStatus, telDetAlreadyOn >                         TelDetAlreadyOn;
   typedef ErrorCode< OSStatus, telBadSWErr >                             TelBadSWErr;
   typedef ErrorCode< OSStatus, telBadSampleRate >                        TelBadSampleRate;
   typedef ErrorCode< OSStatus, telNotEnoughdspBW >                       TelNotEnoughdspBW;

   typedef ErrorCode< OSStatus, errTaskNotFound >                         ErrTaskNotFound;

/*Power Manager Errors*/
   typedef ErrorCode< OSStatus, pmBusyErr >                               PMBusyErr;
   typedef ErrorCode< OSStatus, pmReplyTOErr >                            PMReplyTOErr;
   typedef ErrorCode< OSStatus, pmSendStartErr >                          PMSendStartErr;
   typedef ErrorCode< OSStatus, pmSendEndErr >                            PMSendEndErr;
   typedef ErrorCode< OSStatus, pmRecvStartErr >                          PMRecvStartErr;
   typedef ErrorCode< OSStatus, pmRecvEndErr >                            PMRecvEndErr;

/*Power Manager 2.0 Errors*/
   typedef ErrorCode< OSStatus, kPowerHandlerExistsForDeviceErr >         PowerHandlerExistsForDeviceErr;
   typedef ErrorCode< OSStatus, kPowerHandlerNotFoundForDeviceErr >       PowerHandlerNotFoundForDeviceErr;
   typedef ErrorCode< OSStatus, kPowerHandlerNotFoundForProcErr >         PowerHandlerNotFoundForProcErr;
   typedef ErrorCode< OSStatus, kPowerMgtMessageNotHandled >              PowerMgtMessageNotHandled;
   typedef ErrorCode< OSStatus, kPowerMgtRequestDenied >                  PowerMgtRequestDenied;
   typedef ErrorCode< OSStatus, kCantReportProcessorTemperatureErr >      CantReportProcessorTemperatureErr;
   typedef ErrorCode< OSStatus, kProcessorTempRoutineRequiresMPLib2 >     ProcessorTempRoutineRequiresMPLib2;
   typedef ErrorCode< OSStatus, kNoSuchPowerSource >                      NoSuchPowerSource;
   typedef ErrorCode< OSStatus, kBridgeSoftwareRunningCantSleep >         BridgeSoftwareRunningCantSleep;


/* Debugging library errors */
   typedef ErrorCode< OSStatus, debuggingExecutionContextErr >            DebuggingExecutionContextErr;
   typedef ErrorCode< OSStatus, debuggingDuplicateSignatureErr >          DebuggingDuplicateSignatureErr;
   typedef ErrorCode< OSStatus, debuggingDuplicateOptionErr>              DebuggingDuplicateOptionErr;
   typedef ErrorCode< OSStatus, debuggingInvalidSignatureErr >            DebuggingInvalidSignatureErr;
   typedef ErrorCode< OSStatus, debuggingInvalidOptionErr >               DebuggingInvalidOptionErr;
   typedef ErrorCode< OSStatus, debuggingInvalidNameErr >                 DebuggingInvalidNameErr;
   typedef ErrorCode< OSStatus, debuggingNoCallbackErr >                  DebuggingNoCallbackErr;
   typedef ErrorCode< OSStatus, debuggingNoMatchErr >                     DebuggingNoMatchErr;


/* HID device driver error codes */
   typedef ErrorCode< OSStatus, kHIDVersionIncompatibleErr >              HIDVersionIncompatibleErr;
   typedef ErrorCode< OSStatus, kHIDDeviceNotReady >                      HIDDeviceNotReady;


/* HID error codes */
   typedef ErrorCode< OSStatus, kHIDSuccess >                             HIDSuccess;
   typedef ErrorCode< OSStatus, kHIDInvalidRangePageErr >                 HIDInvalidRangePageErr;
   typedef ErrorCode< OSStatus, kHIDReportIDZeroErr >                     HIDReportIDZeroErr;
   typedef ErrorCode< OSStatus, kHIDReportCountZeroErr >                  HIDReportCountZeroErr;
   typedef ErrorCode< OSStatus, kHIDReportSizeZeroErr >                   HIDReportSizeZeroErr;
   typedef ErrorCode< OSStatus, kHIDUnmatchedDesignatorRangeErr >         HIDUnmatchedDesignatorRangeErr;
   typedef ErrorCode< OSStatus, kHIDUnmatchedStringRangeErr>              HIDUnmatchedStringRangeErr;
   typedef ErrorCode< OSStatus, kHIDInvertedUsageRangeErr >               HIDInvertedUsageRangeErr;
   typedef ErrorCode< OSStatus, kHIDUnmatchedUsageRangeErr >              HIDUnmatchedUsageRangeErr;
   typedef ErrorCode< OSStatus, kHIDInvertedPhysicalRangeErr >            HIDInvertedPhysicalRangeErr;
   typedef ErrorCode< OSStatus, kHIDInvertedLogicalRangeErr>              HIDInvertedLogicalRangeErr;
   typedef ErrorCode< OSStatus, kHIDBadLogicalMaximumErr >                HIDBadLogicalMaximumErr;
   typedef ErrorCode< OSStatus, kHIDBadLogicalMinimumErr >                HIDBadLogicalMinimumErr;
   typedef ErrorCode< OSStatus, kHIDUsagePageZeroErr >                    HIDUsagePageZeroErr;
   typedef ErrorCode< OSStatus, kHIDEndOfDescriptorErr >                  HIDEndOfDescriptorErr;
   typedef ErrorCode< OSStatus, kHIDNotEnoughMemoryErr >                  HIDNotEnoughMemoryErr;
   typedef ErrorCode< OSStatus, kHIDBadParameterErr >                     HIDBadParameterErr;
   typedef ErrorCode< OSStatus, kHIDNullPointerErr >                      HIDNullPointerErr;
   typedef ErrorCode< OSStatus, kHIDInvalidReportLengthErr >              HIDInvalidReportLengthErr;
   typedef ErrorCode< OSStatus, kHIDInvalidReportTypeErr >                HIDInvalidReportTypeErr;
   typedef ErrorCode< OSStatus, kHIDBadLogPhysValuesErr >                 HIDBadLogPhysValuesErr;
   typedef ErrorCode< OSStatus, kHIDIncompatibleReportErr >               HIDIncompatibleReportErr;
   typedef ErrorCode< OSStatus, kHIDInvalidPreparsedDataErr>              HIDInvalidPreparsedDataErr;
   typedef ErrorCode< OSStatus, kHIDNotValueArrayErr >                    HIDNotValueArrayErr;
   typedef ErrorCode< OSStatus, kHIDUsageNotFoundErr >                    HIDUsageNotFoundErr;
   typedef ErrorCode< OSStatus, kHIDValueOutOfRangeErr >                  HIDValueOutOfRangeErr;
   typedef ErrorCode< OSStatus, kHIDBufferTooSmallErr >                   HIDBufferTooSmallErr;
   typedef ErrorCode< OSStatus, kHIDNullStateErr >                        HIDNullStateErr;
   typedef ErrorCode< OSStatus, kHIDBaseError >                           HIDBaseError;


/* the OT modem module may return the following error codes:*/
   typedef ErrorCode< OSStatus, kModemOutOfMemory >                       ModemOutOfMemory;
   typedef ErrorCode< OSStatus, kModemPreferencesMissing >                ModemPreferencesMissing;
   typedef ErrorCode< OSStatus, kModemScriptMissing >                     ModemScriptMissing;



/* MacTextEditor error codes */
   typedef ErrorCode< OSStatus, kTXNEndIterationErr >                     TXNEndIterationErr;
   typedef ErrorCode< OSStatus, kTXNCannotAddFrameErr >                   TXNCannotAddFrameErr;
   typedef ErrorCode< OSStatus, kTXNInvalidFrameIDErr >                   TXNInvalidFrameIDErr;
   typedef ErrorCode< OSStatus, kTXNIllegalToCrossDataBoundariesErr >     TXNIllegalToCrossDataBoundariesErr;
   typedef ErrorCode< OSStatus, kTXNUserCanceledOperationErr >            TXNUserCanceledOperationErr;
   typedef ErrorCode< OSStatus, kTXNBadDefaultFileTypeWarning >           TXNBadDefaultFileTypeWarning;
   typedef ErrorCode< OSStatus, kTXNCannotSetAutoIndentErr >              TXNCannotSetAutoIndentErr;
   typedef ErrorCode< OSStatus, kTXNRunIndexOutofBoundsErr >              TXNRunIndexOutofBoundsErr;
   typedef ErrorCode< OSStatus, kTXNNoMatchErr >                          TXNNoMatchErr;
   typedef ErrorCode< OSStatus, kTXNAttributeTagInvalidForRunErr >        TXNAttributeTagInvalidForRunErr;
   typedef ErrorCode< OSStatus, kTXNSomeOrAllTagsInvalidForRunErr >       TXNSomeOrAllTagsInvalidForRunErr;
   typedef ErrorCode< OSStatus, kTXNInvalidRunIndex >                     TXNInvalidRunIndex;
   typedef ErrorCode< OSStatus, kTXNAlreadyInitializedErr >               TXNAlreadyInitializedErr;
   typedef ErrorCode< OSStatus, kTXNCannotTurnTSMOffWhenUsingUnicodeErr > TXNCannotTurnTSMOffWhenUsingUnicodeErr;
   typedef ErrorCode< OSStatus, kTXNCopyNotAllowedInEchoModeErr >         TXNCopyNotAllowedInEchoModeErr;
   typedef ErrorCode< OSStatus, kTXNDataTypeNotAllowedErr >               TXNDataTypeNotAllowedErr;
   typedef ErrorCode< OSStatus, kTXNATSUIIsNotInstalledErr >              TXNATSUIIsNotInstalledErr;
   typedef ErrorCode< OSStatus, kTXNOutsideOfLineErr >                    TXNOutsideOfLineErr;
   typedef ErrorCode< OSStatus, kTXNOutsideOfFrameErr >                   TXNOutsideOfFrameErr;




/*Possible errors from the PrinterStatus bottleneck*/
   typedef ErrorCode< OSStatus, printerStatusOpCodeNotSupportedErr >      PrinterStatusOpCodeNotSupportedErr;


/* Keychain Manager error codes */
   typedef ErrorCode< OSStatus, errKCNotAvailable >                       ErrKCNotAvailable;
   typedef ErrorCode< OSStatus, errKCReadOnly >                           ErrKCReadOnly;
   typedef ErrorCode< OSStatus, errKCAuthFailed >                         ErrKCAuthFailed;
   typedef ErrorCode< OSStatus, errKCNoSuchKeychain >                     ErrKCNoSuchKeychain;
   typedef ErrorCode< OSStatus, errKCInvalidKeychain >                    ErrKCInvalidKeychain;
   typedef ErrorCode< OSStatus, errKCDuplicateKeychain >                  ErrKCDuplicateKeychain;
   typedef ErrorCode< OSStatus, errKCDuplicateCallback >                  ErrKCDuplicateCallback;
   typedef ErrorCode< OSStatus, errKCInvalidCallback >                    ErrKCInvalidCallback;
   typedef ErrorCode< OSStatus, errKCDuplicateItem >                      ErrKCDuplicateItem;
   typedef ErrorCode< OSStatus, errKCItemNotFound >                       ErrKCItemNotFound;
   typedef ErrorCode< OSStatus, errKCBufferTooSmall >                     ErrKCBufferTooSmall;
   typedef ErrorCode< OSStatus, errKCDataTooLarge >                       ErrKCDataTooLarge;
   typedef ErrorCode< OSStatus, errKCNoSuchAttr >                         ErrKCNoSuchAttr;
   typedef ErrorCode< OSStatus, errKCInvalidItemRef >                     ErrKCInvalidItemRef;
   typedef ErrorCode< OSStatus, errKCInvalidSearchRef >                   ErrKCInvalidSearchRef;
   typedef ErrorCode< OSStatus, errKCNoSuchClass >                        ErrKCNoSuchClass;
   typedef ErrorCode< OSStatus, errKCNoDefaultKeychain >                  ErrKCNoDefaultKeychain;
   typedef ErrorCode< OSStatus, errKCInteractionNotAllowed >              ErrKCInteractionNotAllowed;
   typedef ErrorCode< OSStatus, errKCReadOnlyAttr >                       ErrKCReadOnlyAttr;
   typedef ErrorCode< OSStatus, errKCWrongKCVersion >                     ErrKCWrongKCVersion;
   typedef ErrorCode< OSStatus, errKCKeySizeNotAllowed >                  ErrKCKeySizeNotAllowed;
   typedef ErrorCode< OSStatus, errKCNoStorageModule >                    ErrKCNoStorageModule;
   typedef ErrorCode< OSStatus, errKCNoCertificateModule >                ErrKCNoCertificateModule;
   typedef ErrorCode< OSStatus, errKCNoPolicyModule >                     ErrKCNoPolicyModule;
   typedef ErrorCode< OSStatus, errKCInteractionRequired >                ErrKCInteractionRequired;
   typedef ErrorCode< OSStatus, errKCDataNotAvailable >                   ErrKCDataNotAvailable;
   typedef ErrorCode< OSStatus, errKCDataNotModifiable >                  ErrKCDataNotModifiable;
   typedef ErrorCode< OSStatus, errKCCreateChainFailed >                  ErrKCCreateChainFailed;


/* UnicodeUtilities error & status codes*/
   typedef ErrorCode< OSStatus, kUCOutputBufferTooSmall >                 UCOutputBufferTooSmall;
   typedef ErrorCode< OSStatus, kUCTextBreakLocatorMissingType >          UCTextBreakLocatorMissingType;

/* Multiprocessing API error codes*/
   typedef ErrorCode< OSStatus, kMPIterationEndErr >                      MPIterationEndErr;
   typedef ErrorCode< OSStatus, kMPPrivilegedErr >                        MPPrivilegedErr;
   typedef ErrorCode< OSStatus, kMPProcessCreatedErr >                    MPProcessCreatedErr;
   typedef ErrorCode< OSStatus, kMPProcessTerminatedErr >                 MPProcessTerminatedErr;
   typedef ErrorCode< OSStatus, kMPTaskCreatedErr >                       MPTaskCreatedErr;
   typedef ErrorCode< OSStatus, kMPTaskBlockedErr >                       MPTaskBlockedErr;
   typedef ErrorCode< OSStatus, kMPTaskStoppedErr >                       MPTaskStoppedErr;
   typedef ErrorCode< OSStatus, kMPBlueBlockingErr >                      MPBlueBlockingErr;
   typedef ErrorCode< OSStatus, kMPDeletedErr >                           MPDeletedErr;
   typedef ErrorCode< OSStatus, kMPTimeoutErr >                           MPTimeoutErr;
   typedef ErrorCode< OSStatus, kMPTaskAbortedErr >                       MPTaskAbortedErr;
   typedef ErrorCode< OSStatus, kMPInsufficientResourcesErr>              MPInsufficientResourcesErr;
   typedef ErrorCode< OSStatus, kMPInvalidIDErr >                         MPInvalidIDErr;

/* StringCompare error codes (in TextUtils range)*/
   typedef ErrorCode< OSStatus, kCollateAttributesNotFoundErr >           CollateAttributesNotFoundErr;
   typedef ErrorCode< OSStatus, kCollateInvalidOptions >                  CollateInvalidOptions;
   typedef ErrorCode< OSStatus, kCollateMissingUnicodeTableErr >          CollateMissingUnicodeTableErr;
   typedef ErrorCode< OSStatus, kCollateUnicodeConvertFailedErr >         CollateUnicodeConvertFailedErr;
   typedef ErrorCode< OSStatus, kCollatePatternNotFoundErr >              CollatePatternNotFoundErr;
   typedef ErrorCode< OSStatus, kCollateInvalidChar >                     CollateInvalidChar;
   typedef ErrorCode< OSStatus, kCollateBufferTooSmall >                  CollateBufferTooSmall;
   typedef ErrorCode< OSStatus, kCollateInvalidCollationRef >             CollateInvalidCollationRef;


/* FontSync OSStatus Codes */
   typedef ErrorCode< OSStatus, kFNSInvalidReferenceErr >                 FNSInvalidReferenceErr;
   typedef ErrorCode< OSStatus, kFNSBadReferenceVersionErr >              FNSBadReferenceVersionErr;
   typedef ErrorCode< OSStatus, kFNSInvalidProfileErr >                   FNSInvalidProfileErr;
   typedef ErrorCode< OSStatus, kFNSBadProfileVersionErr >                FNSBadProfileVersionErr;
   typedef ErrorCode< OSStatus, kFNSDuplicateReferenceErr >               FNSDuplicateReferenceErr;
   typedef ErrorCode< OSStatus, kFNSMismatchErr >                         FNSMismatchErr;
   typedef ErrorCode< OSStatus, kFNSInsufficientDataErr >                 FNSInsufficientDataErr;
   typedef ErrorCode< OSStatus, kFNSBadFlattenedSizeErr >                 FNSBadFlattenedSizeErr;
   typedef ErrorCode< OSStatus, kFNSNameNotFoundErr >                     FNSNameNotFoundErr;



/* MacLocales error codes*/
   typedef ErrorCode< OSStatus, kLocalesBufferTooSmallErr >               LocalesBufferTooSmallErr;
   typedef ErrorCode< OSStatus, kLocalesTableFormatErr >                  LocalesTableFormatErr;
   typedef ErrorCode< OSStatus, kLocalesDefaultDisplayStatus >            LocalesDefaultDisplayStatus;


/* Settings Manager (formerly known as Location Manager) Errors */
   typedef ErrorCode< OSStatus, kALMInternalErr >                         ALMInternalErr;
   typedef ErrorCode< OSStatus, kALMGroupNotFoundErr >                    ALMGroupNotFoundErr;
   typedef ErrorCode< OSStatus, kALMNoSuchModuleErr >                     ALMNoSuchModuleErr;
   typedef ErrorCode< OSStatus, kALMModuleCommunicationErr >              ALMModuleCommunicationErr;
   typedef ErrorCode< OSStatus, kALMDuplicateModuleErr >                  ALMDuplicateModuleErr;
   typedef ErrorCode< OSStatus, kALMInstallationErr >                     ALMInstallationErr;
   typedef ErrorCode< OSStatus, kALMDeferSwitchErr >                      ALMDeferSwitchErr;
   typedef ErrorCode< OSStatus, kALMRebootFlagsLevelErr >                 ALMRebootFlagsLevelErr;

   typedef ErrorCode< OSStatus, kALMLocationNotFoundErr >                 ALMLocationNotFoundErr;


/* SoundSprocket Error Codes */
   typedef ErrorCode< OSStatus, kSSpInternalErr >                         SSpInternalErr;
   typedef ErrorCode< OSStatus, kSSpVersionErr >                          SSpVersionErr;
   typedef ErrorCode< OSStatus, kSSpCantInstallErr >                      SSpCantInstallErr;
   typedef ErrorCode< OSStatus, kSSpParallelUpVectorErr >                 SSpParallelUpVectorErr;
   typedef ErrorCode< OSStatus, kSSpScaleToZeroErr >                      SSpScaleToZeroErr;


/* NetSprocket Error Codes */
   typedef ErrorCode< OSStatus, kNSpInitializationFailedErr>              NSpInitializationFailedErr;
   typedef ErrorCode< OSStatus, kNSpAlreadyInitializedErr >               NSpAlreadyInitializedErr;
   typedef ErrorCode< OSStatus, kNSpTopologyNotSupportedErr>              NSpTopologyNotSupportedErr;
   typedef ErrorCode< OSStatus, kNSpPipeFullErr >                         NSpPipeFullErr;
   typedef ErrorCode< OSStatus, kNSpHostFailedErr >                       NSpHostFailedErr;
   typedef ErrorCode< OSStatus, kNSpProtocolNotAvailableErr>              NSpProtocolNotAvailableErr;
   typedef ErrorCode< OSStatus, kNSpInvalidGameRefErr >                   NSpInvalidGameRefErr;
   typedef ErrorCode< OSStatus, kNSpInvalidParameterErr >                 NSpInvalidParameterErr;
   typedef ErrorCode< OSStatus, kNSpOTNotPresentErr >                     NSpOTNotPresentErr;
   typedef ErrorCode< OSStatus, kNSpOTVersionTooOldErr >                  NSpOTVersionTooOldErr;
   typedef ErrorCode< OSStatus, kNSpMemAllocationErr >                    NSpMemAllocationErr;
   typedef ErrorCode< OSStatus, kNSpAlreadyAdvertisingErr >               NSpAlreadyAdvertisingErr;
   typedef ErrorCode< OSStatus, kNSpNotAdvertisingErr >                   NSpNotAdvertisingErr;
   typedef ErrorCode< OSStatus, kNSpInvalidAddressErr >                   NSpInvalidAddressErr;
   typedef ErrorCode< OSStatus, kNSpFreeQExhaustedErr >                   NSpFreeQExhaustedErr;
   typedef ErrorCode< OSStatus, kNSpRemovePlayerFailedErr >               NSpRemovePlayerFailedErr;
   typedef ErrorCode< OSStatus, kNSpAddressInUseErr >                     NSpAddressInUseErr;
   typedef ErrorCode< OSStatus, kNSpFeatureNotImplementedErr >            NSpFeatureNotImplementedErr;
   typedef ErrorCode< OSStatus, kNSpNameRequiredErr >                     NSpNameRequiredErr;
   typedef ErrorCode< OSStatus, kNSpInvalidPlayerIDErr >                  NSpInvalidPlayerIDErr;
   typedef ErrorCode< OSStatus, kNSpInvalidGroupIDErr >                   NSpInvalidGroupIDErr;
   typedef ErrorCode< OSStatus, kNSpNoPlayersErr >                        NSpNoPlayersErr;
   typedef ErrorCode< OSStatus, kNSpNoGroupsErr >                         NSpNoGroupsErr;
   typedef ErrorCode< OSStatus, kNSpNoHostVolunteersErr >                 NSpNoHostVolunteersErr;
   typedef ErrorCode< OSStatus, kNSpCreateGroupFailedErr >                NSpCreateGroupFailedErr;
   typedef ErrorCode< OSStatus, kNSpAddPlayerFailedErr >                  NSpAddPlayerFailedErr;
   typedef ErrorCode< OSStatus, kNSpInvalidDefinitionErr >                NSpInvalidDefinitionErr;
   typedef ErrorCode< OSStatus, kNSpInvalidProtocolRefErr >               NSpInvalidProtocolRefErr;
   typedef ErrorCode< OSStatus, kNSpInvalidProtocolListErr >              NSpInvalidProtocolListErr;
   typedef ErrorCode< OSStatus, kNSpTimeoutErr >                          NSpTimeoutErr;
   typedef ErrorCode< OSStatus, kNSpGameTerminatedErr >                   NSpGameTerminatedErr;
   typedef ErrorCode< OSStatus, kNSpConnectFailedErr >                    NSpConnectFailedErr;
   typedef ErrorCode< OSStatus, kNSpSendFailedErr >                       NSpSendFailedErr;
   typedef ErrorCode< OSStatus, kNSpMessageTooBigErr >                    NSpMessageTooBigErr;
   typedef ErrorCode< OSStatus, kNSpCantBlockErr >                        NSpCantBlockErr;
   typedef ErrorCode< OSStatus, kNSpJoinFailedErr >                       NSpJoinFailedErr;


/* InputSprockets error codes */
   typedef ErrorCode< OSStatus, kISpInternalErr >                         ISpInternalErr;
   typedef ErrorCode< OSStatus, kISpSystemListErr >                       ISpSystemListErr;
   typedef ErrorCode< OSStatus, kISpBufferToSmallErr >                    ISpBufferToSmallErr;
   typedef ErrorCode< OSStatus, kISpElementInListErr >                    ISpElementInListErr;
   typedef ErrorCode< OSStatus, kISpElementNotInListErr >                 ISpElementNotInListErr;
   typedef ErrorCode< OSStatus, kISpSystemInactiveErr >                   ISpSystemInactiveErr;
   typedef ErrorCode< OSStatus, kISpDeviceInactiveErr >                   ISpDeviceInactiveErr;
   typedef ErrorCode< OSStatus, kISpSystemActiveErr >                     ISpSystemActiveErr;
   typedef ErrorCode< OSStatus, kISpDeviceActiveErr >                     ISpDeviceActiveErr;
   typedef ErrorCode< OSStatus, kISpListBusyErr >                         ISpListBusyErr;

/* DrawSprockets error/warning codes */
   typedef ErrorCode< OSStatus, kDSpNotInitializedErr >                   DSpNotInitializedErr;
   typedef ErrorCode< OSStatus, kDSpSystemSWTooOldErr >                   DSpSystemSWTooOldErr;
   typedef ErrorCode< OSStatus, kDSpInvalidContextErr >                   DSpInvalidContextErr;
   typedef ErrorCode< OSStatus, kDSpInvalidAttributesErr >                DSpInvalidAttributesErr;
   typedef ErrorCode< OSStatus, kDSpContextAlreadyReservedErr >           DSpContextAlreadyReservedErr;
   typedef ErrorCode< OSStatus, kDSpContextNotReservedErr >               DSpContextNotReservedErr;
   typedef ErrorCode< OSStatus, kDSpContextNotFoundErr >                  DSpContextNotFoundErr;
   typedef ErrorCode< OSStatus, kDSpFrameRateNotReadyErr >                DSpFrameRateNotReadyErr;
   typedef ErrorCode< OSStatus, kDSpConfirmSwitchWarning >                DSpConfirmSwitchWarning;
   typedef ErrorCode< OSStatus, kDSpInternalErr >                         DSpInternalErr;
   typedef ErrorCode< OSStatus, kDSpStereoContextErr >                    DSpStereoContextErr;


/* Find By Content errors */
   typedef ErrorCode< OSStatus, kFBCvTwinExceptionErr >                   FBCvTwinExceptionErr;
   typedef ErrorCode< OSStatus, kFBCnoIndexesFound >                      FBCnoIndexesFound;
   typedef ErrorCode< OSStatus, kFBCallocFailed >                         FBCallocFailed;
   typedef ErrorCode< OSStatus, kFBCbadParam >                            FBCbadParam;
   typedef ErrorCode< OSStatus, kFBCfileNotIndexed >                      FBCfileNotIndexed;
   typedef ErrorCode< OSStatus, kFBCbadIndexFile >                        FBCbadIndexFile;
   typedef ErrorCode< OSStatus, kFBCcompactionFailed >                    FBCcompactionFailed;
   typedef ErrorCode< OSStatus, kFBCvalidationFailed >                    FBCvalidationFailed;
   typedef ErrorCode< OSStatus, kFBCindexingFailed >                      FBCindexingFailed;
   typedef ErrorCode< OSStatus, kFBCcommitFailed >                        FBCcommitFailed;
   typedef ErrorCode< OSStatus, kFBCdeletionFailed >                      FBCdeletionFailed;
   typedef ErrorCode< OSStatus, kFBCmoveFailed >                          FBCmoveFailed;
   typedef ErrorCode< OSStatus, kFBCtokenizationFailed >                  FBCtokenizationFailed;
   typedef ErrorCode< OSStatus, kFBCmergingFailed >                       FBCmergingFailed;
   typedef ErrorCode< OSStatus, kFBCindexCreationFailed >                 FBCindexCreationFailed;
   typedef ErrorCode< OSStatus, kFBCaccessorStoreFailed >                 FBCaccessorStoreFailed;
   typedef ErrorCode< OSStatus, kFBCaddDocFailed >                        FBCaddDocFailed;
   typedef ErrorCode< OSStatus, kFBCflushFailed >                         FBCflushFailed;
   typedef ErrorCode< OSStatus, kFBCindexNotFound >                       FBCindexNotFound;
   typedef ErrorCode< OSStatus, kFBCnoSearchSession >                     FBCnoSearchSession;
   typedef ErrorCode< OSStatus, kFBCindexingCanceled >                    FBCindexingCanceled;
   typedef ErrorCode< OSStatus, kFBCaccessCanceled >                      FBCaccessCanceled;
   typedef ErrorCode< OSStatus, kFBCindexFileDestroyed >                  FBCindexFileDestroyed;
   typedef ErrorCode< OSStatus, kFBCindexNotAvailable >                   FBCindexNotAvailable;
   typedef ErrorCode< OSStatus, kFBCsearchFailed >                        FBCsearchFailed;
   typedef ErrorCode< OSStatus, kFBCsomeFilesNotIndexed >                 FBCsomeFilesNotIndexed;
   typedef ErrorCode< OSStatus, kFBCillegalSessionChange >                FBCillegalSessionChange;
   typedef ErrorCode< OSStatus, kFBCanalysisNotAvailable >                FBCanalysisNotAvailable;
   typedef ErrorCode< OSStatus, kFBCbadIndexFileVersion >                 FBCbadIndexFileVersion;
   typedef ErrorCode< OSStatus, kFBCsummarizationCanceled >               FBCsummarizationCanceled;
   typedef ErrorCode< OSStatus, kFBCindexDiskIOFailed >                   FBCindexDiskIOFailed;
   typedef ErrorCode< OSStatus, kFBCbadSearchSession >                    FBCbadSearchSession;
   typedef ErrorCode< OSStatus, kFBCnoSuchHit >                           FBCnoSuchHit;


/* QuickTime VR Errors */
   typedef ErrorCode< OSStatus, notAQTVRMovieErr >                        NotAQTVRMovieErr;
   typedef ErrorCode< OSStatus, constraintReachedErr >                    ConstraintReachedErr;
   typedef ErrorCode< OSStatus, callNotSupportedByNodeErr >               CallNotSupportedByNodeErr;
   typedef ErrorCode< OSStatus, selectorNotSupportedByNodeErr >           SelectorNotSupportedByNodeErr;
   typedef ErrorCode< OSStatus, invalidNodeIDErr >                        InvalidNodeIDErr;
   typedef ErrorCode< OSStatus, invalidViewStateErr >                     InvalidViewStateErr;
   typedef ErrorCode< OSStatus, timeNotInViewErr >                        TimeNotInViewErr;
   typedef ErrorCode< OSStatus, propertyNotSupportedByNodeErr >           PropertyNotSupportedByNodeErr;
   typedef ErrorCode< OSStatus, settingNotSupportedByNodeErr >            SettingNotSupportedByNodeErr;
   typedef ErrorCode< OSStatus, limitReachedErr >                         LimitReachedErr;
   typedef ErrorCode< OSStatus, invalidNodeFormatErr >                    InvalidNodeFormatErr;
   typedef ErrorCode< OSStatus, invalidHotSpotIDErr >                     InvalidHotSpotIDErr;
   typedef ErrorCode< OSStatus, noMemoryNodeFailedInitialize >            NoMemoryNodeFailedInitialize;
   typedef ErrorCode< OSStatus, streamingNodeNotReadyErr >                StreamingNodeNotReadyErr;
   typedef ErrorCode< OSStatus, qtvrLibraryLoadErr >                      QTVRLibraryLoadErr;
   typedef ErrorCode< OSStatus, qtvrUninitialized >                       QTVRUninitialized;


/* Appearance Manager Error Codes */
   typedef ErrorCode< OSStatus, themeInvalidBrushErr >                    ThemeInvalidBrushErr;
   typedef ErrorCode< OSStatus, themeProcessRegisteredErr >               ThemeProcessRegisteredErr;
   typedef ErrorCode< OSStatus, themeProcessNotRegisteredErr >            ThemeProcessNotRegisteredErr;
   typedef ErrorCode< OSStatus, themeBadTextColorErr >                    ThemeBadTextColorErr;
   typedef ErrorCode< OSStatus, themeHasNoAccentsErr >                    ThemeHasNoAccentsErr;
   typedef ErrorCode< OSStatus, themeBadCursorIndexErr >                  ThemeBadCursorIndexErr;
   typedef ErrorCode< OSStatus, themeScriptFontNotFoundErr >              ThemeScriptFontNotFoundErr;
   typedef ErrorCode< OSStatus, themeMonitorDepthNotSupportedErr >        ThemeMonitorDepthNotSupportedErr;

/* Control Manager Error Codes */
   typedef ErrorCode< OSStatus, errMessageNotSupported >                  ErrMessageNotSupported;
   typedef ErrorCode< OSStatus, errDataNotSupported >                     ErrDataNotSupported;
   typedef ErrorCode< OSStatus, errControlDoesntSupportFocus >            ErrControlDoesntSupportFocus;
   typedef ErrorCode< OSStatus, errUnknownControl >                       ErrUnknownControl;
   typedef ErrorCode< OSStatus, errCouldntSetFocus >                      ErrCouldntSetFocus;
   typedef ErrorCode< OSStatus, errNoRootControl >                        ErrNoRootControl;
   typedef ErrorCode< OSStatus, errRootAlreadyExists >                    ErrRootAlreadyExists;
   typedef ErrorCode< OSStatus, errInvalidPartCode >                      ErrInvalidPartCode;
   typedef ErrorCode< OSStatus, errControlsAlreadyExist >                 ErrControlsAlreadyExist;
   typedef ErrorCode< OSStatus, errControlIsNotEmbedder >                 ErrControlIsNotEmbedder;
   typedef ErrorCode< OSStatus, errDataSizeMismatch >                     ErrDataSizeMismatch;
   typedef ErrorCode< OSStatus, errControlHiddenOrDisabled >              ErrControlHiddenOrDisabled;
   typedef ErrorCode< OSStatus, errCantEmbedIntoSelf >                    ErrCantEmbedIntoSelf;
   typedef ErrorCode< OSStatus, errCantEmbedRoot >                        ErrCantEmbedRoot;
   typedef ErrorCode< OSStatus, errItemNotControl >                       ErrItemNotControl;
   typedef ErrorCode< OSStatus, controlInvalidDataVersionErr >            ControlInvalidDataVersionErr;
   typedef ErrorCode< OSStatus, controlPropertyInvalid >                  ControlPropertyInvalid;
   typedef ErrorCode< OSStatus, controlPropertyNotFoundErr >              ControlPropertyNotFoundErr;
   typedef ErrorCode< OSStatus, controlHandleInvalidErr >                 ControlHandleInvalidErr;

/* URLAccess Error Codes */
   typedef ErrorCode< OSStatus, kURLInvalidURLReferenceError >            URLInvalidURLReferenceError;
   typedef ErrorCode< OSStatus, kURLProgressAlreadyDisplayedError >       URLProgressAlreadyDisplayedError;
   typedef ErrorCode< OSStatus, kURLDestinationExistsError >              URLDestinationExistsError;
   typedef ErrorCode< OSStatus, kURLInvalidURLError >                     URLInvalidURLError;
   typedef ErrorCode< OSStatus, kURLUnsupportedSchemeError >              URLUnsupportedSchemeError;
   typedef ErrorCode< OSStatus, kURLServerBusyError >                     URLServerBusyError;
   typedef ErrorCode< OSStatus, kURLAuthenticationError >                 URLAuthenticationError;
   typedef ErrorCode< OSStatus, kURLPropertyNotYetKnownError >            URLPropertyNotYetKnownError;
   typedef ErrorCode< OSStatus, kURLUnknownPropertyError >                URLUnknownPropertyError;
   typedef ErrorCode< OSStatus, kURLPropertyBufferTooSmallError >         URLPropertyBufferTooSmallError;
   typedef ErrorCode< OSStatus, kURLUnsettablePropertyError>              URLUnsettablePropertyError;
   typedef ErrorCode< OSStatus, kURLInvalidCallError >                    URLInvalidCallError;
   typedef ErrorCode< OSStatus, kURLFileEmptyError >                      URLFileEmptyError;
   typedef ErrorCode< OSStatus, kURLExtensionFailureError >               URLExtensionFailureError;
   typedef ErrorCode< OSStatus, kURLInvalidConfigurationError >           URLInvalidConfigurationError;
   typedef ErrorCode< OSStatus, kURLAccessNotAvailableError>              URLAccessNotAvailableError;
   typedef ErrorCode< OSStatus, kURL68kNotSupportedError >                URL68kNotSupportedError;

/* ComponentError codes*/
   typedef ErrorCode< OSStatus, badComponentInstance >                    BadComponentInstance;
   typedef ErrorCode< OSStatus, badComponentSelector >                    BadComponentSelector;
  }

#endif
