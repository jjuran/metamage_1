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
   typedef Nucleus::ErrorCode< OSStatus, paramErr >                                ParamErr;
   typedef Nucleus::ErrorCode< OSStatus, noHardwareErr >                           NoHardwareErr;
   typedef Nucleus::ErrorCode< OSStatus, notEnoughHardwareErr >                    NotEnoughHardwareErr;
   typedef Nucleus::ErrorCode< OSStatus, userCanceledErr >                         UserCanceledErr;
   typedef Nucleus::ErrorCode< OSStatus, qErr >                                    QErr;
   typedef Nucleus::ErrorCode< OSStatus, vTypErr >                                 VTypErr;
   typedef Nucleus::ErrorCode< OSStatus, corErr >                                  CorErr;
   typedef Nucleus::ErrorCode< OSStatus, unimpErr >                                UnimpErr;
   typedef Nucleus::ErrorCode< OSStatus, SlpTypeErr >                              SlpTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, seNoDB >                                  SeNoDB;
   typedef Nucleus::ErrorCode< OSStatus, controlErr >                              ControlErr;
   typedef Nucleus::ErrorCode< OSStatus, statusErr >                               StatusErr;
   typedef Nucleus::ErrorCode< OSStatus, readErr >                                 ReadErr;
   typedef Nucleus::ErrorCode< OSStatus, writErr >                                 WritErr;
   typedef Nucleus::ErrorCode< OSStatus, badUnitErr >                              BadUnitErr;
   typedef Nucleus::ErrorCode< OSStatus, unitEmptyErr >                            UnitEmptyErr;
   typedef Nucleus::ErrorCode< OSStatus, openErr >                                 OpenErr;
   typedef Nucleus::ErrorCode< OSStatus, closErr >                                 ClosErr;
   typedef Nucleus::ErrorCode< OSStatus, dRemovErr >                               DRemovErr;
   typedef Nucleus::ErrorCode< OSStatus, dInstErr >                                DInstErr;

   typedef Nucleus::ErrorCode< OSStatus, abortErr >                                AbortErr;
   typedef Nucleus::ErrorCode< OSStatus, iIOAbortErr >                             IIOAbortErr;
   typedef Nucleus::ErrorCode< OSStatus, notOpenErr >                              NotOpenErr;
   typedef Nucleus::ErrorCode< OSStatus, unitTblFullErr >                          UnitTblFullErr;
   typedef Nucleus::ErrorCode< OSStatus, dceExtErr >                               DCEExtErr;
   typedef Nucleus::ErrorCode< OSStatus, slotNumErr >                              SlotNumErr;
   typedef Nucleus::ErrorCode< OSStatus, gcrOnMFMErr >                             GcrOnMFMErr;
   typedef Nucleus::ErrorCode< OSStatus, dirFulErr >                               DirFulErr;
   typedef Nucleus::ErrorCode< OSStatus, dskFulErr >                               DskFulErr;
   typedef Nucleus::ErrorCode< OSStatus, nsvErr >                                  NSVErr;
   typedef Nucleus::ErrorCode< OSStatus, ioErr >                                   IOErr;
   typedef Nucleus::ErrorCode< OSStatus, bdNamErr >                                BdNamErr;
   typedef Nucleus::ErrorCode< OSStatus, fnOpnErr >                                FNOpnErr;
   typedef Nucleus::ErrorCode< OSStatus, eofErr >                                  EOFErr;
   typedef Nucleus::ErrorCode< OSStatus, posErr >                                  PosErr;
   typedef Nucleus::ErrorCode< OSStatus, mFulErr >                                 MFulErr;
   typedef Nucleus::ErrorCode< OSStatus, tmfoErr >                                 TMFOErr;
   typedef Nucleus::ErrorCode< OSStatus, fnfErr >                                  FNFErr;
   typedef Nucleus::ErrorCode< OSStatus, wPrErr >                                  WPrErr;
   typedef Nucleus::ErrorCode< OSStatus, fLckdErr >                                FLckdErr;

   typedef Nucleus::ErrorCode< OSStatus, vLckdErr >                                VLckdErr;
   typedef Nucleus::ErrorCode< OSStatus, fBsyErr >                                 FBsyErr;
   typedef Nucleus::ErrorCode< OSStatus, dupFNErr >                                DupFNErr;
   typedef Nucleus::ErrorCode< OSStatus, opWrErr >                                 OpWrErr;
   typedef Nucleus::ErrorCode< OSStatus, rfNumErr >                                RfNumErr;
   typedef Nucleus::ErrorCode< OSStatus, gfpErr >                                  GFPErr;
   typedef Nucleus::ErrorCode< OSStatus, volOffLinErr >                            VolOffLinErr;
   typedef Nucleus::ErrorCode< OSStatus, permErr >                                 PermErr;
   typedef Nucleus::ErrorCode< OSStatus, volOnLinErr >                             VolOnLinErr;
   typedef Nucleus::ErrorCode< OSStatus, nsDrvErr >                                NSDrvErr;
   typedef Nucleus::ErrorCode< OSStatus, noMacDskErr >                             NoMacDskErr;
   typedef Nucleus::ErrorCode< OSStatus, extFSErr >                                ExtFSErr;
   typedef Nucleus::ErrorCode< OSStatus, fsRnErr >                                 FSRnErr;
   typedef Nucleus::ErrorCode< OSStatus, badMDBErr >                               BadMDBErr;
   typedef Nucleus::ErrorCode< OSStatus, wrPermErr >                               WrPermErr;
   typedef Nucleus::ErrorCode< OSStatus, dirNFErr >                                DirNFErr;
   typedef Nucleus::ErrorCode< OSStatus, tmwdoErr >                                TMWDOErr;
   typedef Nucleus::ErrorCode< OSStatus, badMovErr >                               BadMovErr;
   typedef Nucleus::ErrorCode< OSStatus, wrgVolTypErr >                            WrgVolTypErr;
   typedef Nucleus::ErrorCode< OSStatus, volGoneErr >                              VolGoneErr;

   typedef Nucleus::ErrorCode< OSStatus, fidNotFound >                             FIDNotFound;
   typedef Nucleus::ErrorCode< OSStatus, fidExists >                               FIDExists;
   typedef Nucleus::ErrorCode< OSStatus, notAFileErr >                             NotAFileErr;
   typedef Nucleus::ErrorCode< OSStatus, diffVolErr >                              DiffVolErr;
   typedef Nucleus::ErrorCode< OSStatus, catChangedErr >                           CatChangedErr;
   typedef Nucleus::ErrorCode< OSStatus, desktopDamagedErr >                       DesktopDamagedErr;
   typedef Nucleus::ErrorCode< OSStatus, sameFileErr >                             SameFileErr;
   typedef Nucleus::ErrorCode< OSStatus, badFidErr >                               BadFidErr;
   typedef Nucleus::ErrorCode< OSStatus, notARemountErr >                          NotARemountErr;
   typedef Nucleus::ErrorCode< OSStatus, fileBoundsErr >                           FileBoundsErr;
   typedef Nucleus::ErrorCode< OSStatus, fsDataTooBigErr >                         FSDataTooBigErr;
   typedef Nucleus::ErrorCode< OSStatus, volVMBusyErr >                            VolVMBusyErr;
   typedef Nucleus::ErrorCode< OSStatus, badFCBErr >                               BadFCBErr;
   typedef Nucleus::ErrorCode< OSStatus, errFSUnknownCall >                        ErrFSUnknownCall;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadFSRef >                           ErrFSBadFSRef;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadForkName >                        ErrFSBadForkName;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadBuffer >                          ErrFSBadBuffer;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadForkRef >                         ErrFSBadForkRef;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadInfoBitmap >                      ErrFSBadInfoBitmap;
   typedef Nucleus::ErrorCode< OSStatus, errFSMissingCatInfo >                     ErrFSMissingCatInfo;
   typedef Nucleus::ErrorCode< OSStatus, errFSNotAFolder >                         ErrFSNotAFolder;
   typedef Nucleus::ErrorCode< OSStatus, errFSForkNotFound >                       ErrFSForkNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errFSNameTooLong >                        ErrFSNameTooLong;
   typedef Nucleus::ErrorCode< OSStatus, errFSMissingName >                        ErrFSMissingName;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadPosMode >                         ErrFSBadPosMode;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadAllocFlags >                      ErrFSBadAllocFlags;
   typedef Nucleus::ErrorCode< OSStatus, errFSNoMoreItems >                        ErrFSNoMoreItems;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadItemCount >                       ErrFSBadItemCount;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadSearchParams >                    ErrFSBadSearchParams;
   typedef Nucleus::ErrorCode< OSStatus, errFSRefsDifferent >                      ErrFSRefsDifferent;
   typedef Nucleus::ErrorCode< OSStatus, errFSForkExists >                         ErrFSForkExists;
   typedef Nucleus::ErrorCode< OSStatus, errFSBadIteratorFlags >                   ErrFSBadIteratorFlags;
   typedef Nucleus::ErrorCode< OSStatus, errFSIteratorNotFound >                   ErrFSIteratorNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errFSIteratorNotSupported >               ErrFSIteratorNotSupported;
   typedef Nucleus::ErrorCode< OSStatus, envNotPresent >                           EnvNotPresent;
   typedef Nucleus::ErrorCode< OSStatus, envBadVers >                              EnvBadVers;
   typedef Nucleus::ErrorCode< OSStatus, envVersTooBig >                           EnvVersTooBig;
   typedef Nucleus::ErrorCode< OSStatus, fontDecError >                            FontDecError;
   typedef Nucleus::ErrorCode< OSStatus, fontNotDeclared >                         FontNotDeclared;
   typedef Nucleus::ErrorCode< OSStatus, fontSubErr >                              FontSubErr;
   typedef Nucleus::ErrorCode< OSStatus, fontNotOutlineErr >                       FontNotOutlineErr;
   typedef Nucleus::ErrorCode< OSStatus, firstDskErr >                             FirstDskErr;
   typedef Nucleus::ErrorCode< OSStatus, lastDskErr >                              LastDskErr;
   typedef Nucleus::ErrorCode< OSStatus, noDriveErr >                              NoDriveErr;
   typedef Nucleus::ErrorCode< OSStatus, offLinErr >                               OffLinErr;
   typedef Nucleus::ErrorCode< OSStatus, noNybErr >                                NoNybErr;

   typedef Nucleus::ErrorCode< OSStatus, noAdrMkErr >                              NoAdrMkErr;
   typedef Nucleus::ErrorCode< OSStatus, dataVerErr >                              DataVerErr;
   typedef Nucleus::ErrorCode< OSStatus, badCksmErr >                              BadCksmErr;
   typedef Nucleus::ErrorCode< OSStatus, badBtSlpErr >                             BadBtSlpErr;
   typedef Nucleus::ErrorCode< OSStatus, noDtaMkErr >                              NoDtaMkErr;
   typedef Nucleus::ErrorCode< OSStatus, badDCksum >                               BadDCksum;
   typedef Nucleus::ErrorCode< OSStatus, badDBtSlp >                               BadDBtSlp;
   typedef Nucleus::ErrorCode< OSStatus, wrUnderrun >                              WrUnderrun;
   typedef Nucleus::ErrorCode< OSStatus, cantStepErr >                             CantStepErr;
   typedef Nucleus::ErrorCode< OSStatus, tk0BadErr >                               Tk0BadErr;
   typedef Nucleus::ErrorCode< OSStatus, initIWMErr >                              InitIWMErr;
   typedef Nucleus::ErrorCode< OSStatus, twoSideErr >                              TwoSideErr;
   typedef Nucleus::ErrorCode< OSStatus, spdAdjErr >                               SpdAdjErr;
   typedef Nucleus::ErrorCode< OSStatus, seekErr >                                 SeekErr;
   typedef Nucleus::ErrorCode< OSStatus, sectNFErr >                               SectNFErr;
   typedef Nucleus::ErrorCode< OSStatus, fmt1Err >                                 Fmt1Err;
   typedef Nucleus::ErrorCode< OSStatus, fmt2Err >                                 Fmt2Err;
   typedef Nucleus::ErrorCode< OSStatus, verErr >                                  VerErr;
   typedef Nucleus::ErrorCode< OSStatus, clkRdErr >                                ClkRdErr;
   typedef Nucleus::ErrorCode< OSStatus, clkWrErr >                                ClkWrErr;
   typedef Nucleus::ErrorCode< OSStatus, prWrErr >                                 PrWrErr;
   typedef Nucleus::ErrorCode< OSStatus, prInitErr >                               PrInitErr;
   typedef Nucleus::ErrorCode< OSStatus, rcvrErr >                                 RcvrErr;
   typedef Nucleus::ErrorCode< OSStatus, breakRecd >                               BreakRecd;

/*Scrap Manager errors*/
   typedef Nucleus::ErrorCode< OSStatus, noScrapErr >                              NoScrapErr;
   typedef Nucleus::ErrorCode< OSStatus, noTypeErr >                               NoTypeErr;

/* ENET error codes */
   typedef Nucleus::ErrorCode< OSStatus, eLenErr >                                 ELenErr;
   typedef Nucleus::ErrorCode< OSStatus, eMultiErr >                               EMultiErr;

   typedef Nucleus::ErrorCode< OSStatus, ddpSktErr >                               DDPSktErr;
   typedef Nucleus::ErrorCode< OSStatus, ddpLenErr >                               DDPLenErr;
   typedef Nucleus::ErrorCode< OSStatus, noBridgeErr >                             NoBridgeErr;
   typedef Nucleus::ErrorCode< OSStatus, lapProtErr >                              LAPProtErr;
   typedef Nucleus::ErrorCode< OSStatus, excessCollsns >                           ExcessCollsns;
   typedef Nucleus::ErrorCode< OSStatus, portNotPwr >                              PortNotPwr;
   typedef Nucleus::ErrorCode< OSStatus, portInUse >                               PortInUse;
   typedef Nucleus::ErrorCode< OSStatus, portNotCf >                               PortNotCf;

/* Memory Manager errors*/
   typedef Nucleus::ErrorCode< OSStatus, memROZWarn >                              MemROZWarn;
   typedef Nucleus::ErrorCode< OSStatus, memROZError >                             MemROZError;
   typedef Nucleus::ErrorCode< OSStatus, memROZErr >                               MemROZErr;
   typedef Nucleus::ErrorCode< OSStatus, memFullErr >                              MemFullErr;
   typedef Nucleus::ErrorCode< OSStatus, nilHandleErr >                            NilHandleErr;
   typedef Nucleus::ErrorCode< OSStatus, memWZErr >                                MemWZErr;
   typedef Nucleus::ErrorCode< OSStatus, memPurErr >                               MemPurErr;
   typedef Nucleus::ErrorCode< OSStatus, memAdrErr >                               MemAdrErr;
   typedef Nucleus::ErrorCode< OSStatus, memAZErr >                                MemAZErr;
   typedef Nucleus::ErrorCode< OSStatus, memPCErr >                                MemPCErr;
   typedef Nucleus::ErrorCode< OSStatus, memBCErr >                                MemBCErr;
   typedef Nucleus::ErrorCode< OSStatus, memSCErr >                                MemSCErr;
   typedef Nucleus::ErrorCode< OSStatus, memLockedErr >                            MemLockedErr;

/* Printing Errors */
   typedef Nucleus::ErrorCode< OSStatus, iMemFullErr >                             IMemFullErr;
   typedef Nucleus::ErrorCode< OSStatus, iIOAbort >                                IIOAbort;


   typedef Nucleus::ErrorCode< OSStatus, resourceInMemory >                        ResourceInMemory;
   typedef Nucleus::ErrorCode< OSStatus, writingPastEnd >                          WritingPastEnd;
   typedef Nucleus::ErrorCode< OSStatus, inputOutOfBounds >                        InputOutOfBounds;
   typedef Nucleus::ErrorCode< OSStatus, resNotFound >                             ResNotFound;
   typedef Nucleus::ErrorCode< OSStatus, resFNotFound >                            ResFNotFound;
   typedef Nucleus::ErrorCode< OSStatus, addResFailed >                            AddResFailed;
   typedef Nucleus::ErrorCode< OSStatus, addRefFailed >                            AddRefFailed;
   typedef Nucleus::ErrorCode< OSStatus, rmvResFailed >                            RmvResFailed;
   typedef Nucleus::ErrorCode< OSStatus, rmvRefFailed >                            RmvRefFailed;
   typedef Nucleus::ErrorCode< OSStatus, resAttrErr >                              ResAttrErr;
   typedef Nucleus::ErrorCode< OSStatus, mapReadErr >                              MapReadErr;
   typedef Nucleus::ErrorCode< OSStatus, CantDecompress >                          CantDecompress;
   typedef Nucleus::ErrorCode< OSStatus, badExtResource >                          BadExtResource;
   typedef Nucleus::ErrorCode< OSStatus, noMemForPictPlaybackErr >                 NoMemForPictPlaybackErr;
   typedef Nucleus::ErrorCode< OSStatus, rgnOverflowErr >                          RgnOverflowErr;
   typedef Nucleus::ErrorCode< OSStatus, rgnTooBigError >                          RgnTooBigError;
   typedef Nucleus::ErrorCode< OSStatus, pixMapTooDeepErr >                        PixMapTooDeepErr;
   typedef Nucleus::ErrorCode< OSStatus, insufficientStackErr >                    InsufficientStackErr;
   typedef Nucleus::ErrorCode< OSStatus, nsStackErr >                              NSStackErr;

   typedef Nucleus::ErrorCode< OSStatus, evtNotEnb >                               EvtNotEnb;

/* OffScreen QuickDraw Errors */
   typedef Nucleus::ErrorCode< OSStatus, cMatchErr >                               CMatchErr;
   typedef Nucleus::ErrorCode< OSStatus, cTempMemErr >                             CTempMemErr;
   typedef Nucleus::ErrorCode< OSStatus, cNoMemErr >                               CNoMemErr;
   typedef Nucleus::ErrorCode< OSStatus, cRangeErr >                               CRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, cProtectErr >                             CProtectErr;
   typedef Nucleus::ErrorCode< OSStatus, cDevErr >                                 CDevErr;
   typedef Nucleus::ErrorCode< OSStatus, cResErr >                                 CResErr;
   typedef Nucleus::ErrorCode< OSStatus, cDepthErr >                               CDepthErr;
   typedef Nucleus::ErrorCode< OSStatus, rgnTooBigErr >                            RgnTooBigErr;
   typedef Nucleus::ErrorCode< OSStatus, updPixMemErr >                            UpdPixMemErr;
   typedef Nucleus::ErrorCode< OSStatus, pictInfoVersionErr >                      PictInfoVersionErr;
   typedef Nucleus::ErrorCode< OSStatus, pictInfoIDErr >                           PictInfoIDErr;
   typedef Nucleus::ErrorCode< OSStatus, pictInfoVerbErr >                         PictInfoVerbErr;
   typedef Nucleus::ErrorCode< OSStatus, cantLoadPickMethodErr >                   CantLoadPickMethodErr;
   typedef Nucleus::ErrorCode< OSStatus, colorsRequestedErr >                      ColorsRequestedErr;
   typedef Nucleus::ErrorCode< OSStatus, pictureDataErr >                          PictureDataErr;

/* ColorSync Result codes */
   typedef Nucleus::ErrorCode< OSStatus, cmProfileError >                          CMProfileError;
   typedef Nucleus::ErrorCode< OSStatus, cmMethodError >                           CMMethodError;
   typedef Nucleus::ErrorCode< OSStatus, cmMethodNotFound >                        CMMethodNotFound;
   typedef Nucleus::ErrorCode< OSStatus, cmProfileNotFound >                       CMProfileNotFound;
   typedef Nucleus::ErrorCode< OSStatus, cmProfilesIdentical >                     CMProfilesIdentical;
   typedef Nucleus::ErrorCode< OSStatus, cmCantConcatenateError >                  CMCantConcatenateError;
   typedef Nucleus::ErrorCode< OSStatus, cmCantXYZ >                               CMCantXYZ;
   typedef Nucleus::ErrorCode< OSStatus, cmCantDeleteProfile >                     CMCantDeleteProfile;
   typedef Nucleus::ErrorCode< OSStatus, cmUnsupportedDataType >                   CMUnsupportedDataType;
   typedef Nucleus::ErrorCode< OSStatus, cmNoCurrentProfile >                      CMNoCurrentProfile;


/*Sound Manager errors*/
   typedef Nucleus::ErrorCode< OSStatus, noHardware >                              NoHardware;
   typedef Nucleus::ErrorCode< OSStatus, notEnoughHardware >                       NotEnoughHardware;
   typedef Nucleus::ErrorCode< OSStatus, queueFull >                               QueueFull;
   typedef Nucleus::ErrorCode< OSStatus, resProblem >                              ResProblem;
   typedef Nucleus::ErrorCode< OSStatus, badChannel >                              BadChannel;
   typedef Nucleus::ErrorCode< OSStatus, badFormat >                               BadFormat;
   typedef Nucleus::ErrorCode< OSStatus, notEnoughBufferSpace >                    NotEnoughBufferSpace;
   typedef Nucleus::ErrorCode< OSStatus, badFileFormat >                           BadFileFormat;
   typedef Nucleus::ErrorCode< OSStatus, channelBusy >                             ChannelBusy;
   typedef Nucleus::ErrorCode< OSStatus, buffersTooSmall >                         BuffersTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, channelNotBusy >                          ChannelNotBusy;
   typedef Nucleus::ErrorCode< OSStatus, noMoreRealTime >                          NoMoreRealTime;
   typedef Nucleus::ErrorCode< OSStatus, siVBRCompressionNotSupported >            SIVBRCompressionNotSupported;
   typedef Nucleus::ErrorCode< OSStatus, siNoSoundInHardware >                     SINoSoundInHardware;
   typedef Nucleus::ErrorCode< OSStatus, siBadSoundInDevice >                      SIBadSoundInDevice;
   typedef Nucleus::ErrorCode< OSStatus, siNoBufferSpecified >                     SINoBufferSpecified;
   typedef Nucleus::ErrorCode< OSStatus, siInvalidCompression >                    SIInvalidCompression;
   typedef Nucleus::ErrorCode< OSStatus, siHardDriveTooSlow >                      SIHardDriveTooSlow;
   typedef Nucleus::ErrorCode< OSStatus, siInvalidSampleRate >                     SIInvalidSampleRate;
   typedef Nucleus::ErrorCode< OSStatus, siInvalidSampleSize >                     SIInvalidSampleSize;
   typedef Nucleus::ErrorCode< OSStatus, siDeviceBusyErr >                         SIDeviceBusyErr;
   typedef Nucleus::ErrorCode< OSStatus, siBadDeviceName >                         SIBadDeviceName;
   typedef Nucleus::ErrorCode< OSStatus, siBadRefNum >                             SIBadRefNum;
   typedef Nucleus::ErrorCode< OSStatus, siInputDeviceErr >                        SIInputDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, siUnknownInfoType >                       SIUnknownInfoType;
   typedef Nucleus::ErrorCode< OSStatus, siUnknownQuality >                        SIUnknownQuality;

/*Speech Manager errors*/
   typedef Nucleus::ErrorCode< OSStatus, noSynthFound >                            NoSynthFound;
   typedef Nucleus::ErrorCode< OSStatus, synthOpenFailed >                         SynthOpenFailed;
   typedef Nucleus::ErrorCode< OSStatus, synthNotReady >                           SynthNotReady;
   typedef Nucleus::ErrorCode< OSStatus, bufTooSmall >                             BufTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, voiceNotFound >                           VoiceNotFound;
   typedef Nucleus::ErrorCode< OSStatus, incompatibleVoice >                       IncompatibleVoice;
   typedef Nucleus::ErrorCode< OSStatus, badDictFormat >                           BadDictFormat;
   typedef Nucleus::ErrorCode< OSStatus, badInputText >                            BadInputText;

