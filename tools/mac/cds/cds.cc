/*
	cds.cc
	------
*/

// Iota
#include "iota/strings.hh"

// gear
#include "gear/hexadecimal.hh"
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/write.hh"

// MacCDROM
#include "MacCDROM/Audio.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
#if !TARGET_API_MAC_CARBON
	
	namespace CD = MacCDROM;
	
	
	static CD::CDROMDrive            gDrive;
	static CD::TrackNumber           gSelectedTrack = 1;
	static CD::CDROMTableOfContents  gTOC;
	static CD::AudioStatus_Result    gStatus;
	static CD::AudioPlayMode         gPlayMode = CD::kAudioPlayModeStereo;
	
	
	static void print_field( const char* name, const char* value )
	{
		plus::var_string message = name;
		
		message += ": ";
		message += value;
		message += "\n";
		
		p7::write( p7::stdout_fileno, message );
	}
	
	static void PrintHelp()
	{
		
	}
	
	static void PrintStatus()
	{
		const char* state = "ERROR";
		
		switch ( gStatus.status )
		{
			case CD::kAudioStatusPlaying:  state = "playing";    break;
			case CD::kAudioStatusPaused :  state = "paused";     break;
			case CD::kAudioStatusMuteOn :  state = "muting-on";  break;
			case CD::kAudioStatusDone   :  state = "done";       break;
			case CD::kAudioStatusError  :  state = "error";      break;
			case CD::kAudioStatusNil    :  state = "nil";        break;
		}
		
		print_field( "Audio status", state );
		
		print_field( "Audio play mode", gear::inscribe_decimal( gStatus.playMode & 0x0f ) );
		print_field( "Track format",    gear::inscribe_decimal( gStatus.control  & 0x0f ) );
		
		char time[] = "mm:ss";
		
		time[ 0 ] = '0' | gStatus.minutes >> 4;
		time[ 1 ] = '0' | gStatus.minutes & 0x0F;
		
		time[ 3 ] = '0' | gStatus.seconds >> 4;
		time[ 4 ] = '0' | gStatus.seconds & 0x0F;
		
		print_field( "Time", time );
	}
	
	static void PrintInfo()
	{
		
	}
	
	static void PrintList()
	{
		CD::TrackCount tracks = CD::CountTracks( gTOC );
		
		for ( int track = 1;  track <= tracks;  ++track )
		{
			int frames  = CD::TrackLength( gTOC, track );
			int seconds = frames / 75;
			
			int minutes = seconds / 60;
			
			seconds %= 60;
			
			char buffer[] = "Track  n: 0m:0s\n";
			
			using gear::fill_unsigned_decimal;
			
			fill_unsigned_decimal( track, buffer + STRLEN( "Track " ), 2 );
			
			fill_unsigned_decimal( minutes, buffer + STRLEN( "Track  n: "    ), 2 );
			fill_unsigned_decimal( seconds, buffer + STRLEN( "Track  n: 0m:" ), 2 );
			
			p7::write( p7::stdout_fileno, buffer, sizeof buffer - 1 );
		}
	}
	
	static void PrintDiscID()
	{
		unsigned int discID = CD::CDDBDiscID( gTOC );
		
		char discid_message[] = "Disc ID is abcd1234\n";
		
		char *const discid_buf = discid_message + STRLEN( "Disc ID is " );
		
		gear::encode_32_bit_hex( discID, discid_buf );
		
		p7::write( p7::stdout_fileno, discid_message, sizeof discid_message - 1 );
		
		CD::TrackCount tracks = CD::CountTracks( gTOC );
		
		plus::var_string command = "discid ";
		
		command += gear::inscribe_decimal( tracks );
		
		for ( int track = 1;  track <= tracks;  ++track )
		{
			int offset = CD::TrackStart( gTOC, track );
			
			command += " ";
			command += gear::inscribe_decimal( offset );
		}
		
		command += " ";
		command += gear::inscribe_decimal( CD::DiscLength( gTOC ) / 75 );
		command += "\n";
		
		p7::write( p7::stdout_fileno, command );
	}
	
	
	static void PlayTrackRange( CD::TrackNumber first,
	                            CD::TrackNumber last )
	{
		// Set stop-mark first
		CD::AudioPlayAtTrack( gDrive, last,  true,  gPlayMode );
		
		// Then set start-mark
		CD::AudioPlayAtTrack( gDrive, first, false, gPlayMode );
	}
	
	static void PlayOneTrack( CD::TrackNumber track )
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
			case CD::kAudioStatusPlaying:  CD::AudioPause( gDrive, true  );  break;
			case CD::kAudioStatusPaused :  CD::AudioPause( gDrive, false );  break;
			
			case CD::kAudioStatusNil:
				PlayAllTracks();
				break;
			
			default:
				break;
		}
	}
	
	static void StopNow()
	{
		CD::AudioStop( gDrive );
	}
	
	static void AutoStop()
	{
		CD::ReadTheQSubcode_Result q = CD::ReadTheQSubcode( gDrive );
		
		CD::AudioStopAtTrack( gDrive, q.track );
	}
	
	static void Cue()
	{
		CD::ReadTheQSubcode_Result q = CD::ReadTheQSubcode( gDrive );
		
		switch ( gStatus.status )
		{
			case CD::kAudioStatusNil:
				CD::AudioTrackSearch( gDrive,
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
		CD::ReadTheQSubcode_Result q = CD::ReadTheQSubcode( gDrive );
		
		CD::AudioTrackSearch( gDrive,
		                      q.track + 1,
		                      true,
		                      gPlayMode );
	}
	
	static void GoBack()
	{
		CD::ReadTheQSubcode_Result q = CD::ReadTheQSubcode( gDrive );
		
		CD::AudioTrackSearch( gDrive,
		                      q.track - 1,
		                      true,
		                      gPlayMode );
	}
	
	
	static int unrecognized_command( const char* command )
	{
		plus::var_string message = "cds: unrecognized command '";
		
		message += command;
		message += "'\n";
		
		return 1;
	}
	
	int Main( int argc, char** argv )
	{
		if ( argc <= 1 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "cds: (usage missing)\n" ) );
			
			return 1;
		}
		
		gDrive  = CD::OpenCDROMDriver();
		
		gTOC    = CD::ReadTOC    ( gDrive );
		gStatus = CD::AudioStatus( gDrive );
		
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
				return unrecognized_command( argv[ 1 ] );
		}
		
		return 0;
	}
	
#else
	
	int Main( int argc, char** argv )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "cds: Sorry, no Carbon support!\n" ) );
		
		return 1;
	}
	
#endif
	
}
