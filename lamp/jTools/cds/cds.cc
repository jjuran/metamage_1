/*	======
 *	cds.cc
 *	======
 */

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/functions/write.hh"

// Nitrogen Extras / ClassicExtras
#include "ClassicExtras/CDROMAudio.h"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
#if !TARGET_API_MAC_CARBON
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	
	using namespace NX::Constants;
	
	
	static NX::CDROMDrive            gDrive;
	static NX::TrackNumber           gSelectedTrack = 1;
	static NX::CDROMTableOfContents  gTOC;
	static NX::AudioStatus_Result    gStatus;
	static NX::AudioPlayMode         gPlayMode = kAudioPlayModeStereo;
	
	
	static void PrintHelp()
	{
		
	}
	
	static void PrintStatus()
	{
		const char* state = "ERROR";
		
		switch ( gStatus.status )
		{
			case kAudioStatusPlaying:  state = "playing";    break;
			case kAudioStatusPaused :  state = "paused";     break;
			case kAudioStatusMuteOn :  state = "muting-on";  break;
			case kAudioStatusDone   :  state = "done";       break;
			case kAudioStatusError  :  state = "error";      break;
			case kAudioStatusNil    :  state = "nil";        break;
		}
		
		std::printf( "Audio status: %s\n", state );
		
		std::printf( "Audio play mode: %d\n", gStatus.playMode & 0x0f );
		std::printf( "Track format:    %d\n", gStatus.control  & 0x0f );
		
		char time[] = "mm:ss";
		
		time[ 0 ] = '0' | gStatus.minutes >> 4;
		time[ 1 ] = '0' | gStatus.minutes & 0x0F;
		
		time[ 3 ] = '0' | gStatus.seconds >> 4;
		time[ 4 ] = '0' | gStatus.seconds & 0x0F;
		
		std::printf( "Time: %s\n", time );
	}
	
	static void PrintInfo()
	{
		
	}
	
	static void PrintList()
	{
		NX::TrackCount tracks = NX::CountTracks( gTOC );
		
		for ( int track = 1;  track <= tracks;  ++track )
		{
			int frames  = NX::TrackLength( gTOC, track );
			int seconds = frames / 75;
			
			int minutes = seconds / 60;
			
			seconds %= 60;
			
			std::printf( "Track %d: %.2d:%.2d\n", track, minutes, seconds );
		}
	}
	
	static void PrintDiscID()
	{
		unsigned int discID = NX::CDDBDiscID( gTOC );
		
		std::printf( "Disc ID is %.8x\n", discID );
		
		NX::TrackCount tracks = NX::CountTracks( gTOC );
		
		std::string command = "discid ";
		
		command += NN::Convert< std::string >( tracks );
		
		for ( int track = 1;  track <= tracks;  ++track )
		{
			int offset = NX::TrackStart( gTOC, track );
			
			command += " ";
			command += NN::Convert< std::string >( offset );
		}
		
		command += " ";
		command += NN::Convert< std::string >( NX::DiscLength( gTOC ) / 75 );
		command += "\n";
		
		p7::write( p7::stdout_fileno, command );
	}
	
	
	static void PlayTrackRange( NX::TrackNumber first,
	                            NX::TrackNumber last )
	{
		// Set stop-mark first
		NX::AudioPlayAtTrack( gDrive, last,  true,  gPlayMode );
		
		// Then set start-mark
		NX::AudioPlayAtTrack( gDrive, first, false, gPlayMode );
	}
	
	static void PlayOneTrack( NX::TrackNumber track )
	{
		PlayTrackRange( track, track );
	}
	
	static void PlayAllTracks()
	{
		PlayTrackRange( 1, gTOC.lastAudioTrack );
	}
	
	static void PlayPause()
	{
		switch ( gStatus.status )
		{
			case kAudioStatusPlaying:  NX::AudioPause( gDrive, true  );  break;
			case kAudioStatusPaused :  NX::AudioPause( gDrive, false );  break;
			
			case kAudioStatusNil:
				PlayAllTracks();
				break;
			
			default:
				break;
		}
	}
	
	static void StopNow()
	{
		NX::AudioStop( gDrive );
	}
	
	static void AutoStop()
	{
		NX::ReadTheQSubcode_Result q = NX::ReadTheQSubcode( gDrive );
		
		NX::AudioStopAtTrack( gDrive, q.track );
	}
	
	static void Cue()
	{
		NX::ReadTheQSubcode_Result q = NX::ReadTheQSubcode( gDrive );
		
		switch ( gStatus.status )
		{
			case kAudioStatusNil:
				NX::AudioTrackSearch( gDrive,
				                      q.track,
				                      false,
				                      gPlayMode );
				break;
			
			default:
				break;
		}
	}
	
	static void GoNext()
	{
		NX::ReadTheQSubcode_Result q = NX::ReadTheQSubcode( gDrive );
		
		NX::AudioTrackSearch( gDrive,
		                      q.track + 1,
		                      true,
		                      gPlayMode );
	}
	
	static void GoBack()
	{
		NX::ReadTheQSubcode_Result q = NX::ReadTheQSubcode( gDrive );
		
		NX::AudioTrackSearch( gDrive,
		                      q.track - 1,
		                      true,
		                      gPlayMode );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		if ( argc <= 1 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "cds: (usage missing)\n" ) );
			
			return EXIT_FAILURE;
		}
		
		gDrive  = NX::OpenCDROMDriver();
		
		gTOC    = NX::ReadTOC    ( gDrive );
		gStatus = NX::AudioStatus( gDrive );
		
		switch ( argv[ 1 ][ 0 ] )
		{
			case 'h':  PrintHelp  ();  break;  // How to use CDS.
			case '/':  PrintStatus();  break;  // AudioStatus.  Mnemonic: same key as '?'
			case 'i':  PrintInfo  ();  break;  // Static info per disc.
			case 'l':  PrintList  ();  break;  // Print track list
			case 'd':  PrintDiscID();  break;  // Print CDDB disc ID data for client use.
			
			case 'p':  PlayPause();  break;  // Start playing; if already playing, pause
			case '.':  StopNow  ();  break;  // Stop playing immediately
			case ',':  AutoStop ();  break;  // Stop playing at end of current track
			case 'c':  Cue      ();  break;  // Cue if stopped, auto-cue if playing
			case 'n':  GoNext   ();  break;  // Skip to next track
			case 'b':  GoBack   ();  break;  // Skip to previous track, or track boundary?
			
			default:
				std::fprintf( stderr, "Unrecognized command '%s'\n", argv[ 1 ] );
				
				return EXIT_FAILURE;
		}
		
		return 0;
	}
	
#else
	
	int Main( int argc, iota::argv_t argv )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "cds: Sorry, no Carbon support!\n" ) );
		
		return EXIT_FAILURE;
	}
	
#endif
	
}

