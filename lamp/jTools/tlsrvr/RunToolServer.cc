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
#ifndef __PROCESSES__
#include <Processes.h>
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
#include "mac_proc/launch_application.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// AESendBlocking
#include "AESendBlocking.hh"

// Nitrogen
#include "Mac/AppleEvents/Functions/AEDisposeDesc.hh"
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// tlsrvr
#include "escape.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static plus::string DirectoryCommandForMPW()
	{
		try
		{
			plus::var_string directory_cmd;
			
			directory_cmd  = "Directory ";
			directory_cmd += escaped_HFS_path( "." );
			directory_cmd += '\r';
			
			return directory_cmd;
		}
		catch ( ... )
		{
			return "Echo 'Warning: Can't set non-Mac directory as default'\r";
		}
	}
	
	static
	plus::string command_with_I_O_redirected( const plus::string& command )
	{
		plus::var_string script;
		
		script += command;
		script += " < Dev:Null ";
		
		return script;
	}
	
	static plus::string MakeToolServerScript( const plus::string& command )
	{
		plus::var_string script;
		
		script += "Set Exit 0; ";
		script += DirectoryCommandForMPW();
		script += command_with_I_O_redirected( command );
		script += "\r"
		          "Set CommandStatus {Status}" "\r"
		          "Directory \"{MPW}\"" "\r"   // don't keep the cwd busy
		          "Exit {CommandStatus}" "\r";
		
		return script;
	}
	
	
	static inline
	SInt32 AEGetParamPtr_stat( const AppleEvent& event )
	{
		OSErr  err;
		SInt32 stat;
		
		AEKeyword key  = 'stat';
		DescType  type = typeSInt32;
		Size      size = sizeof stat;
		
		err = AEGetParamPtr( &event, key, type, &type, &stat, size, &size );
		
		Mac::ThrowOSStatus( err );
		
		return stat;
	}
	
	static inline
	long GetResult( const AppleEvent& reply )
	{
		SInt32 stat = AEGetParamPtr_stat( reply );
		
		if ( stat == -1 )
		{
			stat = 127;
		}
		
		return stat;
	}
	
	static
	plus::string AEGetParamPtr_text( const AppleEvent& event, AEKeyword key )
	{
		OSErr    err;
		Size     size;
		DescType type;
		
		plus::string result;
		
		err = AESizeOfParam( &event, key, &type, &size );
		
		if ( err == noErr )
		{
			char* p = result.reset( size );
			
			err = AEGetParamPtr( &event, key, typeText, &type, p, size, &size );
			
			Mac::ThrowOSStatus( err );
		}
		
		return result;
	}
	
	
	static inline
	bool process_exists( const ProcessSerialNumber& psn )
	{
		return mac::proc::test_process( psn ) == noErr;
	}
	
	static ProcessSerialNumber find_or_launch_ToolServer()
	{
		OSErr err;
		FSSpec appFile;
		ProcessSerialNumber psn;
		
		enum
		{
			sigToolServer = 'MPSX',
		};
		
		if ( mac::proc::find_process( psn, sigToolServer ) == noErr )
		{
			return psn;
		}
		
		using mac::file::get_desktop_APPL;
		using mac::file::get_desktop_APPL_on_RAM_disk;
		
		err = get_desktop_APPL_on_RAM_disk( appFile, sigToolServer );
		
		if ( err )
		{
			err = get_desktop_APPL( appFile, sigToolServer );
		}
		
		if ( err == afpItemNotFound )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "tlsrvr: ToolServer not found\n" ) );
			
			exit( 43 );
		}
		
		Mac::ThrowOSStatus( err );
		
		if ( (err = mac::proc::launch_application( &psn, appFile )) )
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
				Mac::ThrowOSStatus( err );
			}
			
			throw p7::exit_failure;
		}
		
		return psn;
	}
	
	static
	n::owned< Mac::AppleEvent >
	//
	SendScript( const ProcessSerialNumber&  psn,
	            const plus::string&         command )
	{
		OSErr           err;
		AEAddressDesc   addr;
		Mac::AppleEvent event;
		Mac::AppleEvent reply;
		
		plus::string script = MakeToolServerScript( command );
		
		err = AECreateDesc( typeProcessSerialNumber, &psn, sizeof psn, &addr );
		
		if ( err == noErr )
		{
			err = AECreateAppleEvent( kAEMiscStandards,
			                          kAEDoScript,
			                          &addr,
			                          kAutoGenerateReturnID,
			                          kAnyTransactionID,
			                          &event );
			
			AEDisposeDesc( &addr );
			
			if ( err == noErr )
			{
				err = AEPutParamPtr( &event,
				                     keyDirectObject,
				                     typeChar,
				                     script.data(),
				                     script.size() );
				
				if ( err )
				{
					AEDisposeDesc( &event );
				}
			}
		}
		
		if ( err == noErr )
		{
			err = AESendBlocking( &event, &reply );
			
			AEDisposeDesc( &event );
		}
		
		Mac::ThrowOSStatus( err );
		
		return n::owned< Mac::AppleEvent >::seize( reply );
	}
	
	static
	n::owned< Mac::AppleEvent > CreateQuitEvent( const ProcessSerialNumber& psn )
	{
		OSErr           err;
		AEAddressDesc   addr;
		Mac::AppleEvent event;
		
		err = AECreateDesc( typeProcessSerialNumber, &psn, sizeof psn, &addr );
		
		if ( err == noErr )
		{
			err = AECreateAppleEvent( kCoreEventClass,
			                          kAEQuitApplication,
			                          &addr,
			                          kAutoGenerateReturnID,
			                          kAnyTransactionID,
			                          &event );
			
			AEDisposeDesc( &addr );
		}
		
		Mac::ThrowOSStatus( err );
		
		return n::owned< Mac::AppleEvent >::seize( event );
	}
	
	static n::owned< Mac::AppleEvent > AESendBlocking( const Mac::AppleEvent& appleEvent )
	{
		Mac::AppleEvent reply;
		
		Mac::ThrowOSStatus( AESendBlocking( &appleEvent, &reply ) );
		
		return n::owned< Mac::AppleEvent >::seize( reply );
	}
	
	static void ConvertAndDumpMacText( plus::var_string& text, p7::fd_t fd )
	{
		std::replace( text.begin(), text.end(), '\r', '\n' );
		
		p7::write( fd, plus::utf8_from_mac( text ) );
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
			SetFrontProcess( &to );
			
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
		
		// Send a Do Script event with the command as the direct object.
		
		n::owned< Mac::AppleEvent > reply = SendScript( toolServer, command );
		
		int result = GetResult( reply );
		
		if ( switch_layers )
		{
			switch_process( toolServer, self );
		}
		
		if ( result == -9 )
		{
			// Observed from MWLink68K and MWLinkPPC on Command-period
			return 128;
		}
		
		plus::var_string output = AEGetParamPtr_text( reply, '----' );
		plus::var_string errors = AEGetParamPtr_text( reply, 'diag' );
		
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
		
		return result;
	}
	
}
