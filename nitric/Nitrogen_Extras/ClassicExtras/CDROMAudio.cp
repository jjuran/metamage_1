/*	=============
 *	CDROMAudio.cp
 *	=============
 */

#ifndef __MACH__
#ifndef CLASSICEXTRAS_CDROMAUDIO_H
#include "ClassicExtras/CDROMAudio.h"
#endif
#endif

// Nitrogen Core
#include "Nitrogen/Assert.h"

// Nitrogen Extras / ClassicExtras
#ifndef CLASSICEXTRAS_APPLECD_H
#include "ClassicExtras/AppleCD.h"
#endif


namespace NitrogenExtras
{
	
	static int cddb_sum( int n )
	{
		/* For backward compatibility this algorithm must not change */

		int result = 0;

		while ( n > 0 )
		{
			result += ( n % 10 );
			n /= 10;
		}

		return result;
	}
	
	unsigned int CDDBDiscID( const CDROMTableOfContents& toc )
	{
		TrackCount tracks = CountTracks( toc );
		
		int sum = 0;
		
		for ( int track = 1;  track <= tracks;  ++track )
		{
			int startFrames = TrackStart( toc, track );
			
			sum += cddb_sum( startFrames / 75 );
		}
		
		int totalTime = DiscContentLength( toc ) / 75;
		
		return   (sum % 0xFF) << 24
		       | totalTime    <<  8
		       | tracks;
	}
	
	// E.g. Given 0x12, return 0x0C.
	inline int DecodeBCD( unsigned char bcd )
	{
		unsigned char high = bcd >> 4;
		unsigned char low  = bcd & 0x0F;
		
		ASSERT( high < 10 );
		ASSERT( low  < 10 );
		
		return high * 10 + low;
	}
	
	inline unsigned char EncodeBCD( int i )
	{
		ASSERT( i >=  0 );
		ASSERT( i <= 99 );
		
		return   i / 10 << 4
		       | i % 10;
	}
	
	inline unsigned long EncodeBCD( int a,
	                                int b,
	                                int c )
	{
		return   EncodeBCD( a ) << 16
		       | EncodeBCD( b ) <<  8
		       | EncodeBCD( c );
	}
	
	inline unsigned long EncodeTimeBCD( Frames time )
	{
		return EncodeBCD( time / 75 / 60,
		                  time / 75 % 60,
		                  time % 75 );
	}
	
	enum
	{
		// For exposition -- these values will never change,
		// and the code is clearer with the actual numbers.
		secondsPerMinute = 60,
		framesPerSecond = 75
	};
	
	inline Frames CountFrames( int minutes, int seconds, int frames )
	{
		return ( (minutes * 60) + seconds ) * 75 + frames;
	}
	
	static Frames CountFramesBCD( unsigned char minutes,
	                              unsigned char seconds,
	                              unsigned char frames )
	{
		return CountFrames( DecodeBCD( minutes ),
		                    DecodeBCD( seconds ),
		                    DecodeBCD( frames  ) );
	}
	
	static Frames CountFramesBCD( const TOCEntry& entry )
	{
		return CountFramesBCD( entry.min, entry.sec, entry.frame );
	}
	
	Frames TrackStart( const CDROMTableOfContents& toc, TrackNumber track )
	{
		ASSERT( track > 0 );
		ASSERT( track <= toc.lastTrack );
		
		return toc.trackStarts[ track - 1 ];
	}
	
	Frames TrackEnd( const CDROMTableOfContents& toc, TrackNumber track )
	{
		ASSERT( track > 0 );
		ASSERT( track <= toc.lastTrack );
		
		if ( track == toc.lastAudioTrack )
		{
			return AudioLeadOut( toc );
		}
		else if ( track == toc.lastTrack )
		{
			return LeadOut( toc );
		}
		else
		{
			return TrackStart( toc, track + 1 );
		}
	}
	
#if CALL_NOT_IN_CARBON
	
	static void Initialize( AudioCDControlParameterBlock& pb )
	{
		std::memset( &pb, 0, sizeof pb );
	}
	
