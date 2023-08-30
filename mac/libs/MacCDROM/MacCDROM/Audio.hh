/*
	MacCDROM/Audio.hh
	-----------------
	
	Joshua Juran
*/

#ifndef MACCDROM_AUDIO_HH
#define MACCDROM_AUDIO_HH

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif

// MacCDROM
#include "MacCDROM/Device.hh"


namespace MacCDROM
{
	
	enum OpticalPositioningType
	{
		kOpticalPositioningLogicalBlockAddress = 0x0000,
		kOpticalPositioningRunningTimeBCD      = 0x0001,
		kOpticalPositioningTrackNumberBCD      = 0x0002,
		kOpticalPositioningPlayListIndex       = 0x0003,
		
		kOpticalPositioningType_Max = nucleus::enumeration_traits< ::UInt16 >::max
	};
	
	enum AudioStatusCode
	{
		kAudioStatusPlaying = 0,
		kAudioStatusPaused  = 1,
		kAudioStatusMuteOn  = 2,
		kAudioStatusDone    = 3,
		kAudioStatusError   = 4,
		kAudioStatusNil     = 5,
		
		kAudioStatusCode_Max = nucleus::enumeration_traits< ::UInt8 >::max
	};
	
	enum AudioPlayMode
	{
		kAudioPlayModeMute = 0,
		
		kAudioPlayModeRightThruRight = 1 << 0,
		kAudioPlayModeLeftThruRight  = 1 << 1,
		kAudioPlayModeRightThruLeft  = 1 << 2,
		kAudioPlayModeLeftThruLeft   = 1 << 3,
		
		kAudioPlayModeStereo = kAudioPlayModeLeftThruLeft
		                     | kAudioPlayModeRightThruRight,
		
		kAudioPlayModeMonaural = kAudioPlayModeLeftThruLeft
		                       | kAudioPlayModeRightThruLeft
		                       | kAudioPlayModeLeftThruRight
		                       | kAudioPlayModeRightThruRight,
		
		kAudioPlayMode_Max = nucleus::enumeration_traits< ::UInt8 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( AudioPlayMode )
	
	enum TrackControl
	{
		kTrackControlTrackFormatMask = 0x0D,
		kTrackControlDigitalCopyMask = 0x02,
		
		kTrackControl2AudioChannels = 0x00,
		kTrackControl4AudioChannels = 0x08,
		
		kTrackControlNoPreemphasis = 0x00,
		kTrackControlPreemphasis   = 0x01,
		
		kTrackControlData = 0x04,
		
		kTrackControlDigitalCopyProhibited = 0x00,
		kTrackControlDigitalCopyPermitted = 0x02,
		
		kTrackControl_Max = nucleus::enumeration_traits< ::UInt8 >::max
	};
	
	typedef unsigned long Frames;
	
	class CDTime
	{
		private:
			unsigned char min;
			unsigned char sec;
			unsigned char blk;
		
		public:
			CDTime() : min( 0 ),
			           sec( 0 ),
			           blk( 0 )
			{}
			
			CDTime( unsigned char min,
			        unsigned char sec,
			        unsigned char blk ) : min( min ),
			                              sec( sec ),
			                              blk( blk )
			{}
			
			CDTime( Frames blocks ) : min( blocks / 75 / 60 ),
			                          sec( blocks / 75 % 60 ),
			                          blk( blocks % 75 )
			{}
			
	};
	
	
	typedef unsigned short TrackNumber, TrackCount;
	
	struct CDROMTableOfContents
	{
		// first track is always 1
		TrackNumber  lastTrack;
		TrackNumber  lastAudioTrack;
		Frames       a2;
		Frames       leadOut;
		
		Frames       trackStarts[ 99 ];
	};
	
	inline TrackCount CountTracks     ( const CDROMTableOfContents& toc )  { return toc.lastTrack;      }
	inline TrackCount CountAudioTracks( const CDROMTableOfContents& toc )  { return toc.lastAudioTrack; }
	
	Frames TrackStart( const CDROMTableOfContents& toc, TrackNumber track );
	Frames TrackEnd  ( const CDROMTableOfContents& toc, TrackNumber track );
	