/* Midi Manager Errors: */
   typedef Nucleus::ErrorCode< OSStatus, midiNoClientErr >                         MIDINoClientErr;
   typedef Nucleus::ErrorCode< OSStatus, midiNoPortErr >                           MIDINoPortErr;
   typedef Nucleus::ErrorCode< OSStatus, midiTooManyPortsErr >                     MIDITooManyPortsErr;
   typedef Nucleus::ErrorCode< OSStatus, midiTooManyConsErr >                      MIDITooManyConsErr;
   typedef Nucleus::ErrorCode< OSStatus, midiVConnectErr >                         MIDIVConnectErr;
   typedef Nucleus::ErrorCode< OSStatus, midiVConnectMade >                        MIDIVConnectMade;
   typedef Nucleus::ErrorCode< OSStatus, midiVConnectRmvd >                        MIDIVConnectRmvd;
   typedef Nucleus::ErrorCode< OSStatus, midiNoConErr >                            MIDINoConErr;
   typedef Nucleus::ErrorCode< OSStatus, midiWriteErr >                            MIDIWriteErr;
   typedef Nucleus::ErrorCode< OSStatus, midiNameLenErr >                          MIDINameLenErr;
   typedef Nucleus::ErrorCode< OSStatus, midiDupIDErr >                            MIDIDupIDErr;
   typedef Nucleus::ErrorCode< OSStatus, midiInvalidCmdErr >                       MIDIInvalidCmdErr;


   typedef Nucleus::ErrorCode< OSStatus, nmTypErr >                                NMTypErr;


   typedef Nucleus::ErrorCode< OSStatus, siInitSDTblErr >                          SIInitSDTblErr;
   typedef Nucleus::ErrorCode< OSStatus, siInitVBLQsErr >                          SIInitVBLQsErr;
   typedef Nucleus::ErrorCode< OSStatus, siInitSPTblErr >                          SIInitSPTblErr;
   typedef Nucleus::ErrorCode< OSStatus, sdmJTInitErr >                            SDMJTInitErr;
   typedef Nucleus::ErrorCode< OSStatus, sdmInitErr >                              SDMInitErr;
   typedef Nucleus::ErrorCode< OSStatus, sdmSRTInitErr >                           SDMSRTInitErr;
   typedef Nucleus::ErrorCode< OSStatus, sdmPRAMInitErr >                          SDMPRAMInitErr;
   typedef Nucleus::ErrorCode< OSStatus, sdmPriInitErr >                           SDMPriInitErr;

   typedef Nucleus::ErrorCode< OSStatus, smSDMInitErr >                            SMSDMInitErr;
   typedef Nucleus::ErrorCode< OSStatus, smSRTInitErr >                            SMSRTInitErr;
   typedef Nucleus::ErrorCode< OSStatus, smPRAMInitErr >                           SMPRAMInitErr;
   typedef Nucleus::ErrorCode< OSStatus, smPriInitErr >                            SMPriInitErr;
   typedef Nucleus::ErrorCode< OSStatus, smEmptySlot >                             SMEmptySlot;
   typedef Nucleus::ErrorCode< OSStatus, smCRCFail >                               SMCRCFail;
   typedef Nucleus::ErrorCode< OSStatus, smFormatErr >                             SMFormatErr;
   typedef Nucleus::ErrorCode< OSStatus, smRevisionErr >                           SMRevisionErr;
   typedef Nucleus::ErrorCode< OSStatus, smNoDir >                                 SMNoDir;
   typedef Nucleus::ErrorCode< OSStatus, smDisabledSlot >                          SMDisabledSlot;
   typedef Nucleus::ErrorCode< OSStatus, smNosInfoArray >                          SMNosInfoArray;


   typedef Nucleus::ErrorCode< OSStatus, smResrvErr >                              SMResrvErr;
   typedef Nucleus::ErrorCode< OSStatus, smUnExBusErr >                            SMUnExBusErr;
   typedef Nucleus::ErrorCode< OSStatus, smBLFieldBad >                            SMBLFieldBad;
   typedef Nucleus::ErrorCode< OSStatus, smFHBlockRdErr >                          SMFHBlockRdErr;
   typedef Nucleus::ErrorCode< OSStatus, smFHBlkDispErr >                          SMFHBlkDispErr;
   typedef Nucleus::ErrorCode< OSStatus, smDisposePErr >                           SMDisposePErr;
   typedef Nucleus::ErrorCode< OSStatus, smNoBoardSRsrc >                          SMNoBoardSRsrc;
   typedef Nucleus::ErrorCode< OSStatus, smGetPRErr >                              SMGetPRErr;
   typedef Nucleus::ErrorCode< OSStatus, smNoBoardId >                             SMNoBoardId;
   typedef Nucleus::ErrorCode< OSStatus, smInitStatVErr >                          SMInitStatVErr;
   typedef Nucleus::ErrorCode< OSStatus, smInitTblVErr >                           SMInitTblVErr;
   typedef Nucleus::ErrorCode< OSStatus, smNoJmpTbl >                              SMNoJmpTbl;
   typedef Nucleus::ErrorCode< OSStatus, smReservedSlot >                          SMReservedSlot;
   typedef Nucleus::ErrorCode< OSStatus, smBadBoardId >                            SMBadBoardId;
   typedef Nucleus::ErrorCode< OSStatus, smBusErrTO >                              SMBusErrTO;

   typedef Nucleus::ErrorCode< OSStatus, svTempDisable >                           SVTempDisable;
   typedef Nucleus::ErrorCode< OSStatus, svDisabled >                              SVDisabled;
   typedef Nucleus::ErrorCode< OSStatus, smBadRefId >                              SMBadRefId;
   typedef Nucleus::ErrorCode< OSStatus, smBadsList >                              SMBadsList;
   typedef Nucleus::ErrorCode< OSStatus, smReservedErr >                           SMReservedErr;
   typedef Nucleus::ErrorCode< OSStatus, smCodeRevErr >                            SMCodeRevErr;

   typedef Nucleus::ErrorCode< OSStatus, smCPUErr >                                SMCPUErr;
   typedef Nucleus::ErrorCode< OSStatus, smsPointerNil >                           SMSPointerNil;
   typedef Nucleus::ErrorCode< OSStatus, smNilsBlockErr >                          SMNilsBlockErr;
   typedef Nucleus::ErrorCode< OSStatus, smSlotOOBErr >                            SMSlotOOBErr;
   typedef Nucleus::ErrorCode< OSStatus, smSelOOBErr >                             SMSelOOBErr;
   typedef Nucleus::ErrorCode< OSStatus, smNewPErr >                               SMNewPErr;
   typedef Nucleus::ErrorCode< OSStatus, smBlkMoveErr >                            SMBlkMoveErr;
   typedef Nucleus::ErrorCode< OSStatus, smCkStatusErr >                           SMCkStatusErr;
   typedef Nucleus::ErrorCode< OSStatus, smGetDrvrNamErr >                         SMGetDrvrNamErr;
   typedef Nucleus::ErrorCode< OSStatus, smDisDrvrNamErr >                         SMDisDrvrNamErr;
   typedef Nucleus::ErrorCode< OSStatus, smNoMoresRsrcs >                          SMNoMoresRsrcs;
   typedef Nucleus::ErrorCode< OSStatus, smsGetDrvrErr >                           SMSGetDrvrErr;
   typedef Nucleus::ErrorCode< OSStatus, smBadsPtrErr >                            SMBadsPtrErr;
   typedef Nucleus::ErrorCode< OSStatus, smByteLanesErr >                          SMByteLanesErr;
   typedef Nucleus::ErrorCode< OSStatus, smOffsetErr >                             SMOffsetErr;
   typedef Nucleus::ErrorCode< OSStatus, smNoGoodOpens >                           SMNoGoodOpens;
   typedef Nucleus::ErrorCode< OSStatus, smSRTOvrFlErr >                           SMSRTOvrFlErr;
   typedef Nucleus::ErrorCode< OSStatus, smRecNotFnd >                             SMRecNotFnd;


/*Dictionary Manager errors*/
   typedef Nucleus::ErrorCode< OSStatus, notBTree >                                NotBTree;
   typedef Nucleus::ErrorCode< OSStatus, btNoSpace >                               BTNoSpace;
   typedef Nucleus::ErrorCode< OSStatus, btDupRecErr >                             BTDupRecErr;
   typedef Nucleus::ErrorCode< OSStatus, btRecNotFnd >                             BTRecNotFnd;
   typedef Nucleus::ErrorCode< OSStatus, btKeyLenErr >                             BTKeyLenErr;
   typedef Nucleus::ErrorCode< OSStatus, btKeyAttrErr >                            BTKeyAttrErr;
   typedef Nucleus::ErrorCode< OSStatus, unknownInsertModeErr >                    UnknownInsertModeErr;
   typedef Nucleus::ErrorCode< OSStatus, recordDataTooBigErr >                     RecordDataTooBigErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidIndexErr >                         InvalidIndexErr;


/* Error codes from FSM functions */
   typedef Nucleus::ErrorCode< OSStatus, fsmFFSNotFoundErr >                       FSMFFSNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, fsmBusyFFSErr >                           FSMBusyFFSErr;
   typedef Nucleus::ErrorCode< OSStatus, fsmBadFFSNameErr >                        FSMBadFFSNameErr;
   typedef Nucleus::ErrorCode< OSStatus, fsmBadFSDLenErr >                         FSMBadFSDLenErr;
   typedef Nucleus::ErrorCode< OSStatus, fsmDuplicateFSIDErr >                     FSMDuplicateFSIDErr;
   typedef Nucleus::ErrorCode< OSStatus, fsmBadFSDVersionErr >                     FSMBadFSDVersionErr;
   typedef Nucleus::ErrorCode< OSStatus, fsmNoAlternateStackErr >                  FSMNoAlternateStackErr;
   typedef Nucleus::ErrorCode< OSStatus, fsmUnknownFSMMessageErr >                 FSMUnknownFSMMessageErr;


/* Edition Mgr errors*/
   typedef Nucleus::ErrorCode< OSStatus, editionMgrInitErr >                       EditionMgrInitErr;
   typedef Nucleus::ErrorCode< OSStatus, badSectionErr >                           BadSectionErr;
   typedef Nucleus::ErrorCode< OSStatus, notRegisteredSectionErr >                 NotRegisteredSectionErr;
   typedef Nucleus::ErrorCode< OSStatus, badEditionFileErr >                       BadEditionFileErr;
   typedef Nucleus::ErrorCode< OSStatus, badSubPartErr >                           BadSubPartErr;
   typedef Nucleus::ErrorCode< OSStatus, multiplePublisherWrn >                    MultiplePublisherWrn;
   typedef Nucleus::ErrorCode< OSStatus, containerNotFoundWrn >                    ContainerNotFoundWrn;
   typedef Nucleus::ErrorCode< OSStatus, containerAlreadyOpenWrn >                 ContainerAlreadyOpenWrn;
   typedef Nucleus::ErrorCode< OSStatus, notThePublisherWrn >                      NotThePublisherWrn;

   typedef Nucleus::ErrorCode< OSStatus, teScrapSizeErr >                          TEScrapSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, hwParamErr >                              HWParamErr;
   typedef Nucleus::ErrorCode< OSStatus, driverHardwareGoneErr >                   DriverHardwareGoneErr;

/*Process Manager errors*/
   typedef Nucleus::ErrorCode< OSStatus, procNotFound >                            ProcNotFound;
   typedef Nucleus::ErrorCode< OSStatus, memFragErr >                              MemFragErr;
   typedef Nucleus::ErrorCode< OSStatus, appModeErr >                              AppModeErr;
   typedef Nucleus::ErrorCode< OSStatus, protocolErr >                             ProtocolErr;
   typedef Nucleus::ErrorCode< OSStatus, hardwareConfigErr >                       HardwareConfigErr;
   typedef Nucleus::ErrorCode< OSStatus, appMemFullErr >                           AppMemFullErr;
   typedef Nucleus::ErrorCode< OSStatus, appIsDaemon >                             AppIsDaemon;
   typedef Nucleus::ErrorCode< OSStatus, bufferIsSmall >                           BufferIsSmall;
   typedef Nucleus::ErrorCode< OSStatus, noOutstandingHLE >                        NoOutstandingHLE;
   typedef Nucleus::ErrorCode< OSStatus, connectionInvalid >                       ConnectionInvalid;
   typedef Nucleus::ErrorCode< OSStatus, noUserInteractionAllowed >                NoUserInteractionAllowed;
   typedef Nucleus::ErrorCode< OSStatus, wrongApplicationPlatform >                WrongApplicationPlatform;
   typedef Nucleus::ErrorCode< OSStatus, appVersionTooOld >                        AppVersionTooOld;
   typedef Nucleus::ErrorCode< OSStatus, notAppropriateForClassic >                NotAppropriateForClassic;

/* Thread Manager Error Codes */
   typedef Nucleus::ErrorCode< OSStatus, threadTooManyReqsErr >                    ThreadTooManyReqsErr;
   typedef Nucleus::ErrorCode< OSStatus, threadNotFoundErr >                       ThreadNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, threadProtocolErr >                       ThreadProtocolErr;

/*MemoryDispatch errors*/
   typedef Nucleus::ErrorCode< OSStatus, notEnoughMemoryErr >                      NotEnoughMemoryErr;
   typedef Nucleus::ErrorCode< OSStatus, notHeldErr >                              NotHeldErr;
   typedef Nucleus::ErrorCode< OSStatus, cannotMakeContiguousErr >                 CannotMakeContiguousErr;
   typedef Nucleus::ErrorCode< OSStatus, notLockedErr >                            NotLockedErr;
   typedef Nucleus::ErrorCode< OSStatus, interruptsMaskedErr >                     InterruptsMaskedErr;
   typedef Nucleus::ErrorCode< OSStatus, cannotDeferErr >                          CannotDeferErr;
   typedef Nucleus::ErrorCode< OSStatus, noMMUErr >                                NoMMUErr;

/* FileMapping errors */
   typedef Nucleus::ErrorCode< OSStatus, vmInvalidBackingFileIDErr >               VMInvalidBackingFileIDErr;
   typedef Nucleus::ErrorCode< OSStatus, vmMappingPrivilegesErr >                  VMMappingPrivilegesErr;
   typedef Nucleus::ErrorCode< OSStatus, vmBusyBackingFileErr >                    VMBusyBackingFileErr;
   typedef Nucleus::ErrorCode< OSStatus, vmNoMoreBackingFilesErr >                 VMNoMoreBackingFilesErr;
   typedef Nucleus::ErrorCode< OSStatus, vmInvalidFileViewIDErr >                  VMInvalidFileViewIDErr;
   typedef Nucleus::ErrorCode< OSStatus, vmFileViewAccessErr >                     VMFileViewAccessErr;
   typedef Nucleus::ErrorCode< OSStatus, vmNoMoreFileViewsErr >                    VMNoMoreFileViewsErr;
   typedef Nucleus::ErrorCode< OSStatus, vmAddressNotInFileViewErr >               VMAddressNotInFileViewErr;
   typedef Nucleus::ErrorCode< OSStatus, vmInvalidOwningProcessErr >               VMInvalidOwningProcessErr;

/* Database access error codes */
   typedef Nucleus::ErrorCode< OSStatus, rcDBNull >                                RCDBNull;
   typedef Nucleus::ErrorCode< OSStatus, rcDBValue >                               RCDBValue;
   typedef Nucleus::ErrorCode< OSStatus, rcDBError >                               RCDBError;
   typedef Nucleus::ErrorCode< OSStatus, rcDBBadType >                             RCDBBadType;
   typedef Nucleus::ErrorCode< OSStatus, rcDBBreak >                               RCDBBreak;
   typedef Nucleus::ErrorCode< OSStatus, rcDBExec >                                RCDBExec;
   typedef Nucleus::ErrorCode< OSStatus, rcDBBadSessID >                           RCDBBadSessID;
   typedef Nucleus::ErrorCode< OSStatus, rcDBBadSessNum >                          RCDBBadSessNum;
   typedef Nucleus::ErrorCode< OSStatus, rcDBBadDDEV >                             RCDBBadDDEV;
   typedef Nucleus::ErrorCode< OSStatus, rcDBAsyncNotSupp >                        RCDBAsyncNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, rcDBBadAsyncPB >                          RCDBBadAsyncPB;
   typedef Nucleus::ErrorCode< OSStatus, rcDBNoHandler >                           RCDBNoHandler;
   typedef Nucleus::ErrorCode< OSStatus, rcDBWrongVersion >                        RCDBWrongVersion;
   typedef Nucleus::ErrorCode< OSStatus, rcDBPackNotInited >                       RCDBPackNotInited;


/*Help Mgr error range: -850 to -874*/
   typedef Nucleus::ErrorCode< OSStatus, hmHelpDisabled >                          HMHelpDisabled;
   typedef Nucleus::ErrorCode< OSStatus, hmBalloonAborted >                        HMBalloonAborted;
   typedef Nucleus::ErrorCode< OSStatus, hmSameAsLastBalloon >                     HMSameAsLastBalloon;
   typedef Nucleus::ErrorCode< OSStatus, hmHelpManagerNotInited >                  HMHelpManagerNotInited;
   typedef Nucleus::ErrorCode< OSStatus, hmSkippedBalloon >                        HMSkippedBalloon;
   typedef Nucleus::ErrorCode< OSStatus, hmWrongVersion >                          HMWrongVersion;
   typedef Nucleus::ErrorCode< OSStatus, hmUnknownHelpType >                       HMUnknownHelpType;
   typedef Nucleus::ErrorCode< OSStatus, hmOperationUnsupported >                  HMOperationUnsupported;
   typedef Nucleus::ErrorCode< OSStatus, hmNoBalloonUp >                           HMNoBalloonUp;
   typedef Nucleus::ErrorCode< OSStatus, hmCloseViewActive >                       HMCloseViewActive;



/*PPC errors*/
   typedef Nucleus::ErrorCode< OSStatus, notInitErr >                              NotInitErr;
   typedef Nucleus::ErrorCode< OSStatus, nameTypeErr >                             NameTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, noPortErr >                               NoPortErr;
   typedef Nucleus::ErrorCode< OSStatus, noGlobalsErr >                            NoGlobalsErr;
   typedef Nucleus::ErrorCode< OSStatus, localOnlyErr >                            LocalOnlyErr;
   typedef Nucleus::ErrorCode< OSStatus, destPortErr >                             DestPortErr;
   typedef Nucleus::ErrorCode< OSStatus, sessTableErr >                            SessTableErr;
   typedef Nucleus::ErrorCode< OSStatus, noSessionErr >                            NoSessionErr;
   typedef Nucleus::ErrorCode< OSStatus, badReqErr >                               BadReqErr;
   typedef Nucleus::ErrorCode< OSStatus, portNameExistsErr >                       PortNameExistsErr;
   typedef Nucleus::ErrorCode< OSStatus, noUserNameErr >                           NoUserNameErr;
   typedef Nucleus::ErrorCode< OSStatus, userRejectErr >                           UserRejectErr;
   typedef Nucleus::ErrorCode< OSStatus, noMachineNameErr >                        NoMachineNameErr;
   typedef Nucleus::ErrorCode< OSStatus, noToolboxNameErr >                        NoToolboxNameErr;
   typedef Nucleus::ErrorCode< OSStatus, noResponseErr >                           NoResponseErr;
   typedef Nucleus::ErrorCode< OSStatus, portClosedErr >                           PortClosedErr;
   typedef Nucleus::ErrorCode< OSStatus, sessClosedErr >                           SessClosedErr;
   typedef Nucleus::ErrorCode< OSStatus, badPortNameErr >                          BadPortNameErr;
   typedef Nucleus::ErrorCode< OSStatus, noDefaultUserErr >                        NoDefaultUserErr;
   typedef Nucleus::ErrorCode< OSStatus, notLoggedInErr >                          NotLoggedInErr;
   typedef Nucleus::ErrorCode< OSStatus, noUserRefErr >                            NoUserRefErr;
   typedef Nucleus::ErrorCode< OSStatus, networkErr >                              NetworkErr;
   typedef Nucleus::ErrorCode< OSStatus, noInformErr >                             NoInformErr;
   typedef Nucleus::ErrorCode< OSStatus, authFailErr >                             AuthFailErr;
   typedef Nucleus::ErrorCode< OSStatus, noUserRecErr >                            NoUserRecErr;
   typedef Nucleus::ErrorCode< OSStatus, badServiceMethodErr >                     BadServiceMethodErr;
   typedef Nucleus::ErrorCode< OSStatus, badLocNameErr >                           BadLocNameErr;
   typedef Nucleus::ErrorCode< OSStatus, guestNotAllowedErr >                      GuestNotAllowedErr;

/* Font Mgr errors*/
   typedef Nucleus::ErrorCode< OSStatus, kFMIterationCompleted >                   FMIterationCompleted;
   typedef Nucleus::ErrorCode< OSStatus, kFMInvalidFontFamilyErr >                 FMInvalidFontFamilyErr;
   typedef Nucleus::ErrorCode< OSStatus, kFMInvalidFontErr >                       FMInvalidFontErr;
   typedef Nucleus::ErrorCode< OSStatus, kFMIterationScopeModifiedErr >            FMIterationScopeModifiedErr;
   typedef Nucleus::ErrorCode< OSStatus, kFMFontTableAccessErr >                   FMFontTableAccessErr;
   typedef Nucleus::ErrorCode< OSStatus, kFMFontContainerAccessErr >               FMFontContainerAccessErr;

   typedef Nucleus::ErrorCode< OSStatus, noMaskFoundErr >                          NoMaskFoundErr;

   typedef Nucleus::ErrorCode< OSStatus, nbpBuffOvr >                              NBPBuffOvr;
   typedef Nucleus::ErrorCode< OSStatus, nbpNoConfirm >                            NBPNoConfirm;
   typedef Nucleus::ErrorCode< OSStatus, nbpConfDiff >                             NBPConfDiff;
   typedef Nucleus::ErrorCode< OSStatus, nbpDuplicate >                            NBPDuplicate;
   typedef Nucleus::ErrorCode< OSStatus, nbpNotFound >                             NBPNotFound;
   typedef Nucleus::ErrorCode< OSStatus, nbpNISErr >                               NBPNISErr;

   typedef Nucleus::ErrorCode< OSStatus, aspBadVersNum >                           ASPBadVersNum;
   typedef Nucleus::ErrorCode< OSStatus, aspBufTooSmall >                          ASPBufTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, aspNoMoreSess >                           ASPNoMoreSess;
   typedef Nucleus::ErrorCode< OSStatus, aspNoServers >                            ASPNoServers;
   typedef Nucleus::ErrorCode< OSStatus, aspParamErr >                             ASPParamErr;
   typedef Nucleus::ErrorCode< OSStatus, aspServerBusy >                           ASPServerBusy;
   typedef Nucleus::ErrorCode< OSStatus, aspSessClosed >                           ASPSessClosed;
   typedef Nucleus::ErrorCode< OSStatus, aspSizeErr >                              ASPSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, aspTooMany >                              ASPTooMany;
   typedef Nucleus::ErrorCode< OSStatus, aspNoAck >                                ASPNoAck;

   typedef Nucleus::ErrorCode< OSStatus, reqFailed >                               ReqFailed;
   typedef Nucleus::ErrorCode< OSStatus, tooManyReqs >                             TooManyReqs;
   typedef Nucleus::ErrorCode< OSStatus, tooManySkts >                             TooManySkts;
   typedef Nucleus::ErrorCode< OSStatus, badATPSkt >                               BadATPSkt;
   typedef Nucleus::ErrorCode< OSStatus, badBuffNum >                              BadBuffNum;
   typedef Nucleus::ErrorCode< OSStatus, noRelErr >                                NoRelErr;
   typedef Nucleus::ErrorCode< OSStatus, cbNotFound >                              CbNotFound;
   typedef Nucleus::ErrorCode< OSStatus, noSendResp >                              NoSendResp;
   typedef Nucleus::ErrorCode< OSStatus, noDataArea >                              NoDataArea;
   typedef Nucleus::ErrorCode< OSStatus, reqAborted >                              ReqAborted;

