/*	================
 *	RunToolServer.cc
 *	================
 */

#include "RunToolServer.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>
#include <stdlib.h>

// Standard C++
#include <algorithm>

// Iota
#include "iota/strings.hh"

// mac-sys-utils
#include "mac_sys/current_process.hh"
#include "mac_sys/is_front_process.hh"

// mac-file-utils
#include "mac_file/desktop.hh"

// mac-proc-utils
#include "mac_proc/find_process.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"
#include "plus/string/concat.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/extras/spew.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// AESendBlocking
#include "AESendBlocking.hh"

// mac_pathname
#include "mac_pathname_from_path.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/Processes.hh"

// tlsrvr
#include "ToolServer.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static plus::string q( const plus::string& str )
	{
		return "'" + str + "'";
	}
	
	static nucleus::mutable_string& operator<<( nucleus::mutable_string& str, const plus::string& appendage )
	{
		if ( appendage.size() > 0 )
		{
			if ( str.size() > 0 )
			{
				str += " ";
			}
			
			str.append( appendage.data(), appendage.size() );
		}
		
		return str;
	}
	
	
	static plus::string DirectoryCommandForMPW()
	{
		try
		{
			plus::var_string directory_cmd = "Directory '";
			
			directory_cmd += mac_pathname_from_path( "." );
			
			directory_cmd += "'" "\r";
			
			return directory_cmd;
		}
		catch ( ... )
		{
			return "Echo 'Warning: Can't set non-Mac directory as default'\r";
		}
	}
	
	static plus::string make_script_from_command( const plus::string& command )
	{
		plus::var_string script;
		
		script += DirectoryCommandForMPW();
		script += command;
		script += "\r"
		          "Set CommandStatus {Status}" "\r"
		          "Directory \"{MPW}\"" "\r"   // don't keep the cwd busy
		          "Exit {CommandStatus}" "\r";
		
		return script;
	}
	
	static nucleus::string MakeToolServerScript( const char*  script_path,
	                                             const char*  out_path,
	                                             const char*  err_path )
	{
		nucleus::mutable_string script;
		
		script << "Set Exit 0;";
		
		script << q( mac_pathname_from_path( script_path ) );
		script << "< Dev:Null";
		
		plus::string outPath = mac_pathname_from_path( out_path );
		plus::string errPath = mac_pathname_from_path( err_path );
		// FIXME:  This is case-sensitive
		//bool identicalOutputAndError = outPath == errPath;
		bool identicalOutputAndError = false;
		if ( identicalOutputAndError )
		{
			script << "\xB7" << q( outPath );  // sum symbol
		}
		else
		{
			script << ">"    << q( outPath );
			script << "\xB3" << q( errPath );  // greater-than-or-equal-to
		}
		
		return script;
	}
	
	
	static inline SInt32
	//
	AEGetParamPtr_stat( const AppleEvent& appleEvent )
	{
		const Mac::DescType type = Mac::typeSInt32;
		const Mac::AEKeyword key = Mac::AEKeyword( 'stat' );
		
		return N::DescType_get< type >( N::AEGetParamPtr_Getter< type >( appleEvent, key ) );
	}
	
	static long GetResult( const Mac::AppleEvent& reply )
	{
		SInt32 stat = AEGetParamPtr_stat( reply );
		
		if ( stat == -1 )
		{
			stat = 127;
		}
		
		return stat;
	}
	
	
	static
	bool process_exists( const ProcessSerialNumber& psn )
	{
		OSErr err;
		
		ProcessInfoRec processInfo;
		
		processInfo.processInfoLength = sizeof processInfo;
		processInfo.processName       = NULL;
		processInfo.processAppSpec    = NULL;
		
		err = GetProcessInformation( &psn, &processInfo );
		
		return err == noErr;
	}
	
	static ProcessSerialNumber find_or_launch_ToolServer()
	{
		OSErr err;
		FSSpec appFile;
		ProcessSerialNumber psn;
		
		if ( mac::proc::find_process( psn, sigToolServer ) == noErr )
		{
			return psn;
		}
		
		if ( const char* ToolServer_path = getenv( "ToolServer" ) )
		{
			using mac::relix::FSSpec_from_existing_path;
			
			err = FSSpec_from_existing_path( ToolServer_path, appFile );
		}
		else
		{
			using mac::file::get_desktop_APPL;
			using mac::file::get_desktop_APPL_on_RAM_disk;
			
			err = get_desktop_APPL_on_RAM_disk( appFile, sigToolServer );
			
			if ( err )
			{
				err = get_desktop_APPL( appFile, sigToolServer );
			}
		}
		
		if ( err == afpItemNotFound )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "tlsrvr: ToolServer not found\n" ) );
			
			exit( 43 );
		}
		
		Mac::ThrowOSStatus( err );
		
		try
		{
			return N::LaunchApplication( appFile );
		}
		catch ( const Mac::OSStatus& err )
		{
			if ( err == afpItemNotFound )
			{
				p7::write( p7::stderr_fileno, STR_LEN( "tlsrvr: ToolServer not found\n" ) );
			}
			else if ( TARGET_API_MAC_CARBON  &&  err == -10661 )
			{
				p7::write( p7::stderr_fileno, STR_LEN( "tlsrvr: ToolServer not runnable on this system\n" ) );
			}
			else
			{
				throw;
			}
		}
		
		throw p7::exit_failure;
	}
	
	static n::owned< Mac::AppleEvent > CreateScriptEvent( const ProcessSerialNumber&  psn,
	                                                      const nucleus::string&      script )
	{
		n::owned< Mac::AppleEvent > appleEvent = N::AECreateAppleEvent( Mac::kAEMiscStandards,
		                                                                Mac::kAEDoScript,
		                                                                N::AECreateDesc< Mac::typeProcessSerialNumber >( psn ) );
		
		N::AEPutParamDesc( appleEvent, Mac::keyDirectObject, N::AECreateDesc< Mac::typeChar >( script ) );
		
		return appleEvent;
	}
	
	static
	n::owned< Mac::AppleEvent > CreateQuitEvent( const ProcessSerialNumber& psn )
	{
		const Mac::AEEventClass aevt = Mac::kCoreEventClass;
		const Mac::AEEventID    quit = Mac::kAEQuitApplication;
		const Mac::DescType  typePSN = Mac::typeProcessSerialNumber;
		
		using namespace Nitrogen;
		
		return AECreateAppleEvent( aevt, quit, AECreateDesc< typePSN >( psn ) );
	}
	
	static n::owned< Mac::AppleEvent > AESendBlocking( const Mac::AppleEvent& appleEvent )
	{
		Mac::AppleEvent reply;
		
		Mac::ThrowOSStatus( AESendBlocking( &appleEvent, &reply ) );
		
		return n::owned< Mac::AppleEvent >::seize( reply );
	}
	
	enum
	{
		kScriptFile,
		kOutputFile,
		kErrorFile
	};
	
	static void make_temp_file( const char* path )
	{
		(void) p7::open( path,
		                 p7::o_wronly | p7::o_creat | p7::o_trunc,
		                 p7::_666 );
	}
	
	const int n_files = 3;
	
	static char const* temp_file_paths[ n_files ] =
	{
		"/tmp/.tlsrvr-" "script", 
		"/tmp/.tlsrvr-" "stdout", 
		"/tmp/.tlsrvr-" "stderr"
	};
	
	static nucleus::string SetUpScript( const plus::string& command )
	{
		// Send a Do Script event with the command as the direct object.
		// Better yet:
		//  * Write the command to a file (which we'll invoke by its filename)
		// so we don't have to quote the command.
		//  * Create temp files to store I/O.
		//  * Run the script with I/O redirected.
		//  * Dump the stored output to stdout and stderr.
		
		// It's okay if command has the fancy quotes, because we don't actually
		// refer to it in the Apple event itself.
		
		for ( int i = 0;  i < n_files;  ++i )
		{
			make_temp_file( temp_file_paths[ i ] );
		}
		
		plus::string inner_script = make_script_from_command( command );
		
		p7::spew( p7::open( temp_file_paths[ kScriptFile ], p7::o_wronly ),
		          inner_script );
		
		nucleus::string script = MakeToolServerScript( temp_file_paths[ kScriptFile ],
		                                               temp_file_paths[ kOutputFile ],
		                                               temp_file_paths[ kErrorFile  ] );
		
		return script;
	}
	
	static void ConvertAndDumpMacText( plus::var_string& text, p7::fd_t fd )
	{
		std::replace( text.begin(), text.end(), '\r', '\n' );
		
		p7::write( fd, plus::utf8_from_mac( text ) );
	}
	
	static void dump_file( const char* path, p7::fd_t fd )
	{
		plus::var_string text = p7::slurp( path );
		
		ConvertAndDumpMacText( text, fd );
	}
	
	static inline bool matches_at_end( const char* a_end,
	                                   size_t a_length,
	                                   const char* b_begin,
	                                   size_t b_length )
	{
		const char* a_begin = a_end - b_length;
		
		return a_length >= b_length  &&  std::equal( a_begin, a_end, b_begin );
	}
	
	static bool find_from_end( const char* begin,
	                           const char* end,
	                           const char* pattern,
	                           size_t length )
	{
		const char* pattern_end = pattern + length;
		
		for ( const char* p = end - length;  p >= begin;  --p )
		{
			if ( std::equal( pattern, pattern_end, p ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	static bool strings_equal( const char* a_begin,
	                           const char* a_end,
	                           const char* b_begin,
	                           size_t b_length )
	{
		const size_t a_length = a_end - a_begin;
		
		return a_length == b_length  &&  std::equal( a_begin, a_end, b_begin );
	}
	
	static bool user_cancelled( const plus::string& errors )
	{
		const char* begin = &*errors.begin();
		const char* end   = &*errors.end  ();
		
		if ( strings_equal( begin,
		                    end,
		                    STR_LEN( "\r" "User break, cancelled...\r" ) ) )
		{
			return true;
		}
		
		// The magic line we're looking for is:
		//
		//     ### ToolServer - Execution of HD:Path:To:Script terminated.
		
		const bool terminated = matches_at_end( end,
		                                        errors.size(),
		                                        STR_LEN( " terminated.\r" ) );
		
		if ( terminated )
		{
			return find_from_end( begin,
			                      end,
			                      STR_LEN( "### ToolServer - Execution of" ) );
		}
		
		return false;
	}
	
	static inline
	bool out_of_memory( const plus::string& output )
	{
		return output == "\r" "Out of memory\r";
	}
	
	static void switch_process( const ProcessSerialNumber& from,
	                            const ProcessSerialNumber& to )
	{
		using mac::sys::is_front_process;
		
		if ( is_front_process( from ) )
		{
			N::SetFrontProcess( to );
			
			while ( is_front_process( from ) )
			{
				sleep( 0 );
			}
		}
	}
	
	int RunCommandInToolServer( const plus::string& command, bool switch_layers )
	{
		const ProcessSerialNumber& self = mac::sys::current_process();
		
		bool retried = false;
		
	retry:
		
		const ProcessSerialNumber toolServer = find_or_launch_ToolServer();
		
		// This is a bit of a hack.
		// It really ought to happen just after we send the event.
		if ( switch_layers )
		{
			switch_process( self, toolServer );
		}
		
		int result = GetResult( AESendBlocking( CreateScriptEvent( toolServer,
		                                                           SetUpScript( command ) ) ) );
		
		if ( switch_layers )
		{
			switch_process( toolServer, self );
		}
		
		if ( result == -9 )
		{
			// Observed from MWLink68K and MWLinkPPC on Command-period
			return 128;
		}
		
		plus::var_string output = p7::slurp( temp_file_paths[ kOutputFile ] );
		plus::var_string errors = p7::slurp( temp_file_paths[ kErrorFile  ] );
		
		// A Metrowerks tool returns 1 on error and 2 on user break, except that
		// if you limit the number of diagnostics displayed and there more errors
		// than the limit, it pretends that YOU cancelled it while printing the
		// output, claiming "User break", and returning 2.
		// Also, sometimes 1 is returned on user cancel, which could mean that
		// ToolServer was running a script rather than a Metrowerks tool.
		// And canceling a script might also return 0.  Go figure.
		
		if ( result <= 2 )
		{
			if ( user_cancelled( errors ) )
			{
				// User pressed Command-period
				return 128;
			}
			else if ( result == 2 )
			{
				// Sorry, but the existence of more errors than I asked to be
				// printed does NOT equal user-sponsored cancellation.
				result = 1;
			}
			else if ( ! retried  &&  out_of_memory( output ) )
			{
				retried = true;
				
				write( STDOUT_FILENO, STR_LEN( "tlsrvr: ToolServer is out of memory.\n" ) );
				write( STDOUT_FILENO, STR_LEN( "tlsrvr: Quitting ToolServer...\n"       ) );
				
				AESendBlocking( CreateQuitEvent( toolServer ) );
				
				while ( process_exists( toolServer ) )
				{
					kill( 1, 0 );
				}
				
				write( STDOUT_FILENO, STR_LEN( "tlsrvr: Retrying command (once)...\n" ) );
				
				goto retry;
			}
		}
		
		ConvertAndDumpMacText( errors, p7::stderr_fileno );
		ConvertAndDumpMacText( output, p7::stdout_fileno );
		
		// Delete temp files
		for ( int i = 0;  i < n_files;  ++i )
		{
			unlink( temp_file_paths[ i ] );
		}
		
		return result;
	}
	
}
