/*	================
 *	RunToolServer.cc
 *	================
 */

#include "RunToolServer.hh"

// Mac OS
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

// Standard C++
#include <string>

// Iota
#include "iota/strings.hh"

// plus
#include "plus/pointer_to_function.hh"

// Io
#include "io/slurp.hh"
#include "io/spew.hh"

// poseven
#include "poseven/functions/stat.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Nitrogen
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/AERegistry.h"
#include "Nitrogen/Folders.h"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// GetPathname
#include "GetPathname.hh"

// FindProcess
#include "FindProcess.hh"

// Divergence
#include "Divergence/Utilities.hh"

// tlsrvr
#include "ToolServer.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Div = Divergence;
	namespace NX = NitrogenExtras;
	
	//using namespace Nucleus::Operators;
	
	using namespace io::path_descent_operators;
	
	
	static std::string q( const std::string& str )
	{
		return std::string( "'" ) + str + "'";
	}
	
	static std::string& operator<<( std::string& str, const std::string& appendage )
	{
		if ( appendage.size() > 0 )
		{
			if ( str.size() > 0 )
			{
				str += " ";
			}
			
			str += appendage;
		}
		
		return str;
	}
	
	
	static std::string DirectoryCommandForMPW()
	{
		try
		{
			FSSpec cwd = Div::ResolvePathToFSSpec( "." );
			
			return "Directory " + q( GetMacPathname( cwd ) ) + "\r";
		}
		catch ( ... )
		{
			return "Echo 'Warning:  Current directory not convertible to FSSpec, ignoring'\r";
		}
	}
	
	static void WriteCommandFile( const std::string& command, const FSSpec& scriptFile )
	{
		// Write the command into a file.
		std::string script;
		
		script += DirectoryCommandForMPW();
		script += command + "\r";
		
		io::spew_file< NN::StringFlattener< std::string > >( scriptFile, script );
	}
	
	static inline void WriteInputFile( const FSSpec& file )
	{
		// Prepare stdin file
		//NN::Owned< N::FSFileRefNum > fileH( N::FSpOpenDF( file, fsWrPerm ) );
		
		// FIXME:  Needs to be implemented
	}
	
	static std::string MakeToolServerScript( const FSSpec& scriptFile,
	                                         const FSSpec& inFile,
	                                         const FSSpec& outFile,
	                                         const FSSpec& errFile )
	{
		std::string script;
		
		script << "Set Exit 0;";
		
		script << q( GetMacPathname( scriptFile ) );
		script << "<" << q( GetMacPathname( inFile ) );
		
		std::string outPath = GetMacPathname( outFile );
		std::string errPath = GetMacPathname( errFile );
		// FIXME:  This is case-sensitive
		//bool identicalOutputAndError = outPath == errPath;
		bool identicalOutputAndError = false;
		if ( identicalOutputAndError )
		{
			script << "·" << q( outPath );
		}
		else
		{
			script << ">" << q( outPath );
			script << "³" << q( errPath );
		}
		
		return script;
	}
	
	
	static inline int device_of_ramdisk()
	{
		if ( TARGET_API_MAC_CARBON )
		{
			// Lamp Carbon doesn't check driver names, and
			// OS X's BSD layer doesn't have /sys in the first place
			return 0;
		}
		
		struct stat ram_status = { 0 };
		
		const bool has_ramdisk = p7::stat( "/sys/mac/vol/ram/mnt", ram_status );
		
		return has_ramdisk ? ram_status.st_dev : 0;
	}
	
	static ProcessSerialNumber launch_ToolServer_from_ramdisk( int dev )
	{
		const N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( -dev );
		
		return N::LaunchApplication( N::DTGetAPPL( sigToolServer, vRefNum ) );
	}
	
	
	static long GetResult( const N::AppleEvent& reply )
	{
		SInt32 stat = N::AEGetParamPtr< N::typeSInt32 >( reply, N::AEKeyword( 'stat' ) );
		
		if ( stat == -1 )
		{
			stat = 127;
		}
		
		return stat;
	}
	
	
	static ProcessSerialNumber find_or_launch_ToolServer()
	{
		try
		{
			return NX::FindProcess( sigToolServer );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != procNotFound )
			{
				throw;
			}
		}
		
		if ( const int device = device_of_ramdisk() )
		{
			try
			{
				return launch_ToolServer_from_ramdisk( device );
			}
			catch ( const N::OSStatus& err )
			{
				if ( err != afpItemNotFound )
				{
					throw;
				}
			}
		}
		
		try
		{
			return N::LaunchApplication( N::DTGetAPPL( sigToolServer ) );
		}
		catch ( const N::OSStatus& err )
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
	
	static NN::Owned< N::AppleEvent > CreateScriptEvent( const std::string& script )
	{
		ProcessSerialNumber psnToolServer = find_or_launch_ToolServer();
		
		NN::Owned< N::AppleEvent > appleEvent = N::AECreateAppleEvent( N::kAEMiscStandards,
		                                                               N::kAEDoScript,
		                                                               N::AECreateDesc< N::typeProcessSerialNumber >( psnToolServer ) );
		
		N::AEPutParamDesc( appleEvent, N::keyDirectObject, N::AECreateDesc< N::typeChar >( script ) );
		
		return appleEvent;
	}
	
	static NN::Owned< N::AppleEvent > AESendBlocking( const N::AppleEvent& appleEvent )
	{
		NN::Owned< N::AppleEvent > replyEvent = N::AEInitializeDesc< N::AppleEvent >();
		
		// Declare a block to limit the scope of mutableReply
		{
			N::Detail::AEDescEditor< N::AppleEvent > mutableReply( replyEvent );
			
			N::ThrowOSStatus( Div::AESendBlocking( &appleEvent, &mutableReply.Get() ) );
			
			// Reply is available.  End scope to restore the reply.
		}
		
		return replyEvent;
	}
	
	enum
	{
		kScriptFile,
		kInputFile,
		kOutputFile,
		kErrorFile
	};
	
	static inline FSSpec DirLookup( const N::FSDirSpec& dir, const std::string& name )
	{
		return dir / name;
	}
	
	static FSSpec NewTempFile( const FSSpec& item )
	{
		if ( io::file_exists( item ) )
		{
			io::delete_file( item );
		}
		
		N::FSpCreate( item, N::OSType( 'R*ch' ), N::OSType( 'TEXT' ) );
		
		return item;
	}
	
	static FSSpec gTempFiles[ 4 ];
	
	static std::string SetUpScript( const std::string& command )
	{
		// Send a Do Script event with the command as the direct object.
		// Better yet:
		//  * Write the command to a file (which we'll invoke by its filename)
		// so we don't have to quote the command.
		//  * Create temp files to store I/O.
		//  * Write all input to temp file.
		//  * Run the script with I/O redirected.
		//  * Dump the stored output to stdout and stderr.
		
		// It's okay if command has the fancy quotes, because we don't actually
		// refer to it in the Apple event itself.
		
		// This breaks in SheepShaver running 8.5, at least for me.
		//N::FSDirSpec tempItems = N::FindFolder( kTemporaryFolderType, kCreateFolder );
		
		N::FSDirSpec tempItems = NN::Convert< N::FSDirSpec >( Div::ResolvePathToFSSpec( "/tmp" ) );
		
		static char const* filenames[ 4 ] =
		{
			".tlsrvr-script", 
			".tlsrvr-stdin", 
			".tlsrvr-stdout", 
			".tlsrvr-stderr"
		};
		
		static FSSpec files[ 4 ];
		
		std::transform( filenames,
		                filenames + 4,
		                files,
		                std::bind1st( plus::ptr_fun( DirLookup ),
		                              tempItems ) );
		
		std::transform( files,
		                files + 4,
		                gTempFiles,
		                plus::ptr_fun( NewTempFile ) );
		
		WriteCommandFile( command, files[ kScriptFile ] );
		
		WriteInputFile( files[ kInputFile ] );
		
		std::string script = MakeToolServerScript( files[ kScriptFile ],
		                                           files[ kInputFile  ],
		                                           files[ kOutputFile ],
		                                           files[ kErrorFile  ] );
		
		return script;
	}
	
	static void ConvertAndDumpMacText( std::string& text, p7::fd_t fd )
	{
		std::replace( text.begin(), text.end(), '\r', '\n' );
		
		p7::write( fd, text );
	}
	
	static std::string Slurp( const FSSpec& file )
	{
		return io::slurp_file< NN::StringFlattener< std::string > >( file );
	}
	
	static void DumpFile( const FSSpec& file, p7::fd_t fd )
	{
		std::string text = Slurp( file );
		
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
	
	static bool user_cancelled( const std::string& errors )
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
	
	int RunCommandInToolServer( const std::string& command, bool switch_layers )
	{
		// This is a bit of a hack.
		// It really ought to happen just after we send the event.
		if ( switch_layers && N::SameProcess( N::CurrentProcess(),
		                                      N::GetFrontProcess() ) )
		{
			N::SetFrontProcess( find_or_launch_ToolServer() );
		}
		
		int result = GetResult( AESendBlocking( CreateScriptEvent( SetUpScript( command ) ) ) );
		
		if ( switch_layers && N::SameProcess( find_or_launch_ToolServer(),
		                                      N::GetFrontProcess() ) )
		{
			N::SetFrontProcess( N::CurrentProcess() );
		}
		
		if ( result == -9 )
		{
			// Observed from MWLink68K and MWLinkPPC on Command-period
			return 128;
		}
		
		std::string errors = Slurp( gTempFiles[ kErrorFile ] );
		
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
		}
		
		ConvertAndDumpMacText( errors, p7::stderr_fileno );
		
		DumpFile( gTempFiles[ kOutputFile ], p7::stdout_fileno );
		
		// Delete temp files
		std::for_each( gTempFiles,
		               gTempFiles + 4,
		               plus::ptr_fun( &N::FSpDelete ) );
		
		return result;
	}
	
}

