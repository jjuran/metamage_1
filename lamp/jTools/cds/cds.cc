/*	======
 *	cds.cc
 *	======
 */

// Nitrogen Core
#include "Nitrogen/Assert.h"

// Nitrogen Extras / ClassicExtras
#include "ClassicExtras/CDROMAudio.h"

// BitsAndBytes
#include "DecimalStrings.hh"
#include "HexStrings.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;

namespace CDS
{
	
	namespace N = Nitrogen;
	namespace NX = NitrogenExtras;
	namespace Bits = BitsAndBytes;
	
	using namespace NX::Constants;
	
	
	static NX::CDROMDrive            gDrive;
	static NX::TrackNumber           gSelectedTrack = 1;
	static NX::CDROMTableOfContents  gTOC;
	static NX::AudioStatus_Result    gStatus;
	static NX::AudioPlayMode         gPlayMode = kAudioPlayModeStereo;
	
	static std::string MinSec( unsigned int seconds )
	{
		return Bits::EncodeDecimal2( seconds / 60 ) + ":" + 
		       Bits::EncodeDecimal2( seconds % 60 );
	}
	
	static void PrintHelp()
	{
		
	}
	
	static void PrintStatus()
	{
	#if !TARGET_API_MAC_CARBON
		
		const char* state = "ERROR";
		
		switch ( gStatus.status )
		{
			case 0:  state = "playing";    break;
			case 1:  state = "paused";     break;
			case 2:  state = "muting-on";  break;
			case 3:  state = "done";       break;
			case 4:  state = "error";      break;
			case 5:  state = "nil";        break;
		}
		
		Io::Out << "Audio status: " << state << "\n";
		
		Io::Out << "Audio play mode: " << (gStatus.playMode & 0x0F) << "\n";
		Io::Out << "Track format: "    << (gStatus.control  & 0x0F) << "\n";
		
		std::string time = "mm:ss";
		
		time[ 0 ] = '0' | gStatus.minutes >> 4;
		time[ 1 ] = '0' | gStatus.minutes & 0x0F;
		
		time[ 3 ] = '0' | gStatus.seconds >> 4;
		time[ 4 ] = '0' | gStatus.seconds & 0x0F;
		
		Io::Out << "Time: " << time << "\n";
		
	#endif
	}
	
	static void PrintInfo()
	{
		
	}
	
	static void PrintList()
	{
	#if !TARGET_API_MAC_CARBON
		
		NX::TrackCount tracks = NX::CountTracks( gTOC );
		
		for ( int track = 1;  track <= tracks;  ++track )
		{
			int frames = NX::TrackLength( gTOC, track );
			int seconds = frames / 75;
			
			Io::Out << "Track " << track << ": " << MinSec( seconds ) << "\n";
		}
		
	#endif
		
	}
	
	static void PrintDiscID()
	{
	#if !TARGET_API_MAC_CARBON
		
		unsigned int discID = NX::CDDBDiscID( gTOC );
		
		Io::Out << "Disc ID is " << Bits::EncodeAsHex( discID ) << "\n";
		
		NX::TrackCount tracks = NX::CountTracks( gTOC );
		
		Io::Out << "discid " << tracks;
		
		for ( int track = 1;  track <= tracks;  ++track )
		{
			int offset = NX::TrackStart( gTOC, track );
			
			Io::Out << " " << offset;
		}
		
		Io::Out << " " << NX::DiscLength( gTOC ) / 75 << "\n";
		
	#endif
		
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
			case 0:  NX::AudioPause( gDrive, true  );  break;
			case 1:  NX::AudioPause( gDrive, false );  break;
			
			case 5:
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
			case 5:
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
	
	
	static int Main( const char* argv1 )
	{
		gDrive  = NX::OpenCDROMDriver();
		
		gTOC    = NX::ReadTOC    ( gDrive );
		gStatus = NX::AudioStatus( gDrive );
		
		switch ( argv1[ 0 ] )
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
				Io::Err << "Unrecognized command " << argv1 << "\n";
				return 1;
				break;
		}
		
		return 0;
	}
	
}

int O::Main( int argc, const char *const argv[] )
{
	ASSERT( argc > 0 );
	
	if ( argc <= 1 )
	{
		Io::Err << "cds: (usage missing)\n";
		return 1;
	}
	
	return CDS::Main( argv[ 1 ] );
}

