/*	=========
 *	AppleCD.h
 *	=========
 */

#ifndef CLASSICEXTRAS_APPLECD_H
#define CLASSICEXTRAS_APPLECD_H


namespace NitrogenExtras
{
	enum
	{
		kAppleCDReadTOC          = 100,
		kAppleCDReadTheQSubcode  = 101,
		kAppleCDAudioTrackSearch = 103,
		kAppleCDAudioPlay        = 104,
		kAppleCDAudioPause       = 105,
		kAppleCDAudioStop        = 106,
		kAppleCDAudioStatus      = 107
	};
	
#if CALL_NOT_IN_CARBON
	
	// this is a custom version of the CntrlParam parameter
	// block defined on page II-181 and II-183 or in
	// Devices.h
	
	#if PRAGMA_STRUCT_ALIGN
		#pragma options align=mac68k
	#endif
	
	struct CDROMControlParameterBlockBase
	{
		QElemPtr   qLink;
		short      qType;
		short      ioTrap;
		Ptr        ioCmdAddr;
		ProcPtr    ioCompletion;
		OSErr      ioResult;
		StringPtr  ioNamePtr;
		short      ioVRefNum;
		short      ioCRefNum;
		short      csCode;
	};
	
	struct TOCEntry
	{
		char controlField;
		char trackNum;
		char min;
		char sec;
		char frame;
	};
	
	struct ReadTOCType1
	{
		char firstTrack;
		char lastTrack;
	};
	
	struct ReadTOCType2
	{
		char min;
		char sec;
		char frame;
	};
	
	struct ReadTOCType4
	{
		short xferCode;
		void* bufPtr;
	};
	
	union ReadTOCControlParameters
	{
		short xferCode;
		ReadTOCType1 type1;
		ReadTOCType2 type2;
		ReadTOCType4 type4;
	};
	
	struct ReadTheQSubcodeControlParameters
	{
		unsigned char control;
		unsigned char track;
		unsigned char index;
		unsigned char min;
		unsigned char sec;
		unsigned char frame;
		unsigned char absMin;
		unsigned char absSec;
		unsigned char absFrame;
	};
	
	struct AudioTrackSearchControlParameters
	{
		unsigned short opticalPositioningType;
		unsigned long address;
		unsigned short startPlaying;
		unsigned short playMode;
	};
	
	struct AudioPlayControlParameters
	{
		unsigned short opticalPositioningType;
		unsigned long address;
		unsigned short isStop;
		unsigned short playMode;
	};
	
	struct AudioPauseControlParameters
	{
		unsigned long pausing;
	};
	
	struct AudioStopControlParameters
	{
		unsigned short opticalPositioningType;
		unsigned long address;
	};
	
	struct AudioStatusControlParameters
	{
		unsigned char status;
		unsigned char playMode;
		unsigned char control;
		unsigned char minutes;
		unsigned char seconds;
		unsigned char frames;
	};
	
	union AudioCDControlParameters
	{
		short                              csParam[ 11 ];
		ReadTOCControlParameters           ReadTOC;
		ReadTheQSubcodeControlParameters   ReadTheQSubcode;
		AudioTrackSearchControlParameters  AudioTrackSearch;
		AudioPlayControlParameters         AudioPlay;
		AudioPauseControlParameters        AudioPause;
		AudioStopControlParameters         AudioStop;
		AudioStatusControlParameters       AudioStatus;
	};
	
	struct AudioCDControlParameterBlock : CDROMControlParameterBlockBase,
	                                      AudioCDControlParameters
	{
	};
	
	#if PRAGMA_STRUCT_ALIGN
		#pragma options align=reset
	#endif
	
	
#endif
	
}

#endif