/* ADSP Error Codes */
   typedef Nucleus::ErrorCode< OSStatus, errRefNum >                               ErrRefNum;
   typedef Nucleus::ErrorCode< OSStatus, errAborted >                              ErrAborted;
   typedef Nucleus::ErrorCode< OSStatus, errState >                                ErrState;
   typedef Nucleus::ErrorCode< OSStatus, errOpening >                              ErrOpening;
   typedef Nucleus::ErrorCode< OSStatus, errAttention >                            ErrAttention;
   typedef Nucleus::ErrorCode< OSStatus, errFwdReset >                             ErrFwdReset;
   typedef Nucleus::ErrorCode< OSStatus, errDSPQueueSize >                         ErrDSPQueueSize;
   typedef Nucleus::ErrorCode< OSStatus, errOpenDenied >                           ErrOpenDenied;


/* Apple event manager error messages */
   typedef Nucleus::ErrorCode< OSStatus, errAECoercionFail >                       ErrAECoercionFail;
   typedef Nucleus::ErrorCode< OSStatus, errAEDescNotFound >                       ErrAEDescNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errAECorruptData >                        ErrAECorruptData;
   typedef Nucleus::ErrorCode< OSStatus, errAEWrongDataType >                      ErrAEWrongDataType;
   typedef Nucleus::ErrorCode< OSStatus, errAENotAEDesc >                          ErrAENotAEDesc;
   typedef Nucleus::ErrorCode< OSStatus, errAEBadListItem >                        ErrAEBadListItem;
   typedef Nucleus::ErrorCode< OSStatus, errAENewerVersion >                       ErrAENewerVersion;
   typedef Nucleus::ErrorCode< OSStatus, errAENotAppleEvent >                      ErrAENotAppleEvent;
   typedef Nucleus::ErrorCode< OSStatus, errAEEventNotHandled >                    ErrAEEventNotHandled;
   typedef Nucleus::ErrorCode< OSStatus, errAEReplyNotValid >                      ErrAEReplyNotValid;
   typedef Nucleus::ErrorCode< OSStatus, errAEUnknownSendMode >                    ErrAEUnknownSendMode;
   typedef Nucleus::ErrorCode< OSStatus, errAEWaitCanceled >                       ErrAEWaitCanceled;
   typedef Nucleus::ErrorCode< OSStatus, errAETimeout >                            ErrAETimeout;
   typedef Nucleus::ErrorCode< OSStatus, errAENoUserInteraction >                  ErrAENoUserInteraction;
   typedef Nucleus::ErrorCode< OSStatus, errAENotASpecialFunction >                ErrAENotASpecialFunction;
   typedef Nucleus::ErrorCode< OSStatus, errAEParamMissed >                        ErrAEParamMissed;
   typedef Nucleus::ErrorCode< OSStatus, errAEUnknownAddressType >                 ErrAEUnknownAddressType;
   typedef Nucleus::ErrorCode< OSStatus, errAEHandlerNotFound >                    ErrAEHandlerNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errAEReplyNotArrived >                    ErrAEReplyNotArrived;
   typedef Nucleus::ErrorCode< OSStatus, errAEIllegalIndex >                       ErrAEIllegalIndex;
   typedef Nucleus::ErrorCode< OSStatus, errAEImpossibleRange >                    ErrAEImpossibleRange;
   typedef Nucleus::ErrorCode< OSStatus, errAEWrongNumberArgs >                    ErrAEWrongNumberArgs;
   typedef Nucleus::ErrorCode< OSStatus, errAEAccessorNotFound >                   ErrAEAccessorNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errAENoSuchLogical >                      ErrAENoSuchLogical;
   typedef Nucleus::ErrorCode< OSStatus, errAEBadTestKey >                         ErrAEBadTestKey;
   typedef Nucleus::ErrorCode< OSStatus, errAENotAnObjSpec >                       ErrAENotAnObjSpec;
   typedef Nucleus::ErrorCode< OSStatus, errAENoSuchObject >                       ErrAENoSuchObject;
   typedef Nucleus::ErrorCode< OSStatus, errAENegativeCount >                      ErrAENegativeCount;
   typedef Nucleus::ErrorCode< OSStatus, errAEEmptyListContainer >                 ErrAEEmptyListContainer;
   typedef Nucleus::ErrorCode< OSStatus, errAEUnknownObjectType >                  ErrAEUnknownObjectType;
   typedef Nucleus::ErrorCode< OSStatus, errAERecordingIsAlreadyOn >               ErrAERecordingIsAlreadyOn;
   typedef Nucleus::ErrorCode< OSStatus, errAEReceiveTerminate >                   ErrAEReceiveTerminate;
   typedef Nucleus::ErrorCode< OSStatus, errAEReceiveEscapeCurrent >               ErrAEReceiveEscapeCurrent;
   typedef Nucleus::ErrorCode< OSStatus, errAEEventFiltered >                      ErrAEEventFiltered;
   typedef Nucleus::ErrorCode< OSStatus, errAEDuplicateHandler >                   ErrAEDuplicateHandler;
   typedef Nucleus::ErrorCode< OSStatus, errAEStreamBadNesting >                   ErrAEStreamBadNesting;
   typedef Nucleus::ErrorCode< OSStatus, errAEStreamAlreadyConverted>              ErrAEStreamAlreadyConverted;
   typedef Nucleus::ErrorCode< OSStatus, errAEDescIsNull >                         ErrAEDescIsNull;
   typedef Nucleus::ErrorCode< OSStatus, errAEBuildSyntaxError >                   ErrAEBuildSyntaxError;
   typedef Nucleus::ErrorCode< OSStatus, errAEBufferTooSmall >                     ErrAEBufferTooSmall;

   typedef Nucleus::ErrorCode< OSStatus, errOSASystemError >                       ErrOSASystemError;
   typedef Nucleus::ErrorCode< OSStatus, errOSAInvalidID >                         ErrOSAInvalidID;
   typedef Nucleus::ErrorCode< OSStatus, errOSABadStorageType >                    ErrOSABadStorageType;
   typedef Nucleus::ErrorCode< OSStatus, errOSAScriptError >                       ErrOSAScriptError;
   typedef Nucleus::ErrorCode< OSStatus, errOSABadSelector >                       ErrOSABadSelector;
   typedef Nucleus::ErrorCode< OSStatus, errOSASourceNotAvailable >                ErrOSASourceNotAvailable;
   typedef Nucleus::ErrorCode< OSStatus, errOSANoSuchDialect >                     ErrOSANoSuchDialect;
   typedef Nucleus::ErrorCode< OSStatus, errOSADataFormatObsolete >                ErrOSADataFormatObsolete;
   typedef Nucleus::ErrorCode< OSStatus, errOSADataFormatTooNew >                  ErrOSADataFormatTooNew;
   typedef Nucleus::ErrorCode< OSStatus, errOSACorruptData >                       ErrOSACorruptData;
   typedef Nucleus::ErrorCode< OSStatus, errOSARecordingIsAlreadyOn >              ErrOSARecordingIsAlreadyOn;
   typedef Nucleus::ErrorCode< OSStatus, errOSAComponentMismatch >                 ErrOSAComponentMismatch;
   typedef Nucleus::ErrorCode< OSStatus, errOSACantOpenComponent >                 ErrOSACantOpenComponent;



/* AppleEvent error definitions */
   typedef Nucleus::ErrorCode< OSStatus, errOffsetInvalid >                        ErrOffsetInvalid;
   typedef Nucleus::ErrorCode< OSStatus, errOffsetIsOutsideOfView >                ErrOffsetIsOutsideOfView;
   typedef Nucleus::ErrorCode< OSStatus, errTopOfDocument >                        ErrTopOfDocument;
   typedef Nucleus::ErrorCode< OSStatus, errTopOfBody >                            ErrTopOfBody;
   typedef Nucleus::ErrorCode< OSStatus, errEndOfDocument >                        ErrEndOfDocument;
   typedef Nucleus::ErrorCode< OSStatus, errEndOfBody >                            ErrEndOfBody;


/* Drag Manager error codes */
   typedef Nucleus::ErrorCode< OSStatus, badDragRefErr >                           BadDragRefErr;
   typedef Nucleus::ErrorCode< OSStatus, badDragItemErr >                          BadDragItemErr;
   typedef Nucleus::ErrorCode< OSStatus, badDragFlavorErr >                        BadDragFlavorErr;
   typedef Nucleus::ErrorCode< OSStatus, duplicateFlavorErr >                      DuplicateFlavorErr;
   typedef Nucleus::ErrorCode< OSStatus, cantGetFlavorErr >                        CantGetFlavorErr;
   typedef Nucleus::ErrorCode< OSStatus, duplicateHandlerErr >                     DuplicateHandlerErr;
   typedef Nucleus::ErrorCode< OSStatus, handlerNotFoundErr >                      HandlerNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, dragNotAcceptedErr >                      DragNotAcceptedErr;
   typedef Nucleus::ErrorCode< OSStatus, unsupportedForPlatformErr >               UnsupportedForPlatformErr;
   typedef Nucleus::ErrorCode< OSStatus, noSuitableDisplaysErr >                   NoSuitableDisplaysErr;
   typedef Nucleus::ErrorCode< OSStatus, badImageRgnErr >                          BadImageRgnErr;
   typedef Nucleus::ErrorCode< OSStatus, badImageErr >                             BadImageErr;
   typedef Nucleus::ErrorCode< OSStatus, nonDragOriginatorErr >                    NonDragOriginatorErr;


/*QuickTime errors*/
   typedef Nucleus::ErrorCode< OSStatus, couldNotResolveDataRef >                  CouldNotResolveDataRef;
   typedef Nucleus::ErrorCode< OSStatus, badImageDescription >                     BadImageDescription;
   typedef Nucleus::ErrorCode< OSStatus, badPublicMovieAtom >                      BadPublicMovieAtom;
   typedef Nucleus::ErrorCode< OSStatus, cantFindHandler >                         CantFindHandler;
   typedef Nucleus::ErrorCode< OSStatus, cantOpenHandler >                         CantOpenHandler;
   typedef Nucleus::ErrorCode< OSStatus, badComponentType >                        BadComponentType;
   typedef Nucleus::ErrorCode< OSStatus, noMediaHandler >                          NoMediaHandler;
   typedef Nucleus::ErrorCode< OSStatus, noDataHandler >                           NoDataHandler;
   typedef Nucleus::ErrorCode< OSStatus, invalidMedia >                            InvalidMedia;
   typedef Nucleus::ErrorCode< OSStatus, invalidTrack >                            InvalidTrack;
   typedef Nucleus::ErrorCode< OSStatus, invalidMovie >                            InvalidMovie;
   typedef Nucleus::ErrorCode< OSStatus, invalidSampleTable >                      InvalidSampleTable;
   typedef Nucleus::ErrorCode< OSStatus, invalidDataRef >                          InvalidDataRef;
   typedef Nucleus::ErrorCode< OSStatus, invalidHandler >                          InvalidHandler;
   typedef Nucleus::ErrorCode< OSStatus, invalidDuration >                         InvalidDuration;
   typedef Nucleus::ErrorCode< OSStatus, invalidTime >                             InvalidTime;
   typedef Nucleus::ErrorCode< OSStatus, cantPutPublicMovieAtom >                  CantPutPublicMovieAtom;
   typedef Nucleus::ErrorCode< OSStatus, badEditList >                             BadEditList;
   typedef Nucleus::ErrorCode< OSStatus, mediaTypesDontMatch >                     MediaTypesDontMatch;
   typedef Nucleus::ErrorCode< OSStatus, progressProcAborted >                     ProgressProcAborted;
   typedef Nucleus::ErrorCode< OSStatus, movieToolboxUninitialized >               MovieToolboxUninitialized;
   typedef Nucleus::ErrorCode< OSStatus, noRecordOfApp >                           NoRecordOfApp;
   typedef Nucleus::ErrorCode< OSStatus, wfFileNotFound >                          WFFileNotFound;
   typedef Nucleus::ErrorCode< OSStatus, cantCreateSingleForkFile >                CantCreateSingleForkFile;
   typedef Nucleus::ErrorCode< OSStatus, invalidEditState >                        InvalidEditState;
   typedef Nucleus::ErrorCode< OSStatus, nonMatchingEditState >                    NonMatchingEditState;
   typedef Nucleus::ErrorCode< OSStatus, staleEditState >                          StaleEditState;
   typedef Nucleus::ErrorCode< OSStatus, userDataItemNotFound >                    UserDataItemNotFound;
   typedef Nucleus::ErrorCode< OSStatus, maxSizeToGrowTooSmall >                   MaxSizeToGrowTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, badTrackIndex >                           BadTrackIndex;
   typedef Nucleus::ErrorCode< OSStatus, trackIDNotFound >                         TrackIDNotFound;
   typedef Nucleus::ErrorCode< OSStatus, trackNotInMovie >                         TrackNotInMovie;
   typedef Nucleus::ErrorCode< OSStatus, timeNotInTrack >                          TimeNotInTrack;
   typedef Nucleus::ErrorCode< OSStatus, timeNotInMedia >                          TimeNotInMedia;
   typedef Nucleus::ErrorCode< OSStatus, badEditIndex >                            BadEditIndex;
   typedef Nucleus::ErrorCode< OSStatus, internalQuickTimeError >                  InternalQuickTimeError;
   typedef Nucleus::ErrorCode< OSStatus, cantEnableTrack >                         CantEnableTrack;
   typedef Nucleus::ErrorCode< OSStatus, invalidRect >                             InvalidRect;
   typedef Nucleus::ErrorCode< OSStatus, invalidSampleNum >                        InvalidSampleNum;
   typedef Nucleus::ErrorCode< OSStatus, invalidChunkNum >                         InvalidChunkNum;
   typedef Nucleus::ErrorCode< OSStatus, invalidSampleDescIndex >                  InvalidSampleDescIndex;
   typedef Nucleus::ErrorCode< OSStatus, invalidChunkCache >                       InvalidChunkCache;
   typedef Nucleus::ErrorCode< OSStatus, invalidSampleDescription >                InvalidSampleDescription;
   typedef Nucleus::ErrorCode< OSStatus, dataNotOpenForRead >                      DataNotOpenForRead;
   typedef Nucleus::ErrorCode< OSStatus, dataNotOpenForWrite >                     DataNotOpenForWrite;
   typedef Nucleus::ErrorCode< OSStatus, dataAlreadyOpenForWrite >                 DataAlreadyOpenForWrite;
   typedef Nucleus::ErrorCode< OSStatus, dataAlreadyClosed >                       DataAlreadyClosed;
   typedef Nucleus::ErrorCode< OSStatus, endOfDataReached >                        EndOfDataReached;
   typedef Nucleus::ErrorCode< OSStatus, dataNoDataRef >                           DataNoDataRef;
   typedef Nucleus::ErrorCode< OSStatus, noMovieFound >                            NoMovieFound;
   typedef Nucleus::ErrorCode< OSStatus, invalidDataRefContainer >                 InvalidDataRefContainer;
   typedef Nucleus::ErrorCode< OSStatus, badDataRefIndex >                         BadDataRefIndex;
   typedef Nucleus::ErrorCode< OSStatus, noDefaultDataRef >                        NoDefaultDataRef;
   typedef Nucleus::ErrorCode< OSStatus, couldNotUseAnExistingSample>              CouldNotUseAnExistingSample;
   typedef Nucleus::ErrorCode< OSStatus, featureUnsupported >                      FeatureUnsupported;
   typedef Nucleus::ErrorCode< OSStatus, noVideoTrackInMovieErr >                  NoVideoTrackInMovieErr;
   typedef Nucleus::ErrorCode< OSStatus, noSoundTrackInMovieErr >                  NoSoundTrackInMovieErr;
   typedef Nucleus::ErrorCode< OSStatus, soundSupportNotAvailableErr>              SoundSupportNotAvailableErr;
   typedef Nucleus::ErrorCode< OSStatus, unsupportedAuxiliaryImportData >          UnsupportedAuxiliaryImportData;
   typedef Nucleus::ErrorCode< OSStatus, auxiliaryExportDataUnavailable >          AuxiliaryExportDataUnavailable;
   typedef Nucleus::ErrorCode< OSStatus, samplesAlreadyInMediaErr >                SamplesAlreadyInMediaErr;
   typedef Nucleus::ErrorCode< OSStatus, noSourceTreeFoundErr >                    NoSourceTreeFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, sourceNotFoundErr >                       SourceNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, movieTextNotFoundErr >                    MovieTextNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, missingRequiredParameterErr>              MissingRequiredParameterErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidSpriteWorldPropertyErr >           InvalidSpriteWorldPropertyErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidSpritePropertyErr >                InvalidSpritePropertyErr;
   typedef Nucleus::ErrorCode< OSStatus, gWorldsNotSameDepthAndSizeErr >           GWorldsNotSameDepthAndSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidSpriteIndexErr >                   InvalidSpriteIndexErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidImageIndexErr >                    InvalidImageIndexErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidSpriteIDErr >                      InvalidSpriteIDErr;

   typedef Nucleus::ErrorCode< OSStatus, internalComponentErr >                    InternalComponentErr;
   typedef Nucleus::ErrorCode< OSStatus, notImplementedMusicOSErr >                NotImplementedMusicOSErr;
   typedef Nucleus::ErrorCode< OSStatus, cantSendToSynthesizerOSErr >              CantSendToSynthesizerOSErr;
   typedef Nucleus::ErrorCode< OSStatus, cantReceiveFromSynthesizerOSErr >         CantReceiveFromSynthesizerOSErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalVoiceAllocationOSErr>              IllegalVoiceAllocationOSErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalPartOSErr >                        IllegalPartOSErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalChannelOSErr >                     IllegalChannelOSErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalKnobOSErr >                        IllegalKnobOSErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalKnobValueOSErr >                   IllegalKnobValueOSErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalInstrumentOSErr >                  IllegalInstrumentOSErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalControllerOSErr >                  IllegalControllerOSErr;
   typedef Nucleus::ErrorCode< OSStatus, midiManagerAbsentOSErr >                  MIDIManagerAbsentOSErr;
   typedef Nucleus::ErrorCode< OSStatus, synthesizerNotRespondingOSErr >           SynthesizerNotRespondingOSErr;
   typedef Nucleus::ErrorCode< OSStatus, synthesizerOSErr >                        SynthesizerOSErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalNoteChannelOSErr >                 IllegalNoteChannelOSErr;
   typedef Nucleus::ErrorCode< OSStatus, noteChannelNotAllocatedOSErr >            NoteChannelNotAllocatedOSErr;
   typedef Nucleus::ErrorCode< OSStatus, tunePlayerFullOSErr >                     TunePlayerFullOSErr;
   typedef Nucleus::ErrorCode< OSStatus, tuneParseOSErr >                          TuneParseOSErr;
   typedef Nucleus::ErrorCode< OSStatus, noExportProcAvailableErr >                NoExportProcAvailableErr;
   typedef Nucleus::ErrorCode< OSStatus, videoOutputInUseErr >                     VideoOutputInUseErr;

   typedef Nucleus::ErrorCode< OSStatus, componentDllLoadErr >                     ComponentDllLoadErr;
   typedef Nucleus::ErrorCode< OSStatus, componentDllEntryNotFoundErr >            ComponentDllEntryNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, qtmlDllLoadErr >                          QTMLDllLoadErr;
   typedef Nucleus::ErrorCode< OSStatus, qtmlDllEntryNotFoundErr >                 QTMLDllEntryNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, qtmlUninitialized >                       QTMLUninitialized;
   typedef Nucleus::ErrorCode< OSStatus, unsupportedOSErr >                        UnsupportedOSErr;
   typedef Nucleus::ErrorCode< OSStatus, unsupportedProcessorErr >                 UnsupportedProcessorErr;

   typedef Nucleus::ErrorCode< OSStatus, cannotFindAtomErr >                       CannotFindAtomErr;
   typedef Nucleus::ErrorCode< OSStatus, notLeafAtomErr >                          NotLeafAtomErr;
   typedef Nucleus::ErrorCode< OSStatus, atomsNotOfSameTypeErr >                   AtomsNotOfSameTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, atomIndexInvalidErr >                     AtomIndexInvalidErr;
   typedef Nucleus::ErrorCode< OSStatus, duplicateAtomTypeAndIDErr >               DuplicateAtomTypeAndIDErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidAtomErr >                          InvalidAtomErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidAtomContainerErr >                 InvalidAtomContainerErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidAtomTypeErr >                      InvalidAtomTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, cannotBeLeafAtomErr >                     CannotBeLeafAtomErr;
   typedef Nucleus::ErrorCode< OSStatus, pathTooLongErr >                          PathTooLongErr;
   typedef Nucleus::ErrorCode< OSStatus, emptyPathErr >                            EmptyPathErr;
   typedef Nucleus::ErrorCode< OSStatus, noPathMappingErr >                        NoPathMappingErr;
   typedef Nucleus::ErrorCode< OSStatus, pathNotVerifiedErr >                      PathNotVerifiedErr;
   typedef Nucleus::ErrorCode< OSStatus, unknownFormatErr >                        UnknownFormatErr;
   typedef Nucleus::ErrorCode< OSStatus, wackBadFileErr >                          WACKBadFileErr;
   typedef Nucleus::ErrorCode< OSStatus, wackForkNotFoundErr >                     WACKForkNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, wackBadMetaDataErr >                      WACKBadMetaDataErr;
   typedef Nucleus::ErrorCode< OSStatus, qfcbNotFoundErr >                         QFCBNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, qfcbNotCreatedErr >                       QFCBNotCreatedErr;
   typedef Nucleus::ErrorCode< OSStatus, AAPNotCreatedErr >                        AAPNotCreatedErr;
   typedef Nucleus::ErrorCode< OSStatus, AAPNotFoundErr >                          AAPNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, ASDBadHeaderErr >                         ASDBadHeaderErr;
   typedef Nucleus::ErrorCode< OSStatus, ASDBadForkErr >                           ASDBadForkErr;
   typedef Nucleus::ErrorCode< OSStatus, ASDEntryNotFoundErr >                     ASDEntryNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, fileOffsetTooBigErr >                     FileOffsetTooBigErr;
   typedef Nucleus::ErrorCode< OSStatus, notAllowedToSaveMovieErr >                NotAllowedToSaveMovieErr;
   typedef Nucleus::ErrorCode< OSStatus, qtNetworkAlreadyAllocatedErr >            QTNetworkAlreadyAllocatedErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHHTTPProtocolErr >                 URLDataHHTTPProtocolErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHHTTPNoNetDriverErr >              URLDataHHTTPNoNetDriverErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHHTTPURLErr >                      URLDataHHTTPURLErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHHTTPRedirectErr >                 URLDataHHTTPRedirectErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPProtocolErr >                  URLDataHFTPProtocolErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPShutdownErr >                  URLDataHFTPShutdownErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPBadUserErr >                   URLDataHFTPBadUserErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPBadPasswordErr >               URLDataHFTPBadPasswordErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPServerErr >                    URLDataHFTPServerErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPDataConnectionErr >            URLDataHFTPDataConnectionErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPNoDirectoryErr >               URLDataHFTPNoDirectoryErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPQuotaErr >                     URLDataHFTPQuotaErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPPermissionsErr >               URLDataHFTPPermissionsErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPFilenameErr >                  URLDataHFTPFilenameErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPNoNetDriverErr >               URLDataHFTPNoNetDriverErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPBadNameListErr >               URLDataHFTPBadNameListErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPNeedPasswordErr >              URLDataHFTPNeedPasswordErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPNoPasswordErr >                URLDataHFTPNoPasswordErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPServerDisconnectedErr >        URLDataHFTPServerDisconnectedErr;
   typedef Nucleus::ErrorCode< OSStatus, urlDataHFTPURLErr >                       URLDataHFTPURLErr;
   typedef Nucleus::ErrorCode< OSStatus, notEnoughDataErr >                        NotEnoughDataErr;
   typedef Nucleus::ErrorCode< OSStatus, qtActionNotHandledErr >                   QTActionNotHandledErr;
   typedef Nucleus::ErrorCode< OSStatus, qtXMLParseErr >                           QTXMLParseErr;
   typedef Nucleus::ErrorCode< OSStatus, qtXMLApplicationErr >                     QTXMLApplicationErr;


   typedef Nucleus::ErrorCode< OSStatus, digiUnimpErr >                            DigiUnimpErr;
   typedef Nucleus::ErrorCode< OSStatus, qtParamErr >                              QTParamErr;
   typedef Nucleus::ErrorCode< OSStatus, matrixErr >                               MatrixErr;
   typedef Nucleus::ErrorCode< OSStatus, notExactMatrixErr >                       NotExactMatrixErr;
   typedef Nucleus::ErrorCode< OSStatus, noMoreKeyColorsErr >                      NoMoreKeyColorsErr;
   typedef Nucleus::ErrorCode< OSStatus, notExactSizeErr >                         NotExactSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, badDepthErr >                             BadDepthErr;
   typedef Nucleus::ErrorCode< OSStatus, noDMAErr >                                NoDMAErr;
   typedef Nucleus::ErrorCode< OSStatus, badCallOrderErr >                         BadCallOrderErr;


