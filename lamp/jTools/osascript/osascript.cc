/*	============
 *	osascript.cc
 *	============
 */

// Standard C++
#include <string>
#include <vector>

// MoreFiles
//#include "MoreFilesExtras.h"

// Nitrogen core
#include "Nitrogen/Assert.h"

// Nitrogen / Mac OS support
#include "Nitrogen/OSA.h"
#include "Nitrogen/Resources.h"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"

// Zion
#include "Zion/FileHandle.hh"
#include "Zion/Platform.hh"


namespace N = Nitrogen;
namespace O = Orion;
namespace Z = Zion;


static std::string ReadFileData( const FSSpec& file )
{
	N::Owned< N::FSFileRefNum > fileH( Z::OpenFileReadOnly( file ) );
	
	UInt32 fileSize = N::GetEOF( fileH );
	
	std::vector< char > data( fileSize );
	
	int bytes = Io::Read( fileH, &data[ 0 ], fileSize );
	
	ASSERT( bytes == fileSize );
	
	return std::string( &data[ 0 ], bytes );
}

static N::Owned< N::OSASpec > CompileSource( const AEDesc& source )
{
	return N::OSACompile( N::OpenDefaultComponent( kOSAComponentType,
	                                               kOSAGenericScriptingComponentSubtype ),
	                      source );
}

static N::Owned< N::OSASpec > LoadCompiledScript( const FSSpec& scriptFile )
{
	N::Owned< N::ResFileRefNum > resFileH( N::FSpOpenResFile( scriptFile,
	                                                          fsRdPerm ) );
	
	return N::OSALoad( N::OpenGenericScriptingComponent(),
	                   N::AECreateDesc( typeOSAGenericStorage,
	                                    N::Get1Resource( kOSAScriptResourceType,
	                                                     N::ResID( 128 ) ) ) );
}

static N::Owned< N::OSASpec > LoadScriptFile( const FSSpec& scriptFile )
{
	OSType type = N::FSpGetFInfo( scriptFile ).fdType;
	
	switch ( type )
	{
		case kOSAFileType:
			return LoadCompiledScript( scriptFile );
			break;
		
		case 'TEXT':
			return CompileSource( N::AECreateDesc< typeChar >( ReadFileData( scriptFile ) ) );
			break;
	}
	
	return N::Owned< N::OSASpec >();
}

enum
{
	optInlineScript,
	optResultFormat
};

enum
{
	resultsHumanReadable,
	resultsRecompilableSource
};

static O::Options DefineOptions()
{
	O::Options options;
	
	options.DefineAppendToStringList( "-e", optInlineScript );
	
	options.DefineSelectEnum( "-h", optResultFormat, resultsHumanReadable      );
	options.DefineSelectEnum( "-s", optResultFormat, resultsRecompilableSource );
	
	return options;
}

int O::Main( int argc, const char *const argv[] )
{
	O::Options options = DefineOptions();
	options.GetOptions( argc, argv );
	
	const std::vector< const char* >& params = options.GetFreeParams();
	
	N::Owned< N::OSASpec > script;
	std::vector< std::string > inlineScriptPieces = options.GetStringList( optInlineScript );
	
	if ( !inlineScriptPieces.empty() )
	{
		// FIXME:  accumulate -e
		script = CompileSource( N::AECreateDesc< typeChar >( inlineScriptPieces[ 0 ] ) );
	}
	else
	{
		if ( !params.empty() )
		{
			script = LoadScriptFile( Path2FSS( params[ 0 ] ) );
		}
	}
	
	N::Owned< N::OSASpec > result = N::OSAExecute( script );
	
	if ( result.Get().id != kOSANullScript )
	{
		Io::Out << N::AEGetDescData< typeChar >( N::OSADisplay( result ) ) << "\n";
	}
	
	return 0;
}