	inline Frames TrackLength( const CDROMTableOfContents& toc, TrackNumber track )
	{
		return TrackEnd( toc, track ) - TrackStart( toc, track );
	}
	
	inline Frames LeadIn( const CDROMTableOfContents& toc )  { return TrackStart( toc, 1 ); }
	
	inline Frames LeadOut( const CDROMTableOfContents& toc )  { return toc.leadOut; }
	
	inline Frames AudioLeadOut( const CDROMTableOfContents& toc )  { return toc.a2; }
	
	inline Frames DiscLength( const CDROMTableOfContents& toc )  { return LeadOut( toc ); }
	
	inline Frames DiscContentLength( const CDROMTableOfContents& toc )
	{
		return LeadOut( toc ) - LeadIn( toc );
	}
	
	inline Frames DiscAudioLength( const CDROMTableOfContents& toc )
	{
		return AudioLeadOut( toc ) - LeadIn( toc );
	}
	
	unsigned int CDDBDiscID( const CDROMTableOfContents& toc );
	
#if CALL_NOT_IN_CARBON
	
	#pragma mark -
	#pragma mark ** Audio track Control Calls **
	
	// csCode 100
	CDROMTableOfContents ReadTOC( const CDROMDrive& drive );
	
	struct ReadTheQSubcode_Result
	{
		TrackControl control;
		TrackNumber track;
		unsigned char index;
		CDTime trackTime;
		CDTime discTime;
	};
	
	// csCode 101
	ReadTheQSubcode_Result ReadTheQSubcode( const CDROMDrive& drive );
	
	// csCode 102
	// ReadHeader
	
	// csCode 103
	void AudioTrackSearch( const CDROMDrive&       drive,
	                       OpticalPositioningType  positioningType,
	                       unsigned long           address,
	                       bool                    startPlaying,
	                       AudioPlayMode           playMode );
	
	void AudioTrackSearch( const CDROMDrive&       drive,
	                       TrackNumber             track,
	                       bool                    startPlaying,
	                       AudioPlayMode           playMode );
	
	// csCode 104
	void AudioPlay( const CDROMDrive&       drive,
	                OpticalPositioningType  positioningType,
	                unsigned long           address,
	                bool                    stopping,
	                AudioPlayMode           playMode );
	
	void AudioPlayAtTime( const CDROMDrive&  drive,
	                      Frames             runningTime,
	                      bool               stopping,
	                      AudioPlayMode      playMode );
	
	void AudioPlayAtTrack( const CDROMDrive&  drive,
	                       TrackNumber        track,
	                       bool               stopping,
	                       AudioPlayMode      playMode );
	
	// csCode 105
	void AudioPause( const CDROMDrive& drive, bool pausing );
	
	// csCode 106
	void AudioStop( const CDROMDrive& drive );
	
	void AudioStopAtTrack( const CDROMDrive& drive, TrackNumber track );
	
	struct AudioStatus_Result
	{
		AudioStatusCode  status;
		AudioPlayMode    playMode;
		TrackControl     control;
		unsigned char    minutes;
		unsigned char    seconds;
		unsigned char    frames;
	};
	
	// csCode 107
	AudioStatus_Result AudioStatus( const CDROMDrive& drive );
	
	// csCode 108
	// AudioScan
	
	// csCode 109
	// AudioControl
	
	// csCode 110
	// ReadMCN
	
	// csCode 111
	// ReadISRC
	
	// csCode 112
	// ReadAudioVolume
	
	// csCode 113
	// GetSpindleSpeed
	
	// csCode 114
	// SetSpindleSpeed
	
	// csCode 115
	// ReadAudio
	
	// csCode 116
	// ReadAllSubCodes
	
	#pragma mark -
	#pragma mark ** Audio track list Control Calls **
	
	// csCode 122
	// SetTrackList
	
	// csCode 123
	// GetTrackList
	
	// csCode 124
	// SetTrackIndex
	
	// csCode 125
	// SetPlayMode
	
	// csCode 126
	// GetPlayMode
	
#endif
	
}

#endif