/*  Kernel Error Codes  */
   typedef Nucleus::ErrorCode< OSStatus, kernelIncompleteErr >                     KernelIncompleteErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelCanceledErr >                       KernelCanceledErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelOptionsErr >                        KernelOptionsErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelPrivilegeErr >                      KernelPrivilegeErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelUnsupportedErr >                    KernelUnsupportedErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelObjectExistsErr >                   KernelObjectExistsErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelWritePermissionErr >                KernelWritePermissionErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelReadPermissionErr >                 KernelReadPermissionErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelExecutePermissionErr >              KernelExecutePermissionErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelDeletePermissionErr >               KernelDeletePermissionErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelExecutionLevelErr >                 KernelExecutionLevelErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelAttributeErr >                      KernelAttributeErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelAsyncSendLimitErr >                 KernelAsyncSendLimitErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelAsyncReceiveLimitErr >              KernelAsyncReceiveLimitErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelTimeoutErr >                        KernelTimeoutErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelInUseErr >                          KernelInUseErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelTerminatedErr >                     KernelTerminatedErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelExceptionErr >                      KernelExceptionErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelIDErr >                             KernelIDErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelAlreadyFreeErr >                    KernelAlreadyFreeErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelReturnValueErr >                    KernelReturnValueErr;
   typedef Nucleus::ErrorCode< OSStatus, kernelUnrecoverableErr >                  KernelUnrecoverableErr;



/* Text Services Mgr error codes */
   typedef Nucleus::ErrorCode< OSStatus, tsmComponentNoErr >                       TSMComponentNoErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmUnsupScriptLanguageErr >               TSMUnsupScriptLanguageErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmInputMethodNotFoundErr >               TSMInputMethodNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmNotAnAppErr >                          TSMNotAnAppErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmAlreadyRegisteredErr >                 TSMAlreadyRegisteredErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmNeverRegisteredErr >                   TSMNeverRegisteredErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmInvalidDocIDErr >                      TSMInvalidDocIDErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmTSMDocBusyErr >                        TSMTSMDocBusyErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmDocNotActiveErr >                      TSMDocNotActiveErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmNoOpenTSErr >                          TSMNoOpenTSErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmCantOpenComponentErr >                 TSMCantOpenComponentErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmTextServiceNotFoundErr >               TSMTextServiceNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmDocumentOpenErr >                      TSMDocumentOpenErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmUseInputWindowErr >                    TSMUseInputWindowErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmTSHasNoMenuErr >                       TSMTSHasNoMenuErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmTSNotOpenErr >                         TSMTSNotOpenErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmComponentAlreadyOpenErr >              TSMComponentAlreadyOpenErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmInputMethodIsOldErr >                  TSMInputMethodIsOldErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmScriptHasNoIMErr >                     TSMScriptHasNoIMErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmUnsupportedTypeErr >                   TSMUnsupportedTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmUnknownErr >                           TSMUnknownErr;
   typedef Nucleus::ErrorCode< OSStatus, tsmInvalidContext >                       TSMInvalidContext;
   typedef Nucleus::ErrorCode< OSStatus, tsmNoHandler >                            TSMNoHandler;
   typedef Nucleus::ErrorCode< OSStatus, tsmNoMoreTokens >                         TSMNoMoreTokens;
   typedef Nucleus::ErrorCode< OSStatus, tsmNoStem >                               TSMNoStem;
   typedef Nucleus::ErrorCode< OSStatus, tsmDefaultIsNotInputMethodErr >           TSMDefaultIsNotInputMethodErr;


/* Mixed Mode error codes */
   typedef Nucleus::ErrorCode< OSStatus, mmInternalError >                         MMInternalError;

/* NameRegistry error codes */
   typedef Nucleus::ErrorCode< OSStatus, nrLockedErr >                             NRLockedErr;
   typedef Nucleus::ErrorCode< OSStatus, nrNotEnoughMemoryErr >                    NRNotEnoughMemoryErr;
   typedef Nucleus::ErrorCode< OSStatus, nrInvalidNodeErr >                        NRInvalidNodeErr;
   typedef Nucleus::ErrorCode< OSStatus, nrNotFoundErr >                           NRNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, nrNotCreatedErr >                         NRNotCreatedErr;
   typedef Nucleus::ErrorCode< OSStatus, nrNameErr >                               NRNameErr;
   typedef Nucleus::ErrorCode< OSStatus, nrNotSlotDeviceErr >                      NRNotSlotDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, nrDataTruncatedErr >                      NRDataTruncatedErr;
   typedef Nucleus::ErrorCode< OSStatus, nrPowerErr >                              NRPowerErr;
   typedef Nucleus::ErrorCode< OSStatus, nrPowerSwitchAbortErr >                   NRPowerSwitchAbortErr;
   typedef Nucleus::ErrorCode< OSStatus, nrTypeMismatchErr >                       NRTypeMismatchErr;
   typedef Nucleus::ErrorCode< OSStatus, nrNotModifiedErr >                        NRNotModifiedErr;
   typedef Nucleus::ErrorCode< OSStatus, nrOverrunErr >                            NROverrunErr;
   typedef Nucleus::ErrorCode< OSStatus, nrResultCodeBase >                        NRResultCodeBase;
   typedef Nucleus::ErrorCode< OSStatus, nrPathNotFound >                          NRPathNotFound;
   typedef Nucleus::ErrorCode< OSStatus, nrPathBufferTooSmall >                    NRPathBufferTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, nrInvalidEntryIterationOp >               NRInvalidEntryIterationOp;
   typedef Nucleus::ErrorCode< OSStatus, nrPropertyAlreadyExists >                 NRPropertyAlreadyExists;
   typedef Nucleus::ErrorCode< OSStatus, nrIterationDone >                         NRIterationDone;
   typedef Nucleus::ErrorCode< OSStatus, nrExitedIteratorScope >                   NRExitedIteratorScope;
   typedef Nucleus::ErrorCode< OSStatus, nrTransactionAborted >                    NRTransactionAborted;
   typedef Nucleus::ErrorCode< OSStatus, nrCallNotSupported >                      NRCallNotSupported;

/* Icon Services error codes */
   typedef Nucleus::ErrorCode< OSStatus, invalidIconRefErr >                       InvalidIconRefErr;
   typedef Nucleus::ErrorCode< OSStatus, noSuchIconErr >                           NoSuchIconErr;
   typedef Nucleus::ErrorCode< OSStatus, noIconDataAvailableErr >                  NoIconDataAvailableErr;


/* Dynamic AppleScript errors */
   typedef Nucleus::ErrorCode< OSStatus, errOSACantCoerce >                        ErrOSACantCoerce;
   typedef Nucleus::ErrorCode< OSStatus, errOSACantAccess >                        ErrOSACantAccess;
   typedef Nucleus::ErrorCode< OSStatus, errOSACantAssign >                        ErrOSACantAssign;
   typedef Nucleus::ErrorCode< OSStatus, errOSAGeneralError >                      ErrOSAGeneralError;
   typedef Nucleus::ErrorCode< OSStatus, errOSADivideByZero >                      ErrOSADivideByZero;
   typedef Nucleus::ErrorCode< OSStatus, errOSANumericOverflow >                   ErrOSANumericOverflow;
   typedef Nucleus::ErrorCode< OSStatus, errOSACantLaunch >                        ErrOSACantLaunch;
   typedef Nucleus::ErrorCode< OSStatus, errOSAAppNotHighLevelEventAware >         ErrOSAAppNotHighLevelEventAware;
   typedef Nucleus::ErrorCode< OSStatus, errOSACorruptTerminology >                ErrOSACorruptTerminology;
   typedef Nucleus::ErrorCode< OSStatus, errOSAStackOverflow >                     ErrOSAStackOverflow;
   typedef Nucleus::ErrorCode< OSStatus, errOSAInternalTableOverflow>              ErrOSAInternalTableOverflow;
   typedef Nucleus::ErrorCode< OSStatus, errOSADataBlockTooLarge >                 ErrOSADataBlockTooLarge;
   typedef Nucleus::ErrorCode< OSStatus, errOSACantGetTerminology >                ErrOSACantGetTerminology;
   typedef Nucleus::ErrorCode< OSStatus, errOSACantCreate >                        ErrOSACantCreate;

/* Static AppleScript errors */
   typedef Nucleus::ErrorCode< OSStatus, errOSATypeError >                         ErrOSATypeError;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAMessageNotUnderstood >               OSAMessageNotUnderstood;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAUndefinedHandler >                   OSAUndefinedHandler;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAIllegalAccess >                      OSAIllegalAccess;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAIllegalIndex >                       OSAIllegalIndex;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAIllegalRange >                       OSAIllegalRange;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAIllegalAssign >                      OSAIllegalAssign;
   typedef Nucleus::ErrorCode< OSStatus, ::OSASyntaxError >                        OSASyntaxError;
   typedef Nucleus::ErrorCode< OSStatus, ::OSASyntaxTypeError >                    OSASyntaxTypeError;
   typedef Nucleus::ErrorCode< OSStatus, ::OSATokenTooLong >                       OSATokenTooLong;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAMissingParameter >                   OSAMissingParameter;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAParameterMismatch >                  OSAParameterMismatch;
   typedef Nucleus::ErrorCode< OSStatus, ::OSADuplicateParameter >                 OSADuplicateParameter;
   typedef Nucleus::ErrorCode< OSStatus, ::OSADuplicateProperty >                  OSADuplicateProperty;
   typedef Nucleus::ErrorCode< OSStatus, ::OSADuplicateHandler >                   OSADuplicateHandler;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAUndefinedVariable >                  OSAUndefinedVariable;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAInconsistentDeclarations>            OSAInconsistentDeclarations;
   typedef Nucleus::ErrorCode< OSStatus, ::OSAControlFlowError >                   OSAControlFlowError;

/* Runtime errors: */
   typedef Nucleus::ErrorCode< OSStatus, errASCantConsiderAndIgnore >              ErrASCantConsiderAndIgnore;
   typedef Nucleus::ErrorCode< OSStatus, errASCantCompareMoreThan32k>              ErrASCantCompareMoreThan32k;
   typedef Nucleus::ErrorCode< OSStatus, errASTerminologyNestingTooDeep >          ErrASTerminologyNestingTooDeep;
   typedef Nucleus::ErrorCode< OSStatus, errASIllegalFormalParameter>              ErrASIllegalFormalParameter;
   typedef Nucleus::ErrorCode< OSStatus, errASParameterNotForEvent >               ErrASParameterNotForEvent;
   typedef Nucleus::ErrorCode< OSStatus, errASNoResultReturned >                   ErrASNoResultReturned;
   typedef Nucleus::ErrorCode< OSStatus, errASInconsistentNames >                  ErrASInconsistentNames;


/* Code Fragment Manager errors */
   typedef Nucleus::ErrorCode< OSStatus, cfragFirstErrCode >                       CFragFirstErrCode;
   typedef Nucleus::ErrorCode< OSStatus, cfragContextIDErr >                       CFragContextIDErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragConnectionIDErr >                    CFragConnectionIDErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoSymbolErr >                        CFragNoSymbolErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoSectionErr >                       CFragNoSectionErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoLibraryErr >                       CFragNoLibraryErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragDupRegistrationErr >                 CFragDupRegistrationErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragFragmentFormatErr >                  CFragFragmentFormatErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragUnresolvedErr >                      CFragUnresolvedErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoPositionErr >                      CFragNoPositionErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoPrivateMemErr >                    CFragNoPrivateMemErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoClientMemErr >                     CFragNoClientMemErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoIDsErr >                           CFragNoIDsErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragInitOrderErr >                       CFragInitOrderErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragImportTooOldErr >                    CFragImportTooOldErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragImportTooNewErr >                    CFragImportTooNewErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragInitLoopErr >                        CFragInitLoopErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragInitAtBootErr >                      CFragInitAtBootErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragLibConnErr >                         CFragLibConnErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragCFMStartupErr >                      CFragCFMStartupErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragCFMInternalErr >                     CFragCFMInternalErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragFragmentCorruptErr >                 CFragFragmentCorruptErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragInitFunctionErr >                    CFragInitFunctionErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoApplicationErr >                   CFragNoApplicationErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragArchitectureErr >                    CFragArchitectureErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragFragmentUsageErr >                   CFragFragmentUsageErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragFileSizeErr >                        CFragFileSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNotClosureErr >                      CFragNotClosureErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragNoRegistrationErr >                  CFragNoRegistrationErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragContainerIDErr >                     CFragContainerIDErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragClosureIDErr >                       CFragClosureIDErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragAbortClosureErr >                    CFragAbortClosureErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragOutputLengthErr >                    CFragOutputLengthErr;
   typedef Nucleus::ErrorCode< OSStatus, cfragLastErrCode >                        CFragLastErrCode;

/*Component Manager & component errors*/
   typedef Nucleus::ErrorCode< OSStatus, invalidComponentID >                      InvalidComponentID;
   typedef Nucleus::ErrorCode< OSStatus, validInstancesExist >                     ValidInstancesExist;
   typedef Nucleus::ErrorCode< OSStatus, componentNotCaptured >                    ComponentNotCaptured;
   typedef Nucleus::ErrorCode< OSStatus, componentDontRegister >                   ComponentDontRegister;
   typedef Nucleus::ErrorCode< OSStatus, unresolvedComponentDLLErr >               UnresolvedComponentDLLErr;
   typedef Nucleus::ErrorCode< OSStatus, retryComponentRegistrationErr >           RetryComponentRegistrationErr;

/*Translation manager & Translation components*/
   typedef Nucleus::ErrorCode< OSStatus, invalidTranslationPathErr >               InvalidTranslationPathErr;
   typedef Nucleus::ErrorCode< OSStatus, couldNotParseSourceFileErr >              CouldNotParseSourceFileErr;
   typedef Nucleus::ErrorCode< OSStatus, noTranslationPathErr >                    NoTranslationPathErr;
   typedef Nucleus::ErrorCode< OSStatus, badTranslationSpecErr >                   BadTranslationSpecErr;
   typedef Nucleus::ErrorCode< OSStatus, noPrefAppErr >                            NoPrefAppErr;

   typedef Nucleus::ErrorCode< OSStatus, buf2SmallErr >                            Buf2SmallErr;
   typedef Nucleus::ErrorCode< OSStatus, noMPPErr >                                NoMPPErr;
   typedef Nucleus::ErrorCode< OSStatus, ckSumErr >                                CKSumErr;
   typedef Nucleus::ErrorCode< OSStatus, extractErr >                              ExtractErr;
   typedef Nucleus::ErrorCode< OSStatus, readQErr >                                ReadQErr;
   typedef Nucleus::ErrorCode< OSStatus, atpLenErr >                               ATPLenErr;
   typedef Nucleus::ErrorCode< OSStatus, atpBadRsp >                               ATPBadRsp;
   typedef Nucleus::ErrorCode< OSStatus, recNotFnd >                               RecNotFnd;
   typedef Nucleus::ErrorCode< OSStatus, sktClosedErr >                            SktClosedErr;


/* OpenTransport errors*/
   typedef Nucleus::ErrorCode< OSStatus, kOTNoError >                              OTNoError;
   typedef Nucleus::ErrorCode< OSStatus, kOTOutOfMemoryErr >                       OTOutOfMemoryErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTNotFoundErr >                          OTNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTDuplicateFoundErr >                    OTDuplicateFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadAddressErr >                        OTBadAddressErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadOptionErr >                         OTBadOptionErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTAccessErr >                            OTAccessErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadReferenceErr >                      OTBadReferenceErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTNoAddressErr >                         OTNoAddressErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTOutStateErr >                          OTOutStateErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadSequenceErr >                       OTBadSequenceErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTSysErrorErr >                          OTSysErrorErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTLookErr >                              OTLookErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadDataErr >                           OTBadDataErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBufferOverflowErr >                    OTBufferOverflowErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTFlowErr >                              OTFlowErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTNoDataErr >                            OTNoDataErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTNoDisconnectErr >                      OTNoDisconnectErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTNoUDErrErr >                           OTNoUDErrErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadFlagErr >                           OTBadFlagErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTNoReleaseErr >                         OTNoReleaseErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTNotSupportedErr >                      OTNotSupportedErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTStateChangeErr >                       OTStateChangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTNoStructureTypeErr >                   OTNoStructureTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadNameErr >                           OTBadNameErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadQLenErr >                           OTBadQLenErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTAddressBusyErr >                       OTAddressBusyErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTIndOutErr >                            OTIndOutErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTProviderMismatchErr >                  OTProviderMismatchErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTResQLenErr >                           OTResQLenErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTResAddressErr >                        OTResAddressErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTQFullErr >                             OTQFullErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTProtocolErr >                          OTProtocolErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadSyncErr >                           OTBadSyncErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTCanceledErr >                          OTCanceledErr;
   typedef Nucleus::ErrorCode< OSStatus, kEPERMErr >                               EPERMErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOENTErr >                              ENOENTErr;
   typedef Nucleus::ErrorCode< OSStatus, kENORSRCErr >                             ENORSRCErr;
   typedef Nucleus::ErrorCode< OSStatus, kEINTRErr >                               EINTRErr;
   typedef Nucleus::ErrorCode< OSStatus, kEIOErr >                                 EIOErr;
   typedef Nucleus::ErrorCode< OSStatus, kENXIOErr >                               ENXIOErr;
   typedef Nucleus::ErrorCode< OSStatus, kEBADFErr >                               EBADFErr;
   typedef Nucleus::ErrorCode< OSStatus, kEAGAINErr >                              EAGAINErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOMEMErr >                              ENOMEMErr;
   typedef Nucleus::ErrorCode< OSStatus, kEACCESErr >                              EACCESErr;
   typedef Nucleus::ErrorCode< OSStatus, kEFAULTErr >                              EFAULTErr;
   typedef Nucleus::ErrorCode< OSStatus, kEBUSYErr >                               EBUSYErr;
   typedef Nucleus::ErrorCode< OSStatus, kEEXISTErr >                              EEXISTErr;
   typedef Nucleus::ErrorCode< OSStatus, kENODEVErr >                              ENODEVErr;
   typedef Nucleus::ErrorCode< OSStatus, kEINVALErr >                              EINVALErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOTTYErr >                              ENOTTYErr;
   typedef Nucleus::ErrorCode< OSStatus, kEPIPEErr >                               EPIPEErr;
   typedef Nucleus::ErrorCode< OSStatus, kERANGEErr >                              ERANGEErr;
   typedef Nucleus::ErrorCode< OSStatus, kEWOULDBLOCKErr >                         EWOULDBLOCKErr;
   typedef Nucleus::ErrorCode< OSStatus, kEDEADLKErr >                             EDEADLKErr;
   typedef Nucleus::ErrorCode< OSStatus, kEALREADYErr >                            EALREADYErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOTSOCKErr >                            ENOTSOCKErr;
   typedef Nucleus::ErrorCode< OSStatus, kEDESTADDRREQErr >                        EDESTADDRREQErr;
   typedef Nucleus::ErrorCode< OSStatus, kEMSGSIZEErr >                            EMSGSIZEErr;
   typedef Nucleus::ErrorCode< OSStatus, kEPROTOTYPEErr >                          EPROTOTYPEErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOPROTOOPTErr >                         ENOPROTOOPTErr;
   typedef Nucleus::ErrorCode< OSStatus, kEPROTONOSUPPORTErr >                     EPROTONOSUPPORTErr;
   typedef Nucleus::ErrorCode< OSStatus, kESOCKTNOSUPPORTErr >                     ESOCKTNOSUPPORTErr;
   typedef Nucleus::ErrorCode< OSStatus, kEOPNOTSUPPErr >                          EOPNOTSUPPErr;
   typedef Nucleus::ErrorCode< OSStatus, kEADDRINUSEErr >                          EADDRINUSEErr;
   typedef Nucleus::ErrorCode< OSStatus, kEADDRNOTAVAILErr >                       EADDRNOTAVAILErr;
   typedef Nucleus::ErrorCode< OSStatus, kENETDOWNErr >                            ENETDOWNErr;
   typedef Nucleus::ErrorCode< OSStatus, kENETUNREACHErr >                         ENETUNREACHErr;
   typedef Nucleus::ErrorCode< OSStatus, kENETRESETErr >                           ENETRESETErr;
   typedef Nucleus::ErrorCode< OSStatus, kECONNABORTEDErr >                        ECONNABORTEDErr;
   typedef Nucleus::ErrorCode< OSStatus, kECONNRESETErr >                          ECONNRESETErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOBUFSErr >                             ENOBUFSErr;
   typedef Nucleus::ErrorCode< OSStatus, kEISCONNErr >                             EISCONNErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOTCONNErr >                            ENOTCONNErr;
   typedef Nucleus::ErrorCode< OSStatus, kESHUTDOWNErr >                           ESHUTDOWNErr;
   typedef Nucleus::ErrorCode< OSStatus, kETOOMANYREFSErr >                        ETOOMANYREFSErr;
   typedef Nucleus::ErrorCode< OSStatus, kETIMEDOUTErr >                           ETIMEDOUTErr;
   typedef Nucleus::ErrorCode< OSStatus, kECONNREFUSEDErr >                        ECONNREFUSEDErr;
   typedef Nucleus::ErrorCode< OSStatus, kEHOSTDOWNErr >                           EHOSTDOWNErr;
   typedef Nucleus::ErrorCode< OSStatus, kEHOSTUNREACHErr >                        EHOSTUNREACHErr;
   typedef Nucleus::ErrorCode< OSStatus, kEPROTOErr >                              EPROTOErr;
   typedef Nucleus::ErrorCode< OSStatus, kETIMEErr >                               ETIMEErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOSRErr >                               ENOSRErr;
   typedef Nucleus::ErrorCode< OSStatus, kEBADMSGErr >                             EBADMSGErr;
   typedef Nucleus::ErrorCode< OSStatus, kECANCELErr >                             ECANCELErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOSTRErr >                              ENOSTRErr;
   typedef Nucleus::ErrorCode< OSStatus, kENODATAErr >                             ENODATAErr;
   typedef Nucleus::ErrorCode< OSStatus, kEINPROGRESSErr >                         EINPROGRESSErr;
   typedef Nucleus::ErrorCode< OSStatus, kESRCHErr >                               ESRCHErr;
   typedef Nucleus::ErrorCode< OSStatus, kENOMSGErr >                              ENOMSGErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTClientNotInittedErr >                  OTClientNotInittedErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTPortHasDiedErr >                       OTPortHasDiedErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTPortWasEjectedErr >                    OTPortWasEjectedErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTBadConfigurationErr >                  OTBadConfigurationErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTConfigurationChangedErr >              OTConfigurationChangedErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTUserRequestedErr >                     OTUserRequestedErr;
   typedef Nucleus::ErrorCode< OSStatus, kOTPortLostConnection >                   OTPortLostConnection;


