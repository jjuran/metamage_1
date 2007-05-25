/*	================
 *	RunToolServer.cc
 *	================
 */

#include "RunToolServer.hh"

// Universal Interfaces
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// C++ Standard Library
#include <string>

// POSIX
#include <unistd.h>

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/slurp.hh"
#include "io/spray.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Nitrogen
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/Folders.h"

// GetPathname
#include "GetPathname.hh"

// Nitrogen Extras / Operators
#include "Operators/AEDataModel.h"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Utilities
#include "Utilities/Processes.h"

// BitsAndBytes
#include "StringFilters.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/StandardIO.hh"

#include "ToolServer.hh"


namespace RunToolServer
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	namespace Div = Divergence;
	namespace NX = NitrogenExtras;
	
	using BitsAndBytes::q;
	
	using namespace Nucleus::Operators;
	
	using namespace io::path_descent_operators;
	
	
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
		
		io::spray_file< NN::StringFlattener< std::string > >( scriptFile, script );
	}
	
	inline void WriteInputFile( const FSSpec& file )
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
	
	
	static long GetResult( const AppleEvent& reply )
	{
		SInt32 stat = N::AEGetParamPtr< N::typeSInt32 >( reply, N::AEKeyword( 'stat' ) );
		
		if ( stat != 0 )
		{
			Io::Out << "Reply stat is '" << stat << "'.\n";
			
			if ( stat == -1 )
			{
				stat = 127;
			}
		}
		
		return stat;
	}
	
	
	static NN::Owned< AppleEvent > CreateScriptEvent( const std::string& script )
	{
		return N::AECreateAppleEvent( N::kAEMiscStandards,
		                              N::kAEDoScript,
		                              N::AECreateDesc< N::typeProcessSerialNumber >( NX::LaunchApplication( sigToolServer ) ) )
		       << N::keyDirectObject
		          + N::AECreateDesc< N::typeChar >( script );
	}
	
	static NN::Owned< N::AppleEvent > AESendBlocking( const N::AppleEvent& appleEvent )
	{
		NN::Owned< AppleEvent > replyEvent = N::AEInitializeDesc();
		
		// Declare a block to limit the scope of mutableReply
		{
			N::Detail::AEDescEditor mutableReply( replyEvent );
			
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
	
	inline FSSpec DirLookup( const N::FSDirSpec& dir, const std::string& name )
	{
		return dir / name;
	}
	
	static NN::Owned< FSSpec > NewTempFile( const FSSpec& item )
	{
		if ( io::file_exists( item ) )
		{
			io::delete_file( item );
		}
		
		N::FSpCreate( item, N::OSType( 'R*ch' ), N::OSType( 'TEXT' ) );
		
		return NN::Owned< FSSpec >::Seize( item );
	}
	
	NN::Shared< FSSpec > gTempFiles[ 4 ];
	
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
		                std::bind1st( N::PtrFun( DirLookup ),
		                              tempItems ) );
		
		std::transform( files,
		                files + 4,
		                gTempFiles,
		                N::PtrFun( NewTempFile ) );
		
		WriteCommandFile( command, files[ kScriptFile ] );
		
		WriteInputFile( files[ kInputFile ] );
		
		std::string script = MakeToolServerScript( files[ kScriptFile ],
		                                           files[ kInputFile  ],
		                                           files[ kOutputFile ],
		                                           files[ kErrorFile  ] );
		
		return script;
	}
	
	static void DumpFile( const FSSpec& file, P7::FileDescriptor fd )
	{
		std::string outputFromToolServer = io::slurp_file< NN::StringFlattener< std::string > >( file );
		
		std::replace( outputFromToolServer.begin(), outputFromToolServer.end(), '\r', '\n' );
		
		io::write( fd, outputFromToolServer.data(), outputFromToolServer.size() );
	}
	
	int RunCommandInToolServer( const std::string& command )
	{
		int result = GetResult( AESendBlocking( CreateScriptEvent( SetUpScript( command ) ) ) );
		
		DumpFile( gTempFiles[ kErrorFile  ], P7::kStdErr_FileNo );
		DumpFile( gTempFiles[ kOutputFile ], P7::kStdOut_FileNo );
		
		// Delete temp files
		std::fill( gTempFiles,
		           gTempFiles + 4,
		           NN::Shared< FSSpec >() );
		
		return result;
	}
	
}