	CDROMTableOfContents ReadTOC( const CDROMDrive& drive )
	{
		CDROMTableOfContents toc;
		AudioCDControlParameterBlock pb;
		
		Initialize( pb );
		
		pb.ioCompletion = NULL;
		pb.ioVRefNum = drive.vRefNum;
		pb.ioCRefNum = drive.dRefNum;
		pb.csCode = kAppleCDReadTOC;
		
		// ReadTOC transfer type 1:
		//     Return track numbers of 
		//       * first track of first session
		//       * last track of last session
		
		pb.ReadTOC.xferCode = 1;
		N::PBControlSync( (ParamBlockRec&)pb );
		
		int firstTrack = DecodeBCD( pb.ReadTOC.type1.firstTrack );
		toc.lastTrack  = DecodeBCD( pb.ReadTOC.type1.lastTrack  );
		
		ASSERT( firstTrack    ==  1 );
		ASSERT( toc.lastTrack <= 99 );
		
		// ReadTOC transfer type 4
		//     Return address entries for 
		//       * point A0 (number of first audio track)
		//       * point A1 (number of last audio track)
		//       * point A2 (address of audio lead-out)
		//       * address of first track
		//       * ...
		//       * address of last track (including data)
		
		char tocBuffer[ 512 ];
		
		pb.ReadTOC.xferCode = 4;
		pb.ReadTOC.type4.bufPtr = tocBuffer;
		N::PBControlSync( (ParamBlockRec&)pb );
		
		typedef TOCEntry RawEntry;
		
		const char* point = tocBuffer + 1;
		const RawEntry* entry;
		
		entry = reinterpret_cast< const RawEntry* >( point );
		int firstAudioTrack = DecodeBCD( entry->min );
		ASSERT( firstAudioTrack == 1 );
		
		entry = reinterpret_cast< const RawEntry* >( point + 5 );
		toc.lastAudioTrack = DecodeBCD( entry->min );
		ASSERT( toc.lastAudioTrack <= toc.lastTrack );
		
		toc.trackStarts.resize( toc.lastTrack );
		
		entry = reinterpret_cast< const RawEntry* >( point + 5*2 );
		toc.a2 = CountFramesBCD( *entry );
		
		for ( int track = firstTrack;  track <= toc.lastTrack;  ++track )
		{
			entry = reinterpret_cast< const RawEntry* >( point + 5 * (2 + track) );
			toc.trackStarts[ track - 1 ] = CountFramesBCD( *entry );
		}
		
		if ( toc.lastAudioTrack == toc.lastTrack )
		{
			// plain audio CD
			toc.leadOut = toc.a2;
		}
		else
		{
			// need to get the leadout of the last session
			pb.ReadTOC.xferCode = 2;
			N::PBControlSync( (ParamBlockRec&)pb );
			
			toc.leadOut = CountFramesBCD( pb.ReadTOC.type2.min,
			                              pb.ReadTOC.type2.sec,
			                              pb.ReadTOC.type2.frame );
		}
		
		return toc;
	}
	
	ReadTheQSubcode_Result ReadTheQSubcode( const CDROMDrive& drive )
	{
		AudioCDControlParameterBlock pb;
		
		Initialize( pb );
		
		pb.ioCompletion = 0;
		pb.ioVRefNum = drive.vRefNum;
		pb.ioCRefNum = drive.dRefNum;
		pb.csCode = kAppleCDReadTheQSubcode;
		
		N::PBControlSync( (ParamBlockRec&)pb );
		
		ReadTheQSubcode_Result q;
		
		q.control = pb.ReadTheQSubcode.control;
		q.track = pb.ReadTheQSubcode.track;
		q.index = pb.ReadTheQSubcode.index;
		
		q.trackTime = CDTime( pb.ReadTheQSubcode.min,
		                      pb.ReadTheQSubcode.sec,
		                      pb.ReadTheQSubcode.frame );
		
		q.discTime = CDTime( pb.ReadTheQSubcode.absMin,
		                     pb.ReadTheQSubcode.absSec,
		                     pb.ReadTheQSubcode.absFrame );
		
		return q;
	}
	