/* Color Picker errors*/
   typedef Nucleus::ErrorCode< OSStatus, firstPickerError >                        FirstPickerError;
   typedef Nucleus::ErrorCode< OSStatus, invalidPickerType >                       InvalidPickerType;
   typedef Nucleus::ErrorCode< OSStatus, requiredFlagsDontMatch >                  RequiredFlagsDontMatch;
   typedef Nucleus::ErrorCode< OSStatus, pickerResourceError >                     PickerResourceError;
   typedef Nucleus::ErrorCode< OSStatus, cantLoadPicker >                          CantLoadPicker;
   typedef Nucleus::ErrorCode< OSStatus, cantCreatePickerWindow >                  CantCreatePickerWindow;
   typedef Nucleus::ErrorCode< OSStatus, cantLoadPackage >                         CantLoadPackage;
   typedef Nucleus::ErrorCode< OSStatus, pickerCantLive >                          PickerCantLive;
   typedef Nucleus::ErrorCode< OSStatus, colorSyncNotInstalled >                   ColorSyncNotInstalled;
   typedef Nucleus::ErrorCode< OSStatus, badProfileError >                         BadProfileError;
   typedef Nucleus::ErrorCode< OSStatus, noHelpForItem >                           NoHelpForItem;



/* NSL error codes*/
   typedef Nucleus::ErrorCode< OSStatus, kNSL68kContextNotSupported >              NSL68kContextNotSupported;
   typedef Nucleus::ErrorCode< OSStatus, kNSLSchedulerError >                      NSLSchedulerError;
   typedef Nucleus::ErrorCode< OSStatus, kNSLBadURLSyntax >                        NSLBadURLSyntax;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNoCarbonLib >                         NSLNoCarbonLib;
   typedef Nucleus::ErrorCode< OSStatus, kNSLUILibraryNotAvailable >               NSLUILibraryNotAvailable;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNotImplementedYet >                   NSLNotImplementedYet;
   typedef Nucleus::ErrorCode< OSStatus, kNSLErrNullPtrError >                     NSLErrNullPtrError;
   typedef Nucleus::ErrorCode< OSStatus, kNSLSomePluginsFailedToLoad>              NSLSomePluginsFailedToLoad;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNullNeighborhoodPtr >                 NSLNullNeighborhoodPtr;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNoPluginsForSearch >                  NSLNoPluginsForSearch;
   typedef Nucleus::ErrorCode< OSStatus, kNSLSearchAlreadyInProgress>              NSLSearchAlreadyInProgress;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNoPluginsFound >                      NSLNoPluginsFound;
   typedef Nucleus::ErrorCode< OSStatus, kNSLPluginLoadFailed >                    NSLPluginLoadFailed;
   typedef Nucleus::ErrorCode< OSStatus, kNSLBadProtocolTypeErr >                  NSLBadProtocolTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNullListPtr >                         NSLNullListPtr;
   typedef Nucleus::ErrorCode< OSStatus, kNSLBadClientInfoPtr >                    NSLBadClientInfoPtr;
   typedef Nucleus::ErrorCode< OSStatus, kNSLCannotContinueLookup >                NSLCannotContinueLookup;
   typedef Nucleus::ErrorCode< OSStatus, kNSLBufferTooSmallForData >               NSLBufferTooSmallForData;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNoContextAvailable >                  NSLNoContextAvailable;
   typedef Nucleus::ErrorCode< OSStatus, kNSLRequestBufferAlreadyInList >          NSLRequestBufferAlreadyInList;
   typedef Nucleus::ErrorCode< OSStatus, kNSLInvalidPluginSpec >                   NSLInvalidPluginSpec;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNoSupportForService >                 NSLNoSupportForService;
   typedef Nucleus::ErrorCode< OSStatus, kNSLBadNetConnection >                    NSLBadNetConnection;
   typedef Nucleus::ErrorCode< OSStatus, kNSLBadDataTypeErr >                      NSLBadDataTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSLBadServiceTypeErr >                   NSLBadServiceTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSLBadReferenceErr >                     NSLBadReferenceErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNoElementsInList >                    NSLNoElementsInList;
   typedef Nucleus::ErrorCode< OSStatus, kNSLInsufficientOTVer >                   NSLInsufficientOTVer;
   typedef Nucleus::ErrorCode< OSStatus, kNSLInsufficientSysVer >                  NSLInsufficientSysVer;
   typedef Nucleus::ErrorCode< OSStatus, kNSLNotInitialized >                      NSLNotInitialized;
   typedef Nucleus::ErrorCode< OSStatus, kNSLInitializationFailed >                NSLInitializationFailed;



/* desktop printing error codes*/
   typedef Nucleus::ErrorCode< OSStatus, kDTPHoldJobErr >                          DTPHoldJobErr;
   typedef Nucleus::ErrorCode< OSStatus, kDTPStopQueueErr >                        DTPStopQueueErr;
   typedef Nucleus::ErrorCode< OSStatus, kDTPTryAgainErr >                         DTPTryAgainErr;
   typedef Nucleus::ErrorCode< OSStatus, kDTPAbortJobErr >                         DTPAbortJobErr;


/* ColorSync Result codes */
/* Profile Access Errors */
   typedef Nucleus::ErrorCode< OSStatus, cmElementTagNotFound >                    CMElementTagNotFound;
   typedef Nucleus::ErrorCode< OSStatus, cmIndexRangeErr >                         CMIndexRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, cmCantDeleteElement >                     CMCantDeleteElement;
   typedef Nucleus::ErrorCode< OSStatus, cmFatalProfileErr >                       CMFatalProfileErr;
   typedef Nucleus::ErrorCode< OSStatus, cmInvalidProfile >                        CMInvalidProfile;
   typedef Nucleus::ErrorCode< OSStatus, cmInvalidProfileLocation >                CMInvalidProfileLocation;
   typedef Nucleus::ErrorCode< OSStatus, cmCantCopyModifiedV1Profile>              CMCantCopyModifiedV1Profile;
/* Profile Search Errors */
   typedef Nucleus::ErrorCode< OSStatus, cmInvalidSearch >                         CMInvalidSearch;
   typedef Nucleus::ErrorCode< OSStatus, cmSearchError >                           CMSearchError;
   typedef Nucleus::ErrorCode< OSStatus, cmErrIncompatibleProfile >                CMErrIncompatibleProfile;
   typedef Nucleus::ErrorCode< OSStatus, cmInvalidColorSpace >                     CMInvalidColorSpace;
   typedef Nucleus::ErrorCode< OSStatus, cmInvalidSrcMap >                         CMInvalidSrcMap;
   typedef Nucleus::ErrorCode< OSStatus, cmInvalidDstMap >                         CMInvalidDstMap;
   typedef Nucleus::ErrorCode< OSStatus, cmNoGDevicesError >                       CMNoGDevicesError;
   typedef Nucleus::ErrorCode< OSStatus, cmInvalidProfileComment >                 CMInvalidProfileComment;
   typedef Nucleus::ErrorCode< OSStatus, cmRangeOverFlow >                         CMRangeOverFlow;
   typedef Nucleus::ErrorCode< OSStatus, cmNamedColorNotFound >                    CMNamedColorNotFound;
   typedef Nucleus::ErrorCode< OSStatus, cmCantGamutCheckError >                   CMCantGamutCheckError;

/* new Folder Manager error codes */
   typedef Nucleus::ErrorCode< OSStatus, badFolderDescErr >                        BadFolderDescErr;
   typedef Nucleus::ErrorCode< OSStatus, duplicateFolderDescErr >                  DuplicateFolderDescErr;
   typedef Nucleus::ErrorCode< OSStatus, noMoreFolderDescErr >                     NoMoreFolderDescErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidFolderTypeErr >                    InvalidFolderTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, duplicateRoutingErr >                     DuplicateRoutingErr;
   typedef Nucleus::ErrorCode< OSStatus, routingNotFoundErr >                      RoutingNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, badRoutingSizeErr >                       BadRoutingSizeErr;


/* Core Foundation errors*/
   typedef Nucleus::ErrorCode< OSStatus, coreFoundationUnknownErr >                CoreFoundationUnknownErr;

/* ScrapMgr error codes (CarbonLib 1.0 and later)*/
   typedef Nucleus::ErrorCode< OSStatus, internalScrapErr >                        InternalScrapErr;
   typedef Nucleus::ErrorCode< OSStatus, duplicateScrapFlavorErr >                 DuplicateScrapFlavorErr;
   typedef Nucleus::ErrorCode< OSStatus, badScrapRefErr >                          BadScrapRefErr;
   typedef Nucleus::ErrorCode< OSStatus, processStateIncorrectErr >                ProcessStateIncorrectErr;
   typedef Nucleus::ErrorCode< OSStatus, scrapPromiseNotKeptErr >                  ScrapPromiseNotKeptErr;
   typedef Nucleus::ErrorCode< OSStatus, noScrapPromiseKeeperErr >                 NoScrapPromiseKeeperErr;
   typedef Nucleus::ErrorCode< OSStatus, nilScrapFlavorDataErr >                   NilScrapFlavorDataErr;
   typedef Nucleus::ErrorCode< OSStatus, scrapFlavorFlagsMismatchErr>              ScrapFlavorFlagsMismatchErr;
   typedef Nucleus::ErrorCode< OSStatus, scrapFlavorSizeMismatchErr >              ScrapFlavorSizeMismatchErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalScrapFlavorFlagsErr >              IllegalScrapFlavorFlagsErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalScrapFlavorTypeErr >               IllegalScrapFlavorTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, illegalScrapFlavorSizeErr >               IllegalScrapFlavorSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, scrapFlavorNotFoundErr >                  ScrapFlavorNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, needClearScrapErr >                       NeedClearScrapErr;


/*  AFP Protocol Errors */
   typedef Nucleus::ErrorCode< OSStatus, afpAccessDenied >                         AFPAccessDenied;
   typedef Nucleus::ErrorCode< OSStatus, afpAuthContinue >                         AFPAuthContinue;
   typedef Nucleus::ErrorCode< OSStatus, afpBadUAM >                               AFPBadUAM;
   typedef Nucleus::ErrorCode< OSStatus, afpBadVersNum >                           AFPBadVersNum;
   typedef Nucleus::ErrorCode< OSStatus, afpBitmapErr >                            AFPBitmapErr;
   typedef Nucleus::ErrorCode< OSStatus, afpCantMove >                             AFPCantMove;
   typedef Nucleus::ErrorCode< OSStatus, afpDenyConflict >                         AFPDenyConflict;
   typedef Nucleus::ErrorCode< OSStatus, afpDirNotEmpty >                          AFPDirNotEmpty;
   typedef Nucleus::ErrorCode< OSStatus, afpDiskFull >                             AFPDiskFull;
   typedef Nucleus::ErrorCode< OSStatus, afpEofError >                             AFPEofError;
   typedef Nucleus::ErrorCode< OSStatus, afpFileBusy >                             AFPFileBusy;
   typedef Nucleus::ErrorCode< OSStatus, afpFlatVol >                              AFPFlatVol;
   typedef Nucleus::ErrorCode< OSStatus, afpItemNotFound >                         AFPItemNotFound;
   typedef Nucleus::ErrorCode< OSStatus, afpLockErr >                              AFPLockErr;
   typedef Nucleus::ErrorCode< OSStatus, afpMiscErr >                              AFPMiscErr;
   typedef Nucleus::ErrorCode< OSStatus, afpNoMoreLocks >                          AFPNoMoreLocks;
   typedef Nucleus::ErrorCode< OSStatus, afpNoServer >                             AFPNoServer;
   typedef Nucleus::ErrorCode< OSStatus, afpObjectExists >                         AFPObjectExists;
   typedef Nucleus::ErrorCode< OSStatus, afpObjectNotFound >                       AFPObjectNotFound;
   typedef Nucleus::ErrorCode< OSStatus, afpParmErr >                              AFPParmErr;
   typedef Nucleus::ErrorCode< OSStatus, afpRangeNotLocked >                       AFPRangeNotLocked;
   typedef Nucleus::ErrorCode< OSStatus, afpRangeOverlap >                         AFPRangeOverlap;
   typedef Nucleus::ErrorCode< OSStatus, afpSessClosed >                           AFPSessClosed;
   typedef Nucleus::ErrorCode< OSStatus, afpUserNotAuth >                          AFPUserNotAuth;
   typedef Nucleus::ErrorCode< OSStatus, afpCallNotSupported >                     AFPCallNotSupported;
   typedef Nucleus::ErrorCode< OSStatus, afpObjectTypeErr >                        AFPObjectTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, afpTooManyFilesOpen >                     AFPTooManyFilesOpen;
   typedef Nucleus::ErrorCode< OSStatus, afpServerGoingDown >                      AFPServerGoingDown;
   typedef Nucleus::ErrorCode< OSStatus, afpCantRename >                           AFPCantRename;
   typedef Nucleus::ErrorCode< OSStatus, afpDirNotFound >                          AFPDirNotFound;
   typedef Nucleus::ErrorCode< OSStatus, afpIconTypeError >                        AFPIconTypeError;
   typedef Nucleus::ErrorCode< OSStatus, afpVolLocked >                            AFPVolLocked;
   typedef Nucleus::ErrorCode< OSStatus, afpObjectLocked >                         AFPObjectLocked;
   typedef Nucleus::ErrorCode< OSStatus, afpContainsSharedErr >                    AFPContainsSharedErr;
   typedef Nucleus::ErrorCode< OSStatus, afpIDNotFound >                           AFPIDNotFound;
   typedef Nucleus::ErrorCode< OSStatus, afpIDExists >                             AFPIDExists;
   typedef Nucleus::ErrorCode< OSStatus, afpDiffVolErr >                           AFPDiffVolErr;
   typedef Nucleus::ErrorCode< OSStatus, afpCatalogChanged >                       AFPCatalogChanged;
   typedef Nucleus::ErrorCode< OSStatus, afpSameObjectErr >                        AFPSameObjectErr;
   typedef Nucleus::ErrorCode< OSStatus, afpBadIDErr >                             AFPBadIDErr;
   typedef Nucleus::ErrorCode< OSStatus, afpPwdSameErr >                           AFPPwdSameErr;
   typedef Nucleus::ErrorCode< OSStatus, afpPwdTooShortErr >                       AFPPwdTooShortErr;
   typedef Nucleus::ErrorCode< OSStatus, afpPwdExpiredErr >                        AFPPwdExpiredErr;
   typedef Nucleus::ErrorCode< OSStatus, afpInsideSharedErr >                      AFPInsideSharedErr;
   typedef Nucleus::ErrorCode< OSStatus, afpInsideTrashErr >                       AFPInsideTrashErr;
   typedef Nucleus::ErrorCode< OSStatus, afpPwdNeedsChangeErr >                    AFPPwdNeedsChangeErr;
   typedef Nucleus::ErrorCode< OSStatus, afpPwdPolicyErr >                         AFPPwdPolicyErr;
   typedef Nucleus::ErrorCode< OSStatus, afpAlreadyLoggedInErr >                   AFPAlreadyLoggedInErr;
   typedef Nucleus::ErrorCode< OSStatus, afpCallNotAllowed >                       AFPCallNotAllowed;

/*  AppleShare Client Errors */
   typedef Nucleus::ErrorCode< OSStatus, afpBadDirIDType >                         AFPBadDirIDType;
   typedef Nucleus::ErrorCode< OSStatus, afpCantMountMoreSrvre >                   AFPCantMountMoreSrvre;
   typedef Nucleus::ErrorCode< OSStatus, afpAlreadyMounted >                       AFPAlreadyMounted;
   typedef Nucleus::ErrorCode< OSStatus, afpSameNodeErr >                          AFPSameNodeErr;

/*Text Engines, TSystemTextEngines, HIEditText error coded*/
   typedef Nucleus::ErrorCode< OSStatus, errUnknownAttributeTag >                  ErrUnknownAttributeTag;
   typedef Nucleus::ErrorCode< OSStatus, errMarginWilllNotFit >                    ErrMarginWilllNotFit;
   typedef Nucleus::ErrorCode< OSStatus, errNotInImagingMode >                     ErrNotInImagingMode;
   typedef Nucleus::ErrorCode< OSStatus, errAlreadyInImagingMode >                 ErrAlreadyInImagingMode;
   typedef Nucleus::ErrorCode< OSStatus, errEngineNotFound >                       ErrEngineNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errIteratorReachedEnd >                   ErrIteratorReachedEnd;
   typedef Nucleus::ErrorCode< OSStatus, errInvalidRange >                         ErrInvalidRange;
   typedef Nucleus::ErrorCode< OSStatus, errOffsetNotOnElementBounday >            ErrOffsetNotOnElementBounday;
   typedef Nucleus::ErrorCode< OSStatus, errNoHiliteText >                         ErrNoHiliteText;
   typedef Nucleus::ErrorCode< OSStatus, errEmptyScrap >                           ErrEmptyScrap;
   typedef Nucleus::ErrorCode< OSStatus, errReadOnlyText >                         ErrReadOnlyText;
   typedef Nucleus::ErrorCode< OSStatus, errUnknownElement >                       ErrUnknownElement;
   typedef Nucleus::ErrorCode< OSStatus, errNonContiuousAttribute >                ErrNonContiuousAttribute;
   typedef Nucleus::ErrorCode< OSStatus, errCannotUndo >                           ErrCannotUndo;

/* HTMLRendering OSStaus codes*/
   typedef Nucleus::ErrorCode< OSStatus, hrHTMLRenderingLibNotInstalledErr >       HRHTMLRenderingLibNotInstalledErr;
   typedef Nucleus::ErrorCode< OSStatus, hrMiscellaneousExceptionErr>              HRMiscellaneousExceptionErr;
   typedef Nucleus::ErrorCode< OSStatus, hrUnableToResizeHandleErr >               HRUnableToResizeHandleErr;
   typedef Nucleus::ErrorCode< OSStatus, hrURLNotHandledErr >                      HRURLNotHandledErr;

/* IAExtractor result codes */
   typedef Nucleus::ErrorCode< OSStatus, errIANoErr >                              ErrIANoErr;
   typedef Nucleus::ErrorCode< OSStatus, errIAUnknownErr >                         ErrIAUnknownErr;
   typedef Nucleus::ErrorCode< OSStatus, errIAAllocationErr >                      ErrIAAllocationErr;
   typedef Nucleus::ErrorCode< OSStatus, errIAParamErr >                           ErrIAParamErr;
   typedef Nucleus::ErrorCode< OSStatus, errIANoMoreItems >                        ErrIANoMoreItems;
   typedef Nucleus::ErrorCode< OSStatus, errIABufferTooSmall >                     ErrIABufferTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, errIACanceled >                           ErrIACanceled;
   typedef Nucleus::ErrorCode< OSStatus, errIAInvalidDocument >                    ErrIAInvalidDocument;
   typedef Nucleus::ErrorCode< OSStatus, errIATextExtractionErr >                  ErrIATextExtractionErr;
   typedef Nucleus::ErrorCode< OSStatus, errIAEndOfTextRun >                       ErrIAEndOfTextRun;

/* QuickTime Streaming Errors */
   typedef Nucleus::ErrorCode< OSStatus, qtsBadSelectorErr >                       QTSBadSelectorErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsBadStateErr >                          QTSBadStateErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsBadDataErr >                           QTSBadDataErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsUnsupportedDataTypeErr >               QTSUnsupportedDataTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsUnsupportedRateErr >                   QTSUnsupportedRateErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsUnsupportedFeatureErr >                QTSUnsupportedFeatureErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsTooMuchDataErr >                       QTSTooMuchDataErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsUnknownValueErr >                      QTSUnknownValueErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsTimeoutErr >                           QTSTimeoutErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsConnectionFailedErr >                  QTSConnectionFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, qtsAddressBusyErr >                       QTSAddressBusyErr;

/*Gestalt error codes*/
   typedef Nucleus::ErrorCode< OSStatus, gestaltUnknownErr >                       GestaltUnknownErr;
   typedef Nucleus::ErrorCode< OSStatus, gestaltUndefSelectorErr >                 GestaltUndefSelectorErr;
   typedef Nucleus::ErrorCode< OSStatus, gestaltDupSelectorErr >                   GestaltDupSelectorErr;
   typedef Nucleus::ErrorCode< OSStatus, gestaltLocationErr >                      GestaltLocationErr;


/* Menu Manager error codes*/
   typedef Nucleus::ErrorCode< OSStatus, menuPropertyInvalidErr >                  MenuPropertyInvalidErr;
   typedef Nucleus::ErrorCode< OSStatus, menuPropertyInvalid >                     MenuPropertyInvalid;
   typedef Nucleus::ErrorCode< OSStatus, menuPropertyNotFoundErr >                 MenuPropertyNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, menuNotFoundErr >                         MenuNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, menuUsesSystemDefErr >                    MenuUsesSystemDefErr;
   typedef Nucleus::ErrorCode< OSStatus, menuItemNotFoundErr >                     MenuItemNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, menuInvalidErr >                          MenuInvalidErr;


/* Window Manager error codes*/
   typedef Nucleus::ErrorCode< OSStatus, errInvalidWindowPtr >                     ErrInvalidWindowPtr;
   typedef Nucleus::ErrorCode< OSStatus, errInvalidWindowRef >                     ErrInvalidWindowRef;
   typedef Nucleus::ErrorCode< OSStatus, errUnsupportedWindowAttributesForClass >  ErrUnsupportedWindowAttributesForClass;
   typedef Nucleus::ErrorCode< OSStatus, errWindowDoesNotHaveProxy >               ErrWindowDoesNotHaveProxy;
   typedef Nucleus::ErrorCode< OSStatus, errInvalidWindowProperty >                ErrInvalidWindowProperty;
   typedef Nucleus::ErrorCode< OSStatus, errWindowPropertyNotFound >               ErrWindowPropertyNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errUnrecognizedWindowClass >              ErrUnrecognizedWindowClass;
   typedef Nucleus::ErrorCode< OSStatus, errCorruptWindowDescription>              ErrCorruptWindowDescription;
   typedef Nucleus::ErrorCode< OSStatus, errUserWantsToDragWindow >                ErrUserWantsToDragWindow;
   typedef Nucleus::ErrorCode< OSStatus, errWindowsAlreadyInitialized >            ErrWindowsAlreadyInitialized;
   typedef Nucleus::ErrorCode< OSStatus, errFloatingWindowsNotInitialized >        ErrFloatingWindowsNotInitialized;
   typedef Nucleus::ErrorCode< OSStatus, errWindowNotFound >                       ErrWindowNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errWindowDoesNotFitOnscreen>              ErrWindowDoesNotFitOnscreen;
   typedef Nucleus::ErrorCode< OSStatus, windowAttributeImmutableErr>              WindowAttributeImmutableErr;
   typedef Nucleus::ErrorCode< OSStatus, windowAttributesConflictErr>              WindowAttributesConflictErr;
   typedef Nucleus::ErrorCode< OSStatus, windowManagerInternalErr >                WindowManagerInternalErr;
   typedef Nucleus::ErrorCode< OSStatus, windowWrongStateErr >                     WindowWrongStateErr;
   typedef Nucleus::ErrorCode< OSStatus, windowGroupInvalidErr >                   WindowGroupInvalidErr;
   typedef Nucleus::ErrorCode< OSStatus, windowAppModalStateAlreadyExistsErr >     WindowAppModalStateAlreadyExistsErr;
   typedef Nucleus::ErrorCode< OSStatus, windowNoAppModalStateErr >                WindowNoAppModalStateErr;
   typedef Nucleus::ErrorCode< OSStatus, errWindowDoesntSupportFocus>              ErrWindowDoesntSupportFocus;
   typedef Nucleus::ErrorCode< OSStatus, errWindowRegionCodeInvalid >              ErrWindowRegionCodeInvalid;

/* Dialog Mgr error codes*/
   typedef Nucleus::ErrorCode< OSStatus, dialogNoTimeoutErr >                      DialogNoTimeoutErr;

/* NavigationLib error codes*/
   typedef Nucleus::ErrorCode< OSStatus, kNavWrongDialogStateErr >                 NavWrongDialogStateErr;
   typedef Nucleus::ErrorCode< OSStatus, kNavWrongDialogClassErr >                 NavWrongDialogClassErr;
   typedef Nucleus::ErrorCode< OSStatus, kNavInvalidSystemConfigErr >              NavInvalidSystemConfigErr;
   typedef Nucleus::ErrorCode< OSStatus, kNavCustomControlMessageFailedErr >       NavCustomControlMessageFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNavInvalidCustomControlMessageErr >      NavInvalidCustomControlMessageErr;
   typedef Nucleus::ErrorCode< OSStatus, kNavMissingKindStringErr >                NavMissingKindStringErr;


