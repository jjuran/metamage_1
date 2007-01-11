/*	================
 *	RunToolServer.cc
 *	================
 */

#include "RunToolServer.hh"

// Universal Interfaces
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// POSIX
#include <unistd.h>

// C++ Standard Library
#include <string>

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen Carbon
#include "Nitrogen/AEInteraction.h"
#include "Nitrogen/Folders.h"

// Nitrogen Extras / Operators
#include "Operators/AEDataModel.h"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"
#include "Utilities/Processes.h"

// Io
#include "Io/Stream.hh"

// BitsAndBytes
#include "StringFilters.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/StandardIO.hh"

#include "ToolServer.hh"


namespace Io
{
	
	using FileSystem::Write;
	
}

namespace RunToolServer
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Div = Divergence;
	namespace FS = FileSystem;
	namespace NX = NitrogenExtras;
	
	using N::fsRdPerm;
	using N::fsWrPerm;
	
	using BitsAndBytes::q;
	
	using namespace Nucleus::Operators;
	
	
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
			
			return "Directory " + q( N::FSpGetMacPathname( cwd ) ) + "\r";
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
		
		N::FSWrite( N::FSpOpenDF( scriptFile, fsWrPerm ),
		            script.size(),
		            script.data() );
	}
	
	static void WriteInputFile( const FSSpec& file )
	{
		// Prepare stdin file
		NN::Owned< N::FSFileRefNum > fileH( N::FSpOpenDF( file, fsWrPerm ) );
		
		// FIXME:  Needs to be implemented
	}
	
	static std::string MakeToolServerScript( const FSSpec& scriptFile,
	                                         const FSSpec& inFile,
	                                         const FSSpec& outFile,
	                                         const FSSpec& errFile )
	{
		std::string script;
		
		script << "Set Exit 0;";
		
		script << q( N::FSpGetMacPathname( scriptFile ) );
		script << "<" << q( N::FSpGetMacPathname( inFile ) );
		
		std::string outPath = N::FSpGetMacPathname( outFile );
		std::string errPath = N::FSpGetMacPathname( errFile );
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
		}
		
		return stat;
	}
	
	
	static NN::Owned< AppleEvent > CreateScriptEvent( const std::string& script )
	{
		return N::AECreateAppleEvent( kAEMiscStandards,
		                              kAEDoScript,
		                              N::AECreateDesc< N::typeProcessSerialNumber >( NX::LaunchApplication( sigToolServer ) ) )
		       << N::keyDirectObject
		          + N::AECreateDesc< N::typeChar >( script );
	}
	
	/*
	static AEReturnID SendScriptEvent( const AppleEvent& appleEvent )
	{
		// Build and send the event.
		
		const bool waitForReply = TARGET_RT_MAC_MACHO;
		
		NN::Owned< AppleEvent > reply = N::AESend
		(
			appleEvent, 
			( waitForReply ? kAEWaitReply : kAEQueueReply ) | kAECanInteract, 
			kAENormalPriority, 
			5 * 60 * 60
		);
		
		return N::AEGetAttributePtr< typeSInt32 >( appleEvent, keyReturnIDAttr );
	}
	*/
	
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
	
	static FSSpec DirLookup( const N::FSDirSpec& dir, const std::string& name )
	{
		return dir & name;
	}
	
	static NN::Owned< FSSpec > NewTempFile( const FSSpec& item )
	{
		if ( N::FSpTestFileExists( item ) )
		{
			N::FSpDelete( item );
		}
		
		N::FSpCreate( item, 'R*ch', 'TEXT' );
		
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
	
	static void DumpFile( const FSSpec& file, int fd )
	{
		NN::Owned< N::FSFileRefNum > fileH = N::FSpOpenDF( file, fsRdPerm );
		
		std::size_t length = N::GetEOF( fileH );
		
		if ( length == 0 )
		{
			return;
		}
		
		std::vector< char > v( length );
		
		int bytes = FS::Read( fileH, &v[ 0 ], v.size() );
		
		ASSERT( bytes == v.size() );
		
		std::replace( v.begin(), v.end(), '\r', '\n' );
		
		write( fd, &v[ 0 ], bytes );
	}
	
	int RunCommandInToolServer( const std::string& command )
	{
		int result = GetResult( AESendBlocking( CreateScriptEvent( SetUpScript( command ) ) ) );
		
		/*
		Z::Follower tailOut( gTempFiles[ kOutputFile ] );
		Z::Follower tailErr( gTempFiles[ kErrorFile  ] );
		
		tailErr.Advance( Io::Err );
		tailOut.Advance( Io::Out );
		*/
		
		DumpFile( gTempFiles[ kErrorFile  ], Io::err );
		DumpFile( gTempFiles[ kOutputFile ], Io::out );
		
		// Delete temp files
		std::fill( gTempFiles,
		           gTempFiles + 4,
		           NN::Shared< FSSpec >() );
		
		return result;
	}
	
}

