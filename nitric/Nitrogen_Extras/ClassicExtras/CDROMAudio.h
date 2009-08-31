/*	============
 *	CDROMAudio.h
 *	============
 */

#ifndef CLASSICEXTRAS_CDROMAUDIO_H
#define CLASSICEXTRAS_CDROMAUDIO_H

// Nucleus
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif

// Nitrogen Extras / ClassicExtras
#ifndef CLASSICEXTRAS_CDROM_H
#include "ClassicExtras/CDROM.h"
#endif


namespace NitrogenExtras
{
	
	namespace NN = Nucleus;
	
	typedef NN::Selector< struct OpticalPositioningType_Tag, ::UInt16 >::Type OpticalPositioningType;
	
	typedef NN::Selector< struct AudioStatusCode_Tag, ::UInt8 >::Type AudioStatusCode;
	
	typedef NN::Flag< struct AudioPlayMode_Tag, ::UInt8 >::Type  AudioPlayMode;
	//typedef NN::Flag< struct AudioPlayMode_Tag, ::UInt8 >::Bit   AudioPlayModeBit;
	
	NUCLEUS_DEFINE_FLAG_OPS( AudioPlayMode )
	
	typedef NN::Selector< struct TrackControl_Tag, ::UInt8 >::Type TrackControl;
	
	namespace Constants
	{
		
		static const OpticalPositioningType kOpticalPositioningLogicalBlockAddress = OpticalPositioningType( 0x0000 );
		static const OpticalPositioningType kOpticalPositioningRunningTimeBCD      = OpticalPositioningType( 0x0001 );
		static const OpticalPositioningType kOpticalPositioningTrackNumberBCD      = OpticalPositioningType( 0x0002 );
		static const OpticalPositioningType kOpticalPositioningPlayListIndex       = OpticalPositioningType( 0x0003 );
		
		static const AudioStatusCode kAudioStatusPlaying = AudioStatusCode( 0 );
		static const AudioStatusCode kAudioStatusPaused  = AudioStatusCode( 1 );
		static const AudioStatusCode kAudioStatusMuteOn  = AudioStatusCode( 2 );
		static const AudioStatusCode kAudioStatusDone    = AudioStatusCode( 3 );
		static const AudioStatusCode kAudioStatusError   = AudioStatusCode( 4 );
		static const AudioStatusCode kAudioStatusNil     = AudioStatusCode( 5 );
		
		/*
		static const AudioPlayModeBit kAudioPlayModeRightThruRightBit = AudioPlayModeBit( 0 );
		static const AudioPlayModeBit kAudioPlayModeLeftThruRightBit  = AudioPlayModeBit( 1 );
		static const AudioPlayModeBit kAudioPlayModeRightThruLeftBit  = AudioPlayModeBit( 2 );
		static const AudioPlayModeBit kAudioPlayModeLeftThruLeftBit   = AudioPlayModeBit( 3 );
		*/
		
		static const AudioPlayMode kAudioPlayModeMute = AudioPlayMode( 0 );
		
		static const AudioPlayMode kAudioPlayModeRightThruRight = AudioPlayMode( 1 << 0 );
		static const AudioPlayMode kAudioPlayModeLeftThruRight  = AudioPlayMode( 1 << 1 );
		static const AudioPlayMode kAudioPlayModeRightThruLeft  = AudioPlayMode( 1 << 2 );
		static const AudioPlayMode kAudioPlayModeLeftThruLeft   = AudioPlayMode( 1 << 3 );
		
		static const AudioPlayMode kAudioPlayModeStereo = AudioPlayMode(   kAudioPlayModeLeftThruLeft
		                                                                 + kAudioPlayModeRightThruRight );
		
		static const AudioPlayMode kAudioPlayModeMonaural = AudioPlayMode(   kAudioPlayModeLeftThruLeft
		                                                                   + kAudioPlayModeRightThruLeft
		                                                                   + kAudioPlayModeLeftThruRight
		                                                                   + kAudioPlayModeRightThruRight );
		
		static const TrackControl kTrackControlTrackFormatMask = TrackControl( 0x0D );
		static const TrackControl kTrackControlDigitalCopyMask = TrackControl( 0x02 );
		
		static const TrackControl kTrackControl2AudioChannels = TrackControl( 0x00 );
		static const TrackControl kTrackControl4AudioChannels = TrackControl( 0x08 );
		
		static const TrackControl kTrackControlNoPreemphasis = TrackControl( 0x00 );
		static const TrackControl kTrackControlPreemphasis   = TrackControl( 0x01 );
		
		static const TrackControl kTrackControlData = TrackControl( 0x04 );
		
		static const TrackControl kTrackControlDigitalCopyProhibited = TrackControl( 0x00 );
		static const TrackControl kTrackControlDigitalCopyPermitted  = TrackControl( 0x02 );
		
	}
	
	using namespace Constants;
	
	typedef std::size_t Frames;
	
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
		
		std::vector< Frames > trackStarts;
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
	#pragma mark ¥ Audio track Control Calls ¥
	
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
	#pragma mark ¥ Audio track list Control Calls ¥
	
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