/* Collection Manager errors */
   typedef Nucleus::ErrorCode< OSStatus, collectionItemLockedErr >                 CollectionItemLockedErr;
   typedef Nucleus::ErrorCode< OSStatus, collectionItemNotFoundErr >               CollectionItemNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, collectionIndexRangeErr >                 CollectionIndexRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, collectionVersionErr >                    CollectionVersionErr;


/* QuickTime Streaming Server Errors */
   typedef Nucleus::ErrorCode< OSStatus, kQTSSUnknownErr >                         QTSSUnknownErr;


/* Display Manager error codes (-6220...-6269)*/
   typedef Nucleus::ErrorCode< OSStatus, kDMGenErr >                               DMGenErr;
   typedef Nucleus::ErrorCode< OSStatus, kDMMirroringOnAlready >                   DMMirroringOnAlready;
   typedef Nucleus::ErrorCode< OSStatus, kDMWrongNumberOfDisplays >                DMWrongNumberOfDisplays;
   typedef Nucleus::ErrorCode< OSStatus, kDMMirroringBlocked >                     DMMirroringBlocked;
   typedef Nucleus::ErrorCode< OSStatus, kDMCantBlock >                            DMCantBlock;
   typedef Nucleus::ErrorCode< OSStatus, kDMMirroringNotOn >                       DMMirroringNotOn;
   typedef Nucleus::ErrorCode< OSStatus, kSysSWTooOld >                            SysSWTooOld;
   typedef Nucleus::ErrorCode< OSStatus, kDMSWNotInitializedErr >                  DMSWNotInitializedErr;
   typedef Nucleus::ErrorCode< OSStatus, kDMDriverNotDisplayMgrAwareErr >          DMDriverNotDisplayMgrAwareErr;
   typedef Nucleus::ErrorCode< OSStatus, kDMDisplayNotFoundErr >                   DMDisplayNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kDMNotFoundErr >                          DMNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kDMDisplayAlreadyInstalledErr >           DMDisplayAlreadyInstalledErr;
   typedef Nucleus::ErrorCode< OSStatus, kDMMainDisplayCannotMoveErr>              DMMainDisplayCannotMoveErr;
   typedef Nucleus::ErrorCode< OSStatus, kDMNoDeviceTableclothErr >                DMNoDeviceTableclothErr;
   typedef Nucleus::ErrorCode< OSStatus, kDMFoundErr >                             DMFoundErr;


/* Language Analysis error codes */
   typedef Nucleus::ErrorCode< OSStatus, laTooSmallBufferErr >                     LATooSmallBufferErr;
   typedef Nucleus::ErrorCode< OSStatus, laEnvironmentBusyErr >                    LAEnvironmentBusyErr;
   typedef Nucleus::ErrorCode< OSStatus, laEnvironmentNotFoundErr >                LAEnvironmentNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, laEnvironmentExistErr >                   LAEnvironmentExistErr;
   typedef Nucleus::ErrorCode< OSStatus, laInvalidPathErr >                        LAInvalidPathErr;
   typedef Nucleus::ErrorCode< OSStatus, laNoMoreMorphemeErr >                     LANoMoreMorphemeErr;
   typedef Nucleus::ErrorCode< OSStatus, laFailAnalysisErr >                       LAFailAnalysisErr;
   typedef Nucleus::ErrorCode< OSStatus, laTextOverFlowErr >                       LATextOverFlowErr;
   typedef Nucleus::ErrorCode< OSStatus, laDictionaryNotOpenedErr >                LADictionaryNotOpenedErr;
   typedef Nucleus::ErrorCode< OSStatus, laDictionaryUnknownErr >                  LADictionaryUnknownErr;
   typedef Nucleus::ErrorCode< OSStatus, laDictionaryTooManyErr >                  LADictionaryTooManyErr;
   typedef Nucleus::ErrorCode< OSStatus, laPropertyValueErr >                      LAPropertyValueErr;
   typedef Nucleus::ErrorCode< OSStatus, laPropertyUnknownErr >                    LAPropertyUnknownErr;
   typedef Nucleus::ErrorCode< OSStatus, laPropertyIsReadOnlyErr >                 LAPropertyIsReadOnlyErr;
   typedef Nucleus::ErrorCode< OSStatus, laPropertyNotFoundErr >                   LAPropertyNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, laPropertyErr >                           LAPropertyErr;
   typedef Nucleus::ErrorCode< OSStatus, laEngineNotFoundErr >                     LAEngineNotFoundErr;


   typedef Nucleus::ErrorCode< OSStatus, kUSBNoErr >                               USBNoErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBNoTran >                              USBNoTran;
   typedef Nucleus::ErrorCode< OSStatus, kUSBNoDelay >                             USBNoDelay;
   typedef Nucleus::ErrorCode< OSStatus, kUSBPending >                             USBPending;

/* USB Hardware Errors */
   typedef Nucleus::ErrorCode< OSStatus, kUSBNotSent2Err >                         USBNotSent2Err;
   typedef Nucleus::ErrorCode< OSStatus, kUSBNotSent1Err >                         USBNotSent1Err;
   typedef Nucleus::ErrorCode< OSStatus, kUSBBufUnderRunErr >                      USBBufUnderRunErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBBufOvrRunErr >                        USBBufOvrRunErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBRes2Err >                             USBRes2Err;
   typedef Nucleus::ErrorCode< OSStatus, kUSBRes1Err >                             USBRes1Err;
   typedef Nucleus::ErrorCode< OSStatus, kUSBUnderRunErr >                         USBUnderRunErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBOverRunErr >                          USBOverRunErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBWrongPIDErr >                         USBWrongPIDErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBPIDCheckErr >                         USBPIDCheckErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBNotRespondingErr >                    USBNotRespondingErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBEndpointStallErr >                    USBEndpointStallErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBDataToggleErr >                       USBDataToggleErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBBitstufErr >                          USBBitstufErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBCRCErr >                              USBCRCErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBLinkErr >                             USBLinkErr;


/* USB Manager Errors */
   typedef Nucleus::ErrorCode< OSStatus, kUSBQueueFull >                           USBQueueFull;
   typedef Nucleus::ErrorCode< OSStatus, kUSBNotHandled >                          USBNotHandled;
   typedef Nucleus::ErrorCode< OSStatus, kUSBUnknownNotification >                 USBUnknownNotification;
   typedef Nucleus::ErrorCode< OSStatus, kUSBBadDispatchTable >                    USBBadDispatchTable;


/* USB Services Errors */
   typedef Nucleus::ErrorCode< OSStatus, kUSBPortDisabled >                        USBPortDisabled;
   typedef Nucleus::ErrorCode< OSStatus, kUSBQueueAborted >                        USBQueueAborted;
   typedef Nucleus::ErrorCode< OSStatus, kUSBTimedOut >                            USBTimedOut;
   typedef Nucleus::ErrorCode< OSStatus, kUSBDeviceDisconnected >                  USBDeviceDisconnected;
   typedef Nucleus::ErrorCode< OSStatus, kUSBDeviceNotSuspended >                  USBDeviceNotSuspended;
   typedef Nucleus::ErrorCode< OSStatus, kUSBDeviceSuspended >                     USBDeviceSuspended;
   typedef Nucleus::ErrorCode< OSStatus, kUSBInvalidBuffer >                       USBInvalidBuffer;
   typedef Nucleus::ErrorCode< OSStatus, kUSBDevicePowerProblem >                  USBDevicePowerProblem;
   typedef Nucleus::ErrorCode< OSStatus, kUSBDeviceBusy >                          USBDeviceBusy;
   typedef Nucleus::ErrorCode< OSStatus, kUSBUnknownInterfaceErr >                 USBUnknownInterfaceErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBPipeStalledError >                    USBPipeStalledError;
   typedef Nucleus::ErrorCode< OSStatus, kUSBPipeIdleError >                       USBPipeIdleError;
   typedef Nucleus::ErrorCode< OSStatus, kUSBNoBandwidthError >                    USBNoBandwidthError;
   typedef Nucleus::ErrorCode< OSStatus, kUSBAbortedError >                        USBAbortedError;
   typedef Nucleus::ErrorCode< OSStatus, kUSBFlagsError >                          USBFlagsError;
   typedef Nucleus::ErrorCode< OSStatus, kUSBCompletionError >                     USBCompletionError;
   typedef Nucleus::ErrorCode< OSStatus, kUSBPBLengthError >                       USBPBLengthError;
   typedef Nucleus::ErrorCode< OSStatus, kUSBPBVersionError >                      USBPBVersionError;
   typedef Nucleus::ErrorCode< OSStatus, kUSBNotFound >                            USBNotFound;
   typedef Nucleus::ErrorCode< OSStatus, kUSBOutOfMemoryErr >                      USBOutOfMemoryErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBDeviceErr >                           USBDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBNoDeviceErr >                         USBNoDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBAlreadyOpenErr >                      USBAlreadyOpenErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBTooManyTransactionsErr >              USBTooManyTransactionsErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBUnknownRequestErr >                   USBUnknownRequestErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBRqErr >                               USBRqErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBIncorrectTypeErr >                    USBIncorrectTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBTooManyPipesErr >                     USBTooManyPipesErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBUnknownPipeErr >                      USBUnknownPipeErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBUnknownDeviceErr >                    USBUnknownDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, kUSBInternalErr >                         USBInternalErr;


/* DictionaryMgr error codes */
   typedef Nucleus::ErrorCode< OSStatus, dcmParamErr >                             DCMParamErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmNotDictionaryErr >                     DCMNotDictionaryErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBadDictionaryErr >                     DCMBadDictionaryErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmPermissionErr >                        DCMPermissionErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmDictionaryNotOpenErr >                 DCMDictionaryNotOpenErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmDictionaryBusyErr >                    DCMDictionaryBusyErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBlockFullErr >                         DCMBlockFullErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmNoRecordErr >                          DCMNoRecordErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmDupRecordErr >                         DCMDupRecordErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmNecessaryFieldErr >                    DCMNecessaryFieldErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBadFieldInfoErr >                      DCMBadFieldInfoErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBadFieldTypeErr >                      DCMBadFieldTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmNoFieldErr >                           DCMNoFieldErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBadKeyErr >                            DCMBadKeyErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmTooManyKeyErr >                        DCMTooManyKeyErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBadDataSizeErr >                       DCMBadDataSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBadFindMethodErr >                     DCMBadFindMethodErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBadPropertyErr >                       DCMBadPropertyErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmProtectedErr >                         DCMProtectedErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmNoAccessMethodErr >                    DCMNoAccessMethodErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBadFeatureErr >                        DCMBadFeatureErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmIterationCompleteErr >                 DCMIterationCompleteErr;
   typedef Nucleus::ErrorCode< OSStatus, dcmBufferOverflowErr >                    DCMBufferOverflowErr;


/* Apple Remote Access error codes*/
   typedef Nucleus::ErrorCode< OSStatus, kRAInvalidParameter >                     RAInvalidParameter;
   typedef Nucleus::ErrorCode< OSStatus, kRAInvalidPort >                          RAInvalidPort;
   typedef Nucleus::ErrorCode< OSStatus, kRAStartupFailed >                        RAStartupFailed;
   typedef Nucleus::ErrorCode< OSStatus, kRAPortSetupFailed >                      RAPortSetupFailed;
   typedef Nucleus::ErrorCode< OSStatus, kRAOutOfMemory >                          RAOutOfMemory;
   typedef Nucleus::ErrorCode< OSStatus, kRANotSupported >                         RANotSupported;
   typedef Nucleus::ErrorCode< OSStatus, kRAMissingResources >                     RAMissingResources;
   typedef Nucleus::ErrorCode< OSStatus, kRAIncompatiblePrefs >                    RAIncompatiblePrefs;
   typedef Nucleus::ErrorCode< OSStatus, kRANotConnected >                         RANotConnected;
   typedef Nucleus::ErrorCode< OSStatus, kRAConnectionCanceled >                   RAConnectionCanceled;
   typedef Nucleus::ErrorCode< OSStatus, kRAUnknownUser >                          RAUnknownUser;
   typedef Nucleus::ErrorCode< OSStatus, kRAInvalidPassword >                      RAInvalidPassword;
   typedef Nucleus::ErrorCode< OSStatus, kRAInternalError >                        RAInternalError;
   typedef Nucleus::ErrorCode< OSStatus, kRAInstallationDamaged >                  RAInstallationDamaged;
   typedef Nucleus::ErrorCode< OSStatus, kRAPortBusy >                             RAPortBusy;
   typedef Nucleus::ErrorCode< OSStatus, kRAUnknownPortState >                     RAUnknownPortState;
   typedef Nucleus::ErrorCode< OSStatus, kRAInvalidPortState >                     RAInvalidPortState;
   typedef Nucleus::ErrorCode< OSStatus, kRAInvalidSerialProtocol >                RAInvalidSerialProtocol;
   typedef Nucleus::ErrorCode< OSStatus, kRAUserLoginDisabled >                    RAUserLoginDisabled;
   typedef Nucleus::ErrorCode< OSStatus, kRAUserPwdChangeRequired >                RAUserPwdChangeRequired;
   typedef Nucleus::ErrorCode< OSStatus, kRAUserPwdEntryRequired >                 RAUserPwdEntryRequired;
   typedef Nucleus::ErrorCode< OSStatus, kRAUserInteractionRequired >              RAUserInteractionRequired;
   typedef Nucleus::ErrorCode< OSStatus, kRAInitOpenTransportFailed >              RAInitOpenTransportFailed;
   typedef Nucleus::ErrorCode< OSStatus, kRARemoteAccessNotReady >                 RARemoteAccessNotReady;
   typedef Nucleus::ErrorCode< OSStatus, kRATCPIPInactive >                        RATCPIPInactive;
   typedef Nucleus::ErrorCode< OSStatus, kRATCPIPNotConfigured >                   RATCPIPNotConfigured;
   typedef Nucleus::ErrorCode< OSStatus, kRANotPrimaryInterface >                  RANotPrimaryInterface;
   typedef Nucleus::ErrorCode< OSStatus, kRAConfigurationDBInitErr >               RAConfigurationDBInitErr;
   typedef Nucleus::ErrorCode< OSStatus, kRAPPPProtocolRejected >                  RAPPPProtocolRejected;
   typedef Nucleus::ErrorCode< OSStatus, kRAPPPAuthenticationFailed >              RAPPPAuthenticationFailed;
   typedef Nucleus::ErrorCode< OSStatus, kRAPPPNegotiationFailed >                 RAPPPNegotiationFailed;
   typedef Nucleus::ErrorCode< OSStatus, kRAPPPUserDisconnected >                  RAPPPUserDisconnected;
   typedef Nucleus::ErrorCode< OSStatus, kRAPPPPeerDisconnected >                  RAPPPPeerDisconnected;
   typedef Nucleus::ErrorCode< OSStatus, kRAPeerNotResponding >                    RAPeerNotResponding;
   typedef Nucleus::ErrorCode< OSStatus, kRAATalkInactive >                        RAATalkInactive;
   typedef Nucleus::ErrorCode< OSStatus, kRAExtAuthenticationFailed >              RAExtAuthenticationFailed;
   typedef Nucleus::ErrorCode< OSStatus, kRANCPRejectedbyPeer >                    RANCPRejectedbyPeer;
   typedef Nucleus::ErrorCode< OSStatus, kRADuplicateIPAddr >                      RADuplicateIPAddr;
   typedef Nucleus::ErrorCode< OSStatus, kRACallBackFailed >                       RACallBackFailed;
   typedef Nucleus::ErrorCode< OSStatus, kRANotEnabled >                           RANotEnabled;

/* ATSUnicode error codes*/
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidTextLayoutErr >               ATSUInvalidTextLayoutErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidStyleErr >                    ATSUInvalidStyleErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidTextRangeErr >                ATSUInvalidTextRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUFontsMatched >                       ATSUFontsMatched;
   typedef Nucleus::ErrorCode< OSStatus, kATSUFontsNotMatched >                    ATSUFontsNotMatched;
   typedef Nucleus::ErrorCode< OSStatus, kATSUNoCorrespondingFontErr>              ATSUNoCorrespondingFontErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidFontErr >                     ATSUInvalidFontErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidAttributeValueErr >           ATSUInvalidAttributeValueErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidAttributeSizeErr >            ATSUInvalidAttributeSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidAttributeTagErr>              ATSUInvalidAttributeTagErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidCacheErr >                    ATSUInvalidCacheErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUNotSetErr >                          ATSUNotSetErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUNoStyleRunsAssignedErr>              ATSUNoStyleRunsAssignedErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUQuickDrawTextErr >                   ATSUQuickDrawTextErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSULowLevelErr >                        ATSULowLevelErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUNoFontCmapAvailableErr>              ATSUNoFontCmapAvailableErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUNoFontScalerAvailableErr >           ATSUNoFontScalerAvailableErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUCoordinateOverflowErr >              ATSUCoordinateOverflowErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSULineBreakInWord >                    ATSULineBreakInWord;
   typedef Nucleus::ErrorCode< OSStatus, kATSUBusyObjectErr >                      ATSUBusyObjectErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSUInvalidFontFallbacksErr >            ATSUInvalidFontFallbacksErr;
   typedef Nucleus::ErrorCode< OSStatus, kATSULastErr >                            ATSULastErr;


/* general text errors*/
   typedef Nucleus::ErrorCode< OSStatus, kTextUnsupportedEncodingErr>              TextUnsupportedEncodingErr;
   typedef Nucleus::ErrorCode< OSStatus, kTextMalformedInputErr >                  TextMalformedInputErr;
   typedef Nucleus::ErrorCode< OSStatus, kTextUndefinedElementErr >                TextUndefinedElementErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECMissingTableErr >                     TECMissingTableErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECTableChecksumErr >                    TECTableChecksumErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECTableFormatErr >                      TECTableFormatErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECCorruptConverterErr >                 TECCorruptConverterErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECNoConversionPathErr >                 TECNoConversionPathErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECBufferBelowMinimumSizeErr >           TECBufferBelowMinimumSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECArrayFullErr >                        TECArrayFullErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECBadTextRunErr >                       TECBadTextRunErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECPartialCharErr >                      TECPartialCharErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECUnmappableElementErr >                TECUnmappableElementErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECIncompleteElementErr >                TECIncompleteElementErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECDirectionErr >                        TECDirectionErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECGlobalsUnavailableErr >               TECGlobalsUnavailableErr;
   typedef Nucleus::ErrorCode< OSStatus, kTECItemUnavailableErr >                  TECItemUnavailableErr;

/* text conversion status codes*/
   typedef Nucleus::ErrorCode< OSStatus, kTECUsedFallbacksStatus >                 TECUsedFallbacksStatus;
   typedef Nucleus::ErrorCode< OSStatus, kTECNeedFlushStatus >                     TECNeedFlushStatus;
   typedef Nucleus::ErrorCode< OSStatus, kTECOutputBufferFullStatus >              TECOutputBufferFullStatus;

/* deprecated error & status codes for low-level converter*/
   typedef Nucleus::ErrorCode< OSStatus, unicodeChecksumErr >                      UnicodeChecksumErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeNoTableErr >                       UnicodeNoTableErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeVariantErr >                       UnicodeVariantErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeFallbacksErr >                     UnicodeFallbacksErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodePartConvertErr >                   UnicodePartConvertErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeBufErr >                           UnicodeBufErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeCharErr >                          UnicodeCharErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeElementErr >                       UnicodeElementErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeNotFoundErr >                      UnicodeNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeTableFormatErr >                   UnicodeTableFormatErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeDirectionErr >                     UnicodeDirectionErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeContextualErr >                    UnicodeContextualErr;
   typedef Nucleus::ErrorCode< OSStatus, unicodeTextEncodingDataErr >              UnicodeTextEncodingDataErr;


/* UTCUtils Status Codes */
   typedef Nucleus::ErrorCode< OSStatus, kUTCUnderflowErr >                        UTCUnderflowErr;
   typedef Nucleus::ErrorCode< OSStatus, kUTCOverflowErr >                         UTCOverflowErr;
   typedef Nucleus::ErrorCode< OSStatus, kIllegalClockValueErr >                   IllegalClockValueErr;


   typedef Nucleus::ErrorCode< OSStatus, codecErr >                                CodecErr;
   typedef Nucleus::ErrorCode< OSStatus, noCodecErr >                              NoCodecErr;
   typedef Nucleus::ErrorCode< OSStatus, codecUnimpErr >                           CodecUnimpErr;
   typedef Nucleus::ErrorCode< OSStatus, codecSizeErr >                            CodecSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, codecScreenBufErr >                       CodecScreenBufErr;
   typedef Nucleus::ErrorCode< OSStatus, codecImageBufErr >                        CodecImageBufErr;
   typedef Nucleus::ErrorCode< OSStatus, codecSpoolErr >                           CodecSpoolErr;
   typedef Nucleus::ErrorCode< OSStatus, codecAbortErr >                           CodecAbortErr;
   typedef Nucleus::ErrorCode< OSStatus, codecWouldOffscreenErr >                  CodecWouldOffscreenErr;
   typedef Nucleus::ErrorCode< OSStatus, codecBadDataErr >                         CodecBadDataErr;
   typedef Nucleus::ErrorCode< OSStatus, codecDataVersErr >                        CodecDataVersErr;
   typedef Nucleus::ErrorCode< OSStatus, codecExtensionNotFoundErr >               CodecExtensionNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, scTypeNotFoundErr >                       ScTypeNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, codecConditionErr >                       CodecConditionErr;
   typedef Nucleus::ErrorCode< OSStatus, codecOpenErr >                            CodecOpenErr;
   typedef Nucleus::ErrorCode< OSStatus, codecCantWhenErr >                        CodecCantWhenErr;
   typedef Nucleus::ErrorCode< OSStatus, codecCantQueueErr >                       CodecCantQueueErr;
   typedef Nucleus::ErrorCode< OSStatus, codecNothingToBlitErr >                   CodecNothingToBlitErr;
   typedef Nucleus::ErrorCode< OSStatus, codecNoMemoryPleaseWaitErr >              CodecNoMemoryPleaseWaitErr;
   typedef Nucleus::ErrorCode< OSStatus, codecDisabledErr >                        CodecDisabledErr;
   typedef Nucleus::ErrorCode< OSStatus, codecNeedToFlushChainErr >                CodecNeedToFlushChainErr;
   typedef Nucleus::ErrorCode< OSStatus, lockPortBitsBadSurfaceErr >               LockPortBitsBadSurfaceErr;
   typedef Nucleus::ErrorCode< OSStatus, lockPortBitsWindowMovedErr >              LockPortBitsWindowMovedErr;
   typedef Nucleus::ErrorCode< OSStatus, lockPortBitsWindowResizedErr >            LockPortBitsWindowResizedErr;
   typedef Nucleus::ErrorCode< OSStatus, lockPortBitsWindowClippedErr >            LockPortBitsWindowClippedErr;
   typedef Nucleus::ErrorCode< OSStatus, lockPortBitsBadPortErr >                  LockPortBitsBadPortErr;
   typedef Nucleus::ErrorCode< OSStatus, lockPortBitsSurfaceLostErr >              LockPortBitsSurfaceLostErr;
   typedef Nucleus::ErrorCode< OSStatus, codecParameterDialogConfirm>              CodecParameterDialogConfirm;
   typedef Nucleus::ErrorCode< OSStatus, codecNeedAccessKeyErr >                   CodecNeedAccessKeyErr;
   typedef Nucleus::ErrorCode< OSStatus, codecOffscreenFailedErr >                 CodecOffscreenFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, codecDroppedFrameErr >                    CodecDroppedFrameErr;
   typedef Nucleus::ErrorCode< OSStatus, directXObjectAlreadyExists >              DirectXObjectAlreadyExists;
   typedef Nucleus::ErrorCode< OSStatus, lockPortBitsWrongGDeviceErr>              LockPortBitsWrongGDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, codecOffscreenFailedPleaseRetryErr >      CodecOffscreenFailedPleaseRetryErr;