	void AudioPlay( const CDROMDrive&       drive,
	                OpticalPositioningType  positioningType,
	                unsigned long           address,
	                bool                    stopping,
	                AudioPlayMode           playMode )
	{
		AudioCDControlParameterBlock pb;
		
		Initialize( pb );
		
		pb.ioCompletion = 0;
		pb.ioVRefNum = drive.vRefNum;
		pb.ioCRefNum = drive.dRefNum;
		pb.csCode = kAppleCDAudioPlay;
		
		pb.AudioPlay.opticalPositioningType = positioningType;
		pb.AudioPlay.address = address;
		pb.AudioPlay.isStop = stopping;
		pb.AudioPlay.playMode = playMode;
		
		N::PBControlSync( (ParamBlockRec&)pb );
	}
	
	void AudioPlayAtTime( const CDROMDrive&  drive,
	                      Frames             runningTime,
	                      bool               stopping,
	                      AudioPlayMode      playMode )
	{
		AudioPlay( drive,
		           kOpticalPositioningRunningTimeBCD,
		           EncodeTimeBCD( runningTime ),
		           stopping,
		           playMode );
	}
	
	void AudioPlayAtTrack( const CDROMDrive&  drive,
	                       TrackNumber        track,
	                       bool               stopping,
	                       AudioPlayMode      playMode )
	{
		AudioPlay( drive,
		           kOpticalPositioningTrackNumberBCD,
		           EncodeBCD( track ),
		           stopping,
		           playMode );
	}
	
	void AudioPause( const CDROMDrive& drive, bool pausing )
	{
		AudioCDControlParameterBlock pb;
		
		Initialize( pb );
		
		pb.ioCompletion = 0;
		pb.ioVRefNum = drive.vRefNum;
		pb.ioCRefNum = drive.dRefNum;
		pb.csCode = kAppleCDAudioPause;
		
		pb.AudioPause.pausing = pausing;
		
		N::PBControlSync( (ParamBlockRec&)pb );
	}
	
	void AudioStop( const CDROMDrive& drive )
	{
		AudioCDControlParameterBlock pb;
		
		Initialize( pb );
		
		pb.ioCompletion = 0;
		pb.ioVRefNum = drive.vRefNum;
		pb.ioCRefNum = drive.dRefNum;
		pb.csCode = kAppleCDAudioStop;
		
		pb.AudioStop.opticalPositioningType = 0;
		pb.AudioStop.address                = 0;
		
		N::PBControlSync( (ParamBlockRec&)pb );
	}
	
	void AudioStopAtTrack( const CDROMDrive& drive, TrackNumber track )
	{
		AudioCDControlParameterBlock pb;
		
		Initialize( pb );
		
		pb.ioCompletion = 0;
		pb.ioVRefNum = drive.vRefNum;
		pb.ioCRefNum = drive.dRefNum;
		pb.csCode = kAppleCDAudioStop;
		
		pb.AudioStop.opticalPositioningType = kOpticalPositioningTrackNumberBCD;
		pb.AudioStop.address                = EncodeBCD( track );
		
		N::PBControlSync( (ParamBlockRec&)pb );
	}
	
	AudioStatus_Result AudioStatus( const CDROMDrive& drive )
	{
		AudioCDControlParameterBlock pb;
		
		Initialize( pb );
		
		pb.ioCompletion = 0;
		pb.ioVRefNum = drive.vRefNum;
		pb.ioCRefNum = drive.dRefNum;
		pb.csCode = kAppleCDAudioStatus;
		
		N::PBControlSync( (ParamBlockRec&)pb );
		
		AudioStatus_Result result;
		
		result.status   = pb.AudioStatus.status;
		result.playMode = pb.AudioStatus.playMode;
		result.control  = pb.AudioStatus.control;
		result.minutes  = pb.AudioStatus.minutes;
		result.seconds  = pb.AudioStatus.seconds;
		result.frames   = pb.AudioStatus.frames;
		
		return result;
	}
	
#endif  // CALL_NOT_IN_CARBON
	
}

