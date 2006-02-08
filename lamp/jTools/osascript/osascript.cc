/*	============
 *	osascript.cc
 *	============
 */

// Standard C++
#include <string>
#include <vector>

// Nitrogen Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen / Mac OS support
#include "Nitrogen/OSA.h"
#include "Nitrogen/Resources.h"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;


static std::string ReadFileData( const FSSpec& file )
{
	NN::Owned< N::FSFileRefNum > fileH( N::FSpOpenDF( file, fsRdPerm ) );
	
	std::size_t fileSize = N::GetEOF( fileH );
	
	std::vector< char > data( fileSize );
	
	std::size_t bytes = N::FSRead( fileH, fileSize, &data[ 0 ] );
	
	ASSERT( bytes == fileSize );
	
	return std::string( &data[ 0 ], bytes );
}

static NN::Owned< N::OSASpec > CompileSource( const AEDesc& source )
{
	return N::OSACompile( N::OpenDefaultComponent( kOSAComponentType,
	                                               kOSAGenericScriptingComponentSubtype ),
	                      source );
}

static NN::Owned< N::OSASpec > LoadCompiledScript( const FSSpec& scriptFile )
{
	NN::Owned< N::ResFileRefNum > resFileH( N::FSpOpenResFile( scriptFile,
	                                                           fsRdPerm ) );
	
	return N::OSALoad( N::OpenGenericScriptingComponent(),
	                   N::AECreateDesc( typeOSAGenericStorage,
	                                    N::Get1Resource( kOSAScriptResourceType,
	                                                     N::ResID( 128 ) ) ) );
}

static NN::Owned< N::OSASpec > LoadScriptFile( const FSSpec& scriptFile )
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
	
	return NN::Owned< N::OSASpec >();
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
	
	NN::Owned< N::OSASpec > script;
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
	
	NN::Owned< N::OSASpec > result = N::OSAExecute( script );
	
	if ( result.Get().id != kOSANullScript )
	{
		Io::Out << N::AEGetDescData< typeChar >( N::OSADisplay( result ) ) << "\n";
	}
	
	return 0;
}