/* PCCard error codes */
   typedef Nucleus::ErrorCode< OSStatus, kBadAdapterErr >                          BadAdapterErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadAttributeErr >                        BadAttributeErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadBaseErr >                             BadBaseErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadEDCErr >                              BadEDCErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadIRQErr >                              BadIRQErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadOffsetErr >                           BadOffsetErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadPageErr >                             BadPageErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadSizeErr >                             BadSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadSocketErr >                           BadSocketErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadTypeErr >                             BadTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadVccErr >                              BadVccErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadVppErr >                              BadVppErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadWindowErr >                           BadWindowErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadArgLengthErr >                        BadArgLengthErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadArgsErr >                             BadArgsErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadHandleErr >                           BadHandleErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadCISErr >                              BadCISErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadSpeedErr >                            BadSpeedErr;
   typedef Nucleus::ErrorCode< OSStatus, kReadFailureErr >                         ReadFailureErr;
   typedef Nucleus::ErrorCode< OSStatus, kWriteFailureErr >                        WriteFailureErr;
   typedef Nucleus::ErrorCode< OSStatus, kGeneralFailureErr >                      GeneralFailureErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoCardErr >                              NoCardErr;
   typedef Nucleus::ErrorCode< OSStatus, kUnsupportedFunctionErr >                 UnsupportedFunctionErr;
   typedef Nucleus::ErrorCode< OSStatus, kUnsupportedModeErr >                     UnsupportedModeErr;
   typedef Nucleus::ErrorCode< OSStatus, kBusyErr >                                BusyErr;
   typedef Nucleus::ErrorCode< OSStatus, kWriteProtectedErr >                      WriteProtectedErr;
   typedef Nucleus::ErrorCode< OSStatus, kConfigurationLockedErr >                 ConfigurationLockedErr;
   typedef Nucleus::ErrorCode< OSStatus, kInUseErr >                               InUseErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoMoreItemsErr >                         NoMoreItemsErr;
   typedef Nucleus::ErrorCode< OSStatus, kOutOfResourceErr >                       OutOfResourceErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoCardSevicesSocketsErr >                NoCardSevicesSocketsErr;
   typedef Nucleus::ErrorCode< OSStatus, kInvalidRegEntryErr >                     InvalidRegEntryErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadLinkErr >                             BadLinkErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadDeviceErr >                           BadDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, k16BitCardErr >                           K16BitCardErr;
   typedef Nucleus::ErrorCode< OSStatus, kCardBusCardErr >                         CardBusCardErr;
   typedef Nucleus::ErrorCode< OSStatus, kPassCallToChainErr >                     PassCallToChainErr;
   typedef Nucleus::ErrorCode< OSStatus, kCantConfigureCardErr >                   CantConfigureCardErr;
   typedef Nucleus::ErrorCode< OSStatus, kPostCardEventErr >                       PostCardEventErr;
   typedef Nucleus::ErrorCode< OSStatus, kInvalidDeviceNumber >                    InvalidDeviceNumber;
   typedef Nucleus::ErrorCode< OSStatus, kUnsupportedVsErr >                       UnsupportedVsErr;
   typedef Nucleus::ErrorCode< OSStatus, kInvalidCSClientErr >                     InvalidCSClientErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadTupleDataErr >                        BadTupleDataErr;
   typedef Nucleus::ErrorCode< OSStatus, kBadCustomIFIDErr >                       BadCustomIFIDErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoIOWindowRequestedErr >                 NoIOWindowRequestedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoMoreTimerClientsErr >                  NoMoreTimerClientsErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoMoreInterruptSlotsErr >                NoMoreInterruptSlotsErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoClientTableErr >                       NoClientTableErr;
   typedef Nucleus::ErrorCode< OSStatus, kUnsupportedCardErr >                     UnsupportedCardErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoCardEnablersFoundErr >                 NoCardEnablersFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoEnablerForCardErr >                    NoEnablerForCardErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoCompatibleNameErr >                    NoCompatibleNameErr;
   typedef Nucleus::ErrorCode< OSStatus, kClientRequestDenied >                    ClientRequestDenied;
   typedef Nucleus::ErrorCode< OSStatus, kNotReadyErr >                            NotReadyErr;
   typedef Nucleus::ErrorCode< OSStatus, kTooManyIOWindowsErr >                    TooManyIOWindowsErr;
   typedef Nucleus::ErrorCode< OSStatus, kAlreadySavedStateErr >                   AlreadySavedStateErr;
   typedef Nucleus::ErrorCode< OSStatus, kAttemptDupCardEntryErr >                 AttemptDupCardEntryErr;
   typedef Nucleus::ErrorCode< OSStatus, kCardPowerOffErr >                        CardPowerOffErr;
   typedef Nucleus::ErrorCode< OSStatus, kNotZVCapableErr >                        NotZVCapableErr;
   typedef Nucleus::ErrorCode< OSStatus, kNoCardBusCISErr >                        NoCardBusCISErr;


   typedef Nucleus::ErrorCode< OSStatus, noDeviceForChannel >                      NoDeviceForChannel;
   typedef Nucleus::ErrorCode< OSStatus, grabTimeComplete >                        GrabTimeComplete;
   typedef Nucleus::ErrorCode< OSStatus, cantDoThatInCurrentMode >                 CantDoThatInCurrentMode;
   typedef Nucleus::ErrorCode< OSStatus, notEnoughMemoryToGrab >                   NotEnoughMemoryToGrab;
   typedef Nucleus::ErrorCode< OSStatus, notEnoughDiskSpaceToGrab >                NotEnoughDiskSpaceToGrab;
   typedef Nucleus::ErrorCode< OSStatus, couldntGetRequiredComponent>              CouldntGetRequiredComponent;
   typedef Nucleus::ErrorCode< OSStatus, badSGChannel >                            BadSGChannel;
   typedef Nucleus::ErrorCode< OSStatus, seqGrabInfoNotAvailable >                 SeqGrabInfoNotAvailable;
   typedef Nucleus::ErrorCode< OSStatus, deviceCantMeetRequest >                   DeviceCantMeetRequest;
   typedef Nucleus::ErrorCode< OSStatus, badControllerHeight >                     BadControllerHeight;
   typedef Nucleus::ErrorCode< OSStatus, editingNotAllowed >                       EditingNotAllowed;
   typedef Nucleus::ErrorCode< OSStatus, controllerBoundsNotExact >                ControllerBoundsNotExact;
   typedef Nucleus::ErrorCode< OSStatus, cannotSetWidthOfAttachedController >      CannotSetWidthOfAttachedController;
   typedef Nucleus::ErrorCode< OSStatus, controllerHasFixedHeight >                ControllerHasFixedHeight;
   typedef Nucleus::ErrorCode< OSStatus, cannotMoveAttachedController >            CannotMoveAttachedController;

/* AERegistry Errors */
   typedef Nucleus::ErrorCode< OSStatus, errAEBadKeyForm >                         ErrAEBadKeyForm;
   typedef Nucleus::ErrorCode< OSStatus, errAECantHandleClass >                    ErrAECantHandleClass;
   typedef Nucleus::ErrorCode< OSStatus, errAECantSupplyType >                     ErrAECantSupplyType;
   typedef Nucleus::ErrorCode< OSStatus, errAECantUndo >                           ErrAECantUndo;
   typedef Nucleus::ErrorCode< OSStatus, errAEEventFailed >                        ErrAEEventFailed;
   typedef Nucleus::ErrorCode< OSStatus, errAEIndexTooLarge >                      ErrAEIndexTooLarge;
   typedef Nucleus::ErrorCode< OSStatus, errAEInTransaction >                      ErrAEInTransaction;
   typedef Nucleus::ErrorCode< OSStatus, errAELocalOnly >                          ErrAELocalOnly;
   typedef Nucleus::ErrorCode< OSStatus, errAENoSuchTransaction >                  ErrAENoSuchTransaction;
   typedef Nucleus::ErrorCode< OSStatus, errAENotAnElement >                       ErrAENotAnElement;
   typedef Nucleus::ErrorCode< OSStatus, errAENotASingleObject >                   ErrAENotASingleObject;
   typedef Nucleus::ErrorCode< OSStatus, errAENotModifiable >                      ErrAENotModifiable;
   typedef Nucleus::ErrorCode< OSStatus, errAENoUserSelection >                    ErrAENoUserSelection;
   typedef Nucleus::ErrorCode< OSStatus, errAEPrivilegeError >                     ErrAEPrivilegeError;
   typedef Nucleus::ErrorCode< OSStatus, errAEReadDenied >                         ErrAEReadDenied;
   typedef Nucleus::ErrorCode< OSStatus, errAETypeError >                          ErrAETypeError;
   typedef Nucleus::ErrorCode< OSStatus, errAEWriteDenied >                        ErrAEWriteDenied;
   typedef Nucleus::ErrorCode< OSStatus, errAENotAnEnumMember >                    ErrAENotAnEnumMember;
   typedef Nucleus::ErrorCode< OSStatus, errAECantPutThatThere >                   ErrAECantPutThatThere;
   typedef Nucleus::ErrorCode< OSStatus, errAEPropertiesClash >                    ErrAEPropertiesClash;

/* TELErr */
   typedef Nucleus::ErrorCode< OSStatus, telGenericError >                         TelGenericError;
   typedef Nucleus::ErrorCode< OSStatus, telNoErr >                                TelNoErr;
   typedef Nucleus::ErrorCode< OSStatus, telNoTools >                              TelNoTools;
   typedef Nucleus::ErrorCode< OSStatus, telBadTermErr >                           TelBadTermErr;
   typedef Nucleus::ErrorCode< OSStatus, telBadDNErr >                             TelBadDNErr;
   typedef Nucleus::ErrorCode< OSStatus, telBadCAErr >                             TelBadCAErr;
   typedef Nucleus::ErrorCode< OSStatus, telBadHandErr >                           TelBadHandErr;
   typedef Nucleus::ErrorCode< OSStatus, telBadProcErr >                           TelBadProcErr;
   typedef Nucleus::ErrorCode< OSStatus, telCAUnavail >                            TelCAUnavail;
   typedef Nucleus::ErrorCode< OSStatus, telNoMemErr >                             TelNoMemErr;
   typedef Nucleus::ErrorCode< OSStatus, telNoOpenErr >                            TelNoOpenErr;
   typedef Nucleus::ErrorCode< OSStatus, telBadHTypeErr >                          TelBadHTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, telHTypeNotSupp >                         TelHTypeNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telBadLevelErr >                          TelBadLevelErr;
   typedef Nucleus::ErrorCode< OSStatus, telBadVTypeErr >                          TelBadVTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, telVTypeNotSupp >                         TelVTypeNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telBadAPattErr >                          TelBadAPattErr;
   typedef Nucleus::ErrorCode< OSStatus, telAPattNotSupp >                         TelAPattNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telBadIndex >                             TelBadIndex;
   typedef Nucleus::ErrorCode< OSStatus, telIndexNotSupp >                         TelIndexNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telBadStateErr >                          TelBadStateErr;
   typedef Nucleus::ErrorCode< OSStatus, telStateNotSupp >                         TelStateNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telBadIntExt >                            TelBadIntExt;
   typedef Nucleus::ErrorCode< OSStatus, telIntExtNotSupp >                        TelIntExtNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telBadDNDType >                           TelBadDNDType;
   typedef Nucleus::ErrorCode< OSStatus, telDNDTypeNotSupp >                       TelDNDTypeNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telFeatNotSub >                           TelFeatNotSub;
   typedef Nucleus::ErrorCode< OSStatus, telFeatNotAvail >                         TelFeatNotAvail;
   typedef Nucleus::ErrorCode< OSStatus, telFeatActive >                           TelFeatActive;
   typedef Nucleus::ErrorCode< OSStatus, telFeatNotSupp >                          TelFeatNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telConfLimitErr >                         TelConfLimitErr;
   typedef Nucleus::ErrorCode< OSStatus, telConfNoLimit >                          TelConfNoLimit;
   typedef Nucleus::ErrorCode< OSStatus, telConfErr >                              TelConfErr;
   typedef Nucleus::ErrorCode< OSStatus, telConfRej >                              TelConfRej;
   typedef Nucleus::ErrorCode< OSStatus, telTransferErr >                          TelTransferErr;
   typedef Nucleus::ErrorCode< OSStatus, telTransferRej >                          TelTransferRej;
   typedef Nucleus::ErrorCode< OSStatus, telCBErr >                                TelCBErr;
   typedef Nucleus::ErrorCode< OSStatus, telConfLimitExceeded >                    TelConfLimitExceeded;
   typedef Nucleus::ErrorCode< OSStatus, telBadDNType >                            TelBadDNType;
   typedef Nucleus::ErrorCode< OSStatus, telBadPageID >                            TelBadPageID;
   typedef Nucleus::ErrorCode< OSStatus, telBadIntercomID >                        TelBadIntercomID;
   typedef Nucleus::ErrorCode< OSStatus, telBadFeatureID >                         TelBadFeatureID;
   typedef Nucleus::ErrorCode< OSStatus, telBadFwdType >                           TelBadFwdType;
   typedef Nucleus::ErrorCode< OSStatus, telBadPickupGroupID >                     TelBadPickupGroupID;
   typedef Nucleus::ErrorCode< OSStatus, telBadParkID >                            TelBadParkID;
   typedef Nucleus::ErrorCode< OSStatus, telBadSelect >                            TelBadSelect;
   typedef Nucleus::ErrorCode< OSStatus, telBadBearerType >                        TelBadBearerType;
   typedef Nucleus::ErrorCode< OSStatus, telBadRate >                              TelBadRate;
   typedef Nucleus::ErrorCode< OSStatus, telDNTypeNotSupp >                        TelDNTypeNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telFwdTypeNotSupp >                       TelFwdTypeNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telBadDisplayMode >                       TelBadDisplayMode;
   typedef Nucleus::ErrorCode< OSStatus, telDisplayModeNotSupp >                   TelDisplayModeNotSupp;
   typedef Nucleus::ErrorCode< OSStatus, telNoCallbackRef >                        TelNoCallbackRef;
   typedef Nucleus::ErrorCode< OSStatus, telAlreadyOpen >                          TelAlreadyOpen;
   typedef Nucleus::ErrorCode< OSStatus, telStillNeeded >                          TelStillNeeded;
   typedef Nucleus::ErrorCode< OSStatus, telTermNotOpen >                          TelTermNotOpen;
   typedef Nucleus::ErrorCode< OSStatus, telCANotAcceptable >                      TelCANotAcceptable;
   typedef Nucleus::ErrorCode< OSStatus, telCANotRejectable >                      TelCANotRejectable;
   typedef Nucleus::ErrorCode< OSStatus, telCANotDeflectable >                     TelCANotDeflectable;
   typedef Nucleus::ErrorCode< OSStatus, telPBErr >                                TelPBErr;
   typedef Nucleus::ErrorCode< OSStatus, telBadFunction >                          TelBadFunction;
   typedef Nucleus::ErrorCode< OSStatus, telNoSuchTool >                           TelNoSuchTool;
   typedef Nucleus::ErrorCode< OSStatus, telUnknownErr >                           TelUnknownErr;
   typedef Nucleus::ErrorCode< OSStatus, telNoCommFolder >                         TelNoCommFolder;
   typedef Nucleus::ErrorCode< OSStatus, telInitFailed >                           TelInitFailed;
   typedef Nucleus::ErrorCode< OSStatus, telBadCodeResource >                      TelBadCodeResource;
   typedef Nucleus::ErrorCode< OSStatus, telDeviceNotFound >                       TelDeviceNotFound;
   typedef Nucleus::ErrorCode< OSStatus, telBadProcID >                            TelBadProcID;
   typedef Nucleus::ErrorCode< OSStatus, telValidateFailed >                       TelValidateFailed;
   typedef Nucleus::ErrorCode< OSStatus, telAutoAnsNotOn >                         TelAutoAnsNotOn;
   typedef Nucleus::ErrorCode< OSStatus, telDetAlreadyOn >                         TelDetAlreadyOn;
   typedef Nucleus::ErrorCode< OSStatus, telBadSWErr >                             TelBadSWErr;
   typedef Nucleus::ErrorCode< OSStatus, telBadSampleRate >                        TelBadSampleRate;
   typedef Nucleus::ErrorCode< OSStatus, telNotEnoughdspBW >                       TelNotEnoughdspBW;

   typedef Nucleus::ErrorCode< OSStatus, errTaskNotFound >                         ErrTaskNotFound;

/*Power Manager Errors*/
   typedef Nucleus::ErrorCode< OSStatus, pmBusyErr >                               PMBusyErr;
   typedef Nucleus::ErrorCode< OSStatus, pmReplyTOErr >                            PMReplyTOErr;
   typedef Nucleus::ErrorCode< OSStatus, pmSendStartErr >                          PMSendStartErr;
   typedef Nucleus::ErrorCode< OSStatus, pmSendEndErr >                            PMSendEndErr;
   typedef Nucleus::ErrorCode< OSStatus, pmRecvStartErr >                          PMRecvStartErr;
   typedef Nucleus::ErrorCode< OSStatus, pmRecvEndErr >                            PMRecvEndErr;

/*Power Manager 2.0 Errors*/
   typedef Nucleus::ErrorCode< OSStatus, kPowerHandlerExistsForDeviceErr >         PowerHandlerExistsForDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, kPowerHandlerNotFoundForDeviceErr >       PowerHandlerNotFoundForDeviceErr;
   typedef Nucleus::ErrorCode< OSStatus, kPowerHandlerNotFoundForProcErr >         PowerHandlerNotFoundForProcErr;
   typedef Nucleus::ErrorCode< OSStatus, kPowerMgtMessageNotHandled >              PowerMgtMessageNotHandled;
   typedef Nucleus::ErrorCode< OSStatus, kPowerMgtRequestDenied >                  PowerMgtRequestDenied;
   typedef Nucleus::ErrorCode< OSStatus, kCantReportProcessorTemperatureErr >      CantReportProcessorTemperatureErr;
   typedef Nucleus::ErrorCode< OSStatus, kProcessorTempRoutineRequiresMPLib2 >     ProcessorTempRoutineRequiresMPLib2;
   typedef Nucleus::ErrorCode< OSStatus, kNoSuchPowerSource >                      NoSuchPowerSource;
   typedef Nucleus::ErrorCode< OSStatus, kBridgeSoftwareRunningCantSleep >         BridgeSoftwareRunningCantSleep;


/* Debugging library errors */
   typedef Nucleus::ErrorCode< OSStatus, debuggingExecutionContextErr >            DebuggingExecutionContextErr;
   typedef Nucleus::ErrorCode< OSStatus, debuggingDuplicateSignatureErr >          DebuggingDuplicateSignatureErr;
   typedef Nucleus::ErrorCode< OSStatus, debuggingDuplicateOptionErr>              DebuggingDuplicateOptionErr;
   typedef Nucleus::ErrorCode< OSStatus, debuggingInvalidSignatureErr >            DebuggingInvalidSignatureErr;
   typedef Nucleus::ErrorCode< OSStatus, debuggingInvalidOptionErr >               DebuggingInvalidOptionErr;
   typedef Nucleus::ErrorCode< OSStatus, debuggingInvalidNameErr >                 DebuggingInvalidNameErr;
   typedef Nucleus::ErrorCode< OSStatus, debuggingNoCallbackErr >                  DebuggingNoCallbackErr;
   typedef Nucleus::ErrorCode< OSStatus, debuggingNoMatchErr >                     DebuggingNoMatchErr;


/* HID device driver error codes */
   typedef Nucleus::ErrorCode< OSStatus, kHIDVersionIncompatibleErr >              HIDVersionIncompatibleErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDDeviceNotReady >                      HIDDeviceNotReady;


/* HID error codes */
   typedef Nucleus::ErrorCode< OSStatus, kHIDSuccess >                             HIDSuccess;
   typedef Nucleus::ErrorCode< OSStatus, kHIDInvalidRangePageErr >                 HIDInvalidRangePageErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDReportIDZeroErr >                     HIDReportIDZeroErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDReportCountZeroErr >                  HIDReportCountZeroErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDReportSizeZeroErr >                   HIDReportSizeZeroErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDUnmatchedDesignatorRangeErr >         HIDUnmatchedDesignatorRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDUnmatchedStringRangeErr>              HIDUnmatchedStringRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDInvertedUsageRangeErr >               HIDInvertedUsageRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDUnmatchedUsageRangeErr >              HIDUnmatchedUsageRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDInvertedPhysicalRangeErr >            HIDInvertedPhysicalRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDInvertedLogicalRangeErr>              HIDInvertedLogicalRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDBadLogicalMaximumErr >                HIDBadLogicalMaximumErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDBadLogicalMinimumErr >                HIDBadLogicalMinimumErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDUsagePageZeroErr >                    HIDUsagePageZeroErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDEndOfDescriptorErr >                  HIDEndOfDescriptorErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDNotEnoughMemoryErr >                  HIDNotEnoughMemoryErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDBadParameterErr >                     HIDBadParameterErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDNullPointerErr >                      HIDNullPointerErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDInvalidReportLengthErr >              HIDInvalidReportLengthErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDInvalidReportTypeErr >                HIDInvalidReportTypeErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDBadLogPhysValuesErr >                 HIDBadLogPhysValuesErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDIncompatibleReportErr >               HIDIncompatibleReportErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDInvalidPreparsedDataErr>              HIDInvalidPreparsedDataErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDNotValueArrayErr >                    HIDNotValueArrayErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDUsageNotFoundErr >                    HIDUsageNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDValueOutOfRangeErr >                  HIDValueOutOfRangeErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDBufferTooSmallErr >                   HIDBufferTooSmallErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDNullStateErr >                        HIDNullStateErr;
   typedef Nucleus::ErrorCode< OSStatus, kHIDBaseError >                           HIDBaseError;


/* the OT modem module may return the following error codes:*/
   typedef Nucleus::ErrorCode< OSStatus, kModemOutOfMemory >                       ModemOutOfMemory;
   typedef Nucleus::ErrorCode< OSStatus, kModemPreferencesMissing >                ModemPreferencesMissing;
   typedef Nucleus::ErrorCode< OSStatus, kModemScriptMissing >                     ModemScriptMissing;



/* MacTextEditor error codes */
   typedef Nucleus::ErrorCode< OSStatus, kTXNEndIterationErr >                     TXNEndIterationErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNCannotAddFrameErr >                   TXNCannotAddFrameErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNInvalidFrameIDErr >                   TXNInvalidFrameIDErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNIllegalToCrossDataBoundariesErr >     TXNIllegalToCrossDataBoundariesErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNUserCanceledOperationErr >            TXNUserCanceledOperationErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNBadDefaultFileTypeWarning >           TXNBadDefaultFileTypeWarning;
   typedef Nucleus::ErrorCode< OSStatus, kTXNCannotSetAutoIndentErr >              TXNCannotSetAutoIndentErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNRunIndexOutofBoundsErr >              TXNRunIndexOutofBoundsErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNNoMatchErr >                          TXNNoMatchErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNAttributeTagInvalidForRunErr >        TXNAttributeTagInvalidForRunErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNSomeOrAllTagsInvalidForRunErr >       TXNSomeOrAllTagsInvalidForRunErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNInvalidRunIndex >                     TXNInvalidRunIndex;
   typedef Nucleus::ErrorCode< OSStatus, kTXNAlreadyInitializedErr >               TXNAlreadyInitializedErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNCannotTurnTSMOffWhenUsingUnicodeErr > TXNCannotTurnTSMOffWhenUsingUnicodeErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNCopyNotAllowedInEchoModeErr >         TXNCopyNotAllowedInEchoModeErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNDataTypeNotAllowedErr >               TXNDataTypeNotAllowedErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNATSUIIsNotInstalledErr >              TXNATSUIIsNotInstalledErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNOutsideOfLineErr >                    TXNOutsideOfLineErr;
   typedef Nucleus::ErrorCode< OSStatus, kTXNOutsideOfFrameErr >                   TXNOutsideOfFrameErr;




/*Possible errors from the PrinterStatus bottleneck*/
   typedef Nucleus::ErrorCode< OSStatus, printerStatusOpCodeNotSupportedErr >      PrinterStatusOpCodeNotSupportedErr;


/* Keychain Manager error codes */
   typedef Nucleus::ErrorCode< OSStatus, errKCNotAvailable >                       ErrKCNotAvailable;
   typedef Nucleus::ErrorCode< OSStatus, errKCReadOnly >                           ErrKCReadOnly;
   typedef Nucleus::ErrorCode< OSStatus, errKCAuthFailed >                         ErrKCAuthFailed;
   typedef Nucleus::ErrorCode< OSStatus, errKCNoSuchKeychain >                     ErrKCNoSuchKeychain;
   typedef Nucleus::ErrorCode< OSStatus, errKCInvalidKeychain >                    ErrKCInvalidKeychain;
   typedef Nucleus::ErrorCode< OSStatus, errKCDuplicateKeychain >                  ErrKCDuplicateKeychain;
   typedef Nucleus::ErrorCode< OSStatus, errKCDuplicateCallback >                  ErrKCDuplicateCallback;
   typedef Nucleus::ErrorCode< OSStatus, errKCInvalidCallback >                    ErrKCInvalidCallback;
   typedef Nucleus::ErrorCode< OSStatus, errKCDuplicateItem >                      ErrKCDuplicateItem;
   typedef Nucleus::ErrorCode< OSStatus, errKCItemNotFound >                       ErrKCItemNotFound;
   typedef Nucleus::ErrorCode< OSStatus, errKCBufferTooSmall >                     ErrKCBufferTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, errKCDataTooLarge >                       ErrKCDataTooLarge;
   typedef Nucleus::ErrorCode< OSStatus, errKCNoSuchAttr >                         ErrKCNoSuchAttr;
   typedef Nucleus::ErrorCode< OSStatus, errKCInvalidItemRef >                     ErrKCInvalidItemRef;
   typedef Nucleus::ErrorCode< OSStatus, errKCInvalidSearchRef >                   ErrKCInvalidSearchRef;
   typedef Nucleus::ErrorCode< OSStatus, errKCNoSuchClass >                        ErrKCNoSuchClass;
   typedef Nucleus::ErrorCode< OSStatus, errKCNoDefaultKeychain >                  ErrKCNoDefaultKeychain;
   typedef Nucleus::ErrorCode< OSStatus, errKCInteractionNotAllowed >              ErrKCInteractionNotAllowed;
   typedef Nucleus::ErrorCode< OSStatus, errKCReadOnlyAttr >                       ErrKCReadOnlyAttr;
   typedef Nucleus::ErrorCode< OSStatus, errKCWrongKCVersion >                     ErrKCWrongKCVersion;
   typedef Nucleus::ErrorCode< OSStatus, errKCKeySizeNotAllowed >                  ErrKCKeySizeNotAllowed;
   typedef Nucleus::ErrorCode< OSStatus, errKCNoStorageModule >                    ErrKCNoStorageModule;
   typedef Nucleus::ErrorCode< OSStatus, errKCNoCertificateModule >                ErrKCNoCertificateModule;
   typedef Nucleus::ErrorCode< OSStatus, errKCNoPolicyModule >                     ErrKCNoPolicyModule;
   typedef Nucleus::ErrorCode< OSStatus, errKCInteractionRequired >                ErrKCInteractionRequired;
   typedef Nucleus::ErrorCode< OSStatus, errKCDataNotAvailable >                   ErrKCDataNotAvailable;
   typedef Nucleus::ErrorCode< OSStatus, errKCDataNotModifiable >                  ErrKCDataNotModifiable;
   typedef Nucleus::ErrorCode< OSStatus, errKCCreateChainFailed >                  ErrKCCreateChainFailed;


/* UnicodeUtilities error & status codes*/
   typedef Nucleus::ErrorCode< OSStatus, kUCOutputBufferTooSmall >                 UCOutputBufferTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, kUCTextBreakLocatorMissingType >          UCTextBreakLocatorMissingType;

/* Multiprocessing API error codes*/
   typedef Nucleus::ErrorCode< OSStatus, kMPIterationEndErr >                      MPIterationEndErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPPrivilegedErr >                        MPPrivilegedErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPProcessCreatedErr >                    MPProcessCreatedErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPProcessTerminatedErr >                 MPProcessTerminatedErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPTaskCreatedErr >                       MPTaskCreatedErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPTaskBlockedErr >                       MPTaskBlockedErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPTaskStoppedErr >                       MPTaskStoppedErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPBlueBlockingErr >                      MPBlueBlockingErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPDeletedErr >                           MPDeletedErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPTimeoutErr >                           MPTimeoutErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPTaskAbortedErr >                       MPTaskAbortedErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPInsufficientResourcesErr>              MPInsufficientResourcesErr;
   typedef Nucleus::ErrorCode< OSStatus, kMPInvalidIDErr >                         MPInvalidIDErr;

/* StringCompare error codes (in TextUtils range)*/
   typedef Nucleus::ErrorCode< OSStatus, kCollateAttributesNotFoundErr >           CollateAttributesNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kCollateInvalidOptions >                  CollateInvalidOptions;
   typedef Nucleus::ErrorCode< OSStatus, kCollateMissingUnicodeTableErr >          CollateMissingUnicodeTableErr;
   typedef Nucleus::ErrorCode< OSStatus, kCollateUnicodeConvertFailedErr >         CollateUnicodeConvertFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kCollatePatternNotFoundErr >              CollatePatternNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kCollateInvalidChar >                     CollateInvalidChar;
   typedef Nucleus::ErrorCode< OSStatus, kCollateBufferTooSmall >                  CollateBufferTooSmall;
   typedef Nucleus::ErrorCode< OSStatus, kCollateInvalidCollationRef >             CollateInvalidCollationRef;


/* FontSync OSStatus Codes */
   typedef Nucleus::ErrorCode< OSStatus, kFNSInvalidReferenceErr >                 FNSInvalidReferenceErr;
   typedef Nucleus::ErrorCode< OSStatus, kFNSBadReferenceVersionErr >              FNSBadReferenceVersionErr;
   typedef Nucleus::ErrorCode< OSStatus, kFNSInvalidProfileErr >                   FNSInvalidProfileErr;
   typedef Nucleus::ErrorCode< OSStatus, kFNSBadProfileVersionErr >                FNSBadProfileVersionErr;
   typedef Nucleus::ErrorCode< OSStatus, kFNSDuplicateReferenceErr >               FNSDuplicateReferenceErr;
   typedef Nucleus::ErrorCode< OSStatus, kFNSMismatchErr >                         FNSMismatchErr;
   typedef Nucleus::ErrorCode< OSStatus, kFNSInsufficientDataErr >                 FNSInsufficientDataErr;
   typedef Nucleus::ErrorCode< OSStatus, kFNSBadFlattenedSizeErr >                 FNSBadFlattenedSizeErr;
   typedef Nucleus::ErrorCode< OSStatus, kFNSNameNotFoundErr >                     FNSNameNotFoundErr;



/* MacLocales error codes*/
   typedef Nucleus::ErrorCode< OSStatus, kLocalesBufferTooSmallErr >               LocalesBufferTooSmallErr;
   typedef Nucleus::ErrorCode< OSStatus, kLocalesTableFormatErr >                  LocalesTableFormatErr;
   typedef Nucleus::ErrorCode< OSStatus, kLocalesDefaultDisplayStatus >            LocalesDefaultDisplayStatus;


/* Settings Manager (formerly known as Location Manager) Errors */
   typedef Nucleus::ErrorCode< OSStatus, kALMInternalErr >                         ALMInternalErr;
   typedef Nucleus::ErrorCode< OSStatus, kALMGroupNotFoundErr >                    ALMGroupNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kALMNoSuchModuleErr >                     ALMNoSuchModuleErr;
   typedef Nucleus::ErrorCode< OSStatus, kALMModuleCommunicationErr >              ALMModuleCommunicationErr;
   typedef Nucleus::ErrorCode< OSStatus, kALMDuplicateModuleErr >                  ALMDuplicateModuleErr;
   typedef Nucleus::ErrorCode< OSStatus, kALMInstallationErr >                     ALMInstallationErr;
   typedef Nucleus::ErrorCode< OSStatus, kALMDeferSwitchErr >                      ALMDeferSwitchErr;
   typedef Nucleus::ErrorCode< OSStatus, kALMRebootFlagsLevelErr >                 ALMRebootFlagsLevelErr;

   typedef Nucleus::ErrorCode< OSStatus, kALMLocationNotFoundErr >                 ALMLocationNotFoundErr;


/* SoundSprocket Error Codes */
   typedef Nucleus::ErrorCode< OSStatus, kSSpInternalErr >                         SSpInternalErr;
   typedef Nucleus::ErrorCode< OSStatus, kSSpVersionErr >                          SSpVersionErr;
   typedef Nucleus::ErrorCode< OSStatus, kSSpCantInstallErr >                      SSpCantInstallErr;
   typedef Nucleus::ErrorCode< OSStatus, kSSpParallelUpVectorErr >                 SSpParallelUpVectorErr;
   typedef Nucleus::ErrorCode< OSStatus, kSSpScaleToZeroErr >                      SSpScaleToZeroErr;


/* NetSprocket Error Codes */
   typedef Nucleus::ErrorCode< OSStatus, kNSpInitializationFailedErr>              NSpInitializationFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpAlreadyInitializedErr >               NSpAlreadyInitializedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpTopologyNotSupportedErr>              NSpTopologyNotSupportedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpPipeFullErr >                         NSpPipeFullErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpHostFailedErr >                       NSpHostFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpProtocolNotAvailableErr>              NSpProtocolNotAvailableErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpInvalidGameRefErr >                   NSpInvalidGameRefErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpInvalidParameterErr >                 NSpInvalidParameterErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpOTNotPresentErr >                     NSpOTNotPresentErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpOTVersionTooOldErr >                  NSpOTVersionTooOldErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpMemAllocationErr >                    NSpMemAllocationErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpAlreadyAdvertisingErr >               NSpAlreadyAdvertisingErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpNotAdvertisingErr >                   NSpNotAdvertisingErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpInvalidAddressErr >                   NSpInvalidAddressErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpFreeQExhaustedErr >                   NSpFreeQExhaustedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpRemovePlayerFailedErr >               NSpRemovePlayerFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpAddressInUseErr >                     NSpAddressInUseErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpFeatureNotImplementedErr >            NSpFeatureNotImplementedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpNameRequiredErr >                     NSpNameRequiredErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpInvalidPlayerIDErr >                  NSpInvalidPlayerIDErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpInvalidGroupIDErr >                   NSpInvalidGroupIDErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpNoPlayersErr >                        NSpNoPlayersErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpNoGroupsErr >                         NSpNoGroupsErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpNoHostVolunteersErr >                 NSpNoHostVolunteersErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpCreateGroupFailedErr >                NSpCreateGroupFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpAddPlayerFailedErr >                  NSpAddPlayerFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpInvalidDefinitionErr >                NSpInvalidDefinitionErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpInvalidProtocolRefErr >               NSpInvalidProtocolRefErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpInvalidProtocolListErr >              NSpInvalidProtocolListErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpTimeoutErr >                          NSpTimeoutErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpGameTerminatedErr >                   NSpGameTerminatedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpConnectFailedErr >                    NSpConnectFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpSendFailedErr >                       NSpSendFailedErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpMessageTooBigErr >                    NSpMessageTooBigErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpCantBlockErr >                        NSpCantBlockErr;
   typedef Nucleus::ErrorCode< OSStatus, kNSpJoinFailedErr >                       NSpJoinFailedErr;


/* InputSprockets error codes */
   typedef Nucleus::ErrorCode< OSStatus, kISpInternalErr >                         ISpInternalErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpSystemListErr >                       ISpSystemListErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpBufferToSmallErr >                    ISpBufferToSmallErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpElementInListErr >                    ISpElementInListErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpElementNotInListErr >                 ISpElementNotInListErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpSystemInactiveErr >                   ISpSystemInactiveErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpDeviceInactiveErr >                   ISpDeviceInactiveErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpSystemActiveErr >                     ISpSystemActiveErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpDeviceActiveErr >                     ISpDeviceActiveErr;
   typedef Nucleus::ErrorCode< OSStatus, kISpListBusyErr >                         ISpListBusyErr;

/* DrawSprockets error/warning codes */
   typedef Nucleus::ErrorCode< OSStatus, kDSpNotInitializedErr >                   DSpNotInitializedErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpSystemSWTooOldErr >                   DSpSystemSWTooOldErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpInvalidContextErr >                   DSpInvalidContextErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpInvalidAttributesErr >                DSpInvalidAttributesErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpContextAlreadyReservedErr >           DSpContextAlreadyReservedErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpContextNotReservedErr >               DSpContextNotReservedErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpContextNotFoundErr >                  DSpContextNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpFrameRateNotReadyErr >                DSpFrameRateNotReadyErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpConfirmSwitchWarning >                DSpConfirmSwitchWarning;
   typedef Nucleus::ErrorCode< OSStatus, kDSpInternalErr >                         DSpInternalErr;
   typedef Nucleus::ErrorCode< OSStatus, kDSpStereoContextErr >                    DSpStereoContextErr;


/* Find By Content errors */
   typedef Nucleus::ErrorCode< OSStatus, kFBCvTwinExceptionErr >                   FBCvTwinExceptionErr;
   typedef Nucleus::ErrorCode< OSStatus, kFBCnoIndexesFound >                      FBCnoIndexesFound;
   typedef Nucleus::ErrorCode< OSStatus, kFBCallocFailed >                         FBCallocFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCbadParam >                            FBCbadParam;
   typedef Nucleus::ErrorCode< OSStatus, kFBCfileNotIndexed >                      FBCfileNotIndexed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCbadIndexFile >                        FBCbadIndexFile;
   typedef Nucleus::ErrorCode< OSStatus, kFBCcompactionFailed >                    FBCcompactionFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCvalidationFailed >                    FBCvalidationFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCindexingFailed >                      FBCindexingFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCcommitFailed >                        FBCcommitFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCdeletionFailed >                      FBCdeletionFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCmoveFailed >                          FBCmoveFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCtokenizationFailed >                  FBCtokenizationFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCmergingFailed >                       FBCmergingFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCindexCreationFailed >                 FBCindexCreationFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCaccessorStoreFailed >                 FBCaccessorStoreFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCaddDocFailed >                        FBCaddDocFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCflushFailed >                         FBCflushFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCindexNotFound >                       FBCindexNotFound;
   typedef Nucleus::ErrorCode< OSStatus, kFBCnoSearchSession >                     FBCnoSearchSession;
   typedef Nucleus::ErrorCode< OSStatus, kFBCindexingCanceled >                    FBCindexingCanceled;
   typedef Nucleus::ErrorCode< OSStatus, kFBCaccessCanceled >                      FBCaccessCanceled;
   typedef Nucleus::ErrorCode< OSStatus, kFBCindexFileDestroyed >                  FBCindexFileDestroyed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCindexNotAvailable >                   FBCindexNotAvailable;
   typedef Nucleus::ErrorCode< OSStatus, kFBCsearchFailed >                        FBCsearchFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCsomeFilesNotIndexed >                 FBCsomeFilesNotIndexed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCillegalSessionChange >                FBCillegalSessionChange;
   typedef Nucleus::ErrorCode< OSStatus, kFBCanalysisNotAvailable >                FBCanalysisNotAvailable;
   typedef Nucleus::ErrorCode< OSStatus, kFBCbadIndexFileVersion >                 FBCbadIndexFileVersion;
   typedef Nucleus::ErrorCode< OSStatus, kFBCsummarizationCanceled >               FBCsummarizationCanceled;
   typedef Nucleus::ErrorCode< OSStatus, kFBCindexDiskIOFailed >                   FBCindexDiskIOFailed;
   typedef Nucleus::ErrorCode< OSStatus, kFBCbadSearchSession >                    FBCbadSearchSession;
   typedef Nucleus::ErrorCode< OSStatus, kFBCnoSuchHit >                           FBCnoSuchHit;


/* QuickTime VR Errors */
   typedef Nucleus::ErrorCode< OSStatus, notAQTVRMovieErr >                        NotAQTVRMovieErr;
   typedef Nucleus::ErrorCode< OSStatus, constraintReachedErr >                    ConstraintReachedErr;
   typedef Nucleus::ErrorCode< OSStatus, callNotSupportedByNodeErr >               CallNotSupportedByNodeErr;
   typedef Nucleus::ErrorCode< OSStatus, selectorNotSupportedByNodeErr >           SelectorNotSupportedByNodeErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidNodeIDErr >                        InvalidNodeIDErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidViewStateErr >                     InvalidViewStateErr;
   typedef Nucleus::ErrorCode< OSStatus, timeNotInViewErr >                        TimeNotInViewErr;
   typedef Nucleus::ErrorCode< OSStatus, propertyNotSupportedByNodeErr >           PropertyNotSupportedByNodeErr;
   typedef Nucleus::ErrorCode< OSStatus, settingNotSupportedByNodeErr >            SettingNotSupportedByNodeErr;
   typedef Nucleus::ErrorCode< OSStatus, limitReachedErr >                         LimitReachedErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidNodeFormatErr >                    InvalidNodeFormatErr;
   typedef Nucleus::ErrorCode< OSStatus, invalidHotSpotIDErr >                     InvalidHotSpotIDErr;
   typedef Nucleus::ErrorCode< OSStatus, noMemoryNodeFailedInitialize >            NoMemoryNodeFailedInitialize;
   typedef Nucleus::ErrorCode< OSStatus, streamingNodeNotReadyErr >                StreamingNodeNotReadyErr;
   typedef Nucleus::ErrorCode< OSStatus, qtvrLibraryLoadErr >                      QTVRLibraryLoadErr;
   typedef Nucleus::ErrorCode< OSStatus, qtvrUninitialized >                       QTVRUninitialized;


/* Appearance Manager Error Codes */
   typedef Nucleus::ErrorCode< OSStatus, themeInvalidBrushErr >                    ThemeInvalidBrushErr;
   typedef Nucleus::ErrorCode< OSStatus, themeProcessRegisteredErr >               ThemeProcessRegisteredErr;
   typedef Nucleus::ErrorCode< OSStatus, themeProcessNotRegisteredErr >            ThemeProcessNotRegisteredErr;
   typedef Nucleus::ErrorCode< OSStatus, themeBadTextColorErr >                    ThemeBadTextColorErr;
   typedef Nucleus::ErrorCode< OSStatus, themeHasNoAccentsErr >                    ThemeHasNoAccentsErr;
   typedef Nucleus::ErrorCode< OSStatus, themeBadCursorIndexErr >                  ThemeBadCursorIndexErr;
   typedef Nucleus::ErrorCode< OSStatus, themeScriptFontNotFoundErr >              ThemeScriptFontNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, themeMonitorDepthNotSupportedErr >        ThemeMonitorDepthNotSupportedErr;

/* Control Manager Error Codes */
   typedef Nucleus::ErrorCode< OSStatus, errMessageNotSupported >                  ErrMessageNotSupported;
   typedef Nucleus::ErrorCode< OSStatus, errDataNotSupported >                     ErrDataNotSupported;
   typedef Nucleus::ErrorCode< OSStatus, errControlDoesntSupportFocus >            ErrControlDoesntSupportFocus;
   typedef Nucleus::ErrorCode< OSStatus, errUnknownControl >                       ErrUnknownControl;
   typedef Nucleus::ErrorCode< OSStatus, errCouldntSetFocus >                      ErrCouldntSetFocus;
   typedef Nucleus::ErrorCode< OSStatus, errNoRootControl >                        ErrNoRootControl;
   typedef Nucleus::ErrorCode< OSStatus, errRootAlreadyExists >                    ErrRootAlreadyExists;
   typedef Nucleus::ErrorCode< OSStatus, errInvalidPartCode >                      ErrInvalidPartCode;
   typedef Nucleus::ErrorCode< OSStatus, errControlsAlreadyExist >                 ErrControlsAlreadyExist;
   typedef Nucleus::ErrorCode< OSStatus, errControlIsNotEmbedder >                 ErrControlIsNotEmbedder;
   typedef Nucleus::ErrorCode< OSStatus, errDataSizeMismatch >                     ErrDataSizeMismatch;
   typedef Nucleus::ErrorCode< OSStatus, errControlHiddenOrDisabled >              ErrControlHiddenOrDisabled;
   typedef Nucleus::ErrorCode< OSStatus, errCantEmbedIntoSelf >                    ErrCantEmbedIntoSelf;
   typedef Nucleus::ErrorCode< OSStatus, errCantEmbedRoot >                        ErrCantEmbedRoot;
   typedef Nucleus::ErrorCode< OSStatus, errItemNotControl >                       ErrItemNotControl;
   typedef Nucleus::ErrorCode< OSStatus, controlInvalidDataVersionErr >            ControlInvalidDataVersionErr;
   typedef Nucleus::ErrorCode< OSStatus, controlPropertyInvalid >                  ControlPropertyInvalid;
   typedef Nucleus::ErrorCode< OSStatus, controlPropertyNotFoundErr >              ControlPropertyNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, controlHandleInvalidErr >                 ControlHandleInvalidErr;

/* URLAccess Error Codes */
   typedef Nucleus::ErrorCode< OSStatus, kURLInvalidURLReferenceError >            URLInvalidURLReferenceError;
   typedef Nucleus::ErrorCode< OSStatus, kURLProgressAlreadyDisplayedError >       URLProgressAlreadyDisplayedError;
   typedef Nucleus::ErrorCode< OSStatus, kURLDestinationExistsError >              URLDestinationExistsError;
   typedef Nucleus::ErrorCode< OSStatus, kURLInvalidURLError >                     URLInvalidURLError;
   typedef Nucleus::ErrorCode< OSStatus, kURLUnsupportedSchemeError >              URLUnsupportedSchemeError;
   typedef Nucleus::ErrorCode< OSStatus, kURLServerBusyError >                     URLServerBusyError;
   typedef Nucleus::ErrorCode< OSStatus, kURLAuthenticationError >                 URLAuthenticationError;
   typedef Nucleus::ErrorCode< OSStatus, kURLPropertyNotYetKnownError >            URLPropertyNotYetKnownError;
   typedef Nucleus::ErrorCode< OSStatus, kURLUnknownPropertyError >                URLUnknownPropertyError;
   typedef Nucleus::ErrorCode< OSStatus, kURLPropertyBufferTooSmallError >         URLPropertyBufferTooSmallError;
   typedef Nucleus::ErrorCode< OSStatus, kURLUnsettablePropertyError>              URLUnsettablePropertyError;
   typedef Nucleus::ErrorCode< OSStatus, kURLInvalidCallError >                    URLInvalidCallError;
   typedef Nucleus::ErrorCode< OSStatus, kURLFileEmptyError >                      URLFileEmptyError;
   typedef Nucleus::ErrorCode< OSStatus, kURLExtensionFailureError >               URLExtensionFailureError;
   typedef Nucleus::ErrorCode< OSStatus, kURLInvalidConfigurationError >           URLInvalidConfigurationError;
   typedef Nucleus::ErrorCode< OSStatus, kURLAccessNotAvailableError>              URLAccessNotAvailableError;
   typedef Nucleus::ErrorCode< OSStatus, kURL68kNotSupportedError >                URL68kNotSupportedError;

/* ComponentError codes*/
   typedef Nucleus::ErrorCode< OSStatus, badComponentInstance >                    BadComponentInstance;
   typedef Nucleus::ErrorCode< OSStatus, badComponentSelector >                    BadComponentSelector;
  }

#endif
