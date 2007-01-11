/*	============
 *	osascript.cc
 *	============
 */

// Standard C++
#include <string>
#include <vector>

// POSIX
#include "unistd.h"

// Nucleus
#include "Nucleus/NAssert.h"
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/OSA.h"
#include "Nitrogen/Resources.h"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace Div = Divergence;
namespace O = Orion;


template < N::DescType desiredType >
typename N::DescType_Traits< desiredType >::Result
GetScriptErrorData( N::ComponentInstance scriptingComponent, N::AEKeyword keyword )
{
	return N::AEGetDescData< desiredType >( N::OSAScriptError( scriptingComponent,
	                                                           keyword,
	                                                           desiredType ) );
}

static void ReportAndThrowScriptError( N::ComponentInstance comp, const char* step )
{
	SInt16       errorNumber  = GetScriptErrorData< N::typeSInt16 >( comp, N::AEKeyword( kOSAErrorNumber  ) );
	std::string  errorMessage = GetScriptErrorData< N::typeChar   >( comp, N::AEKeyword( kOSAErrorMessage ) );
	
	if ( errorNumber < 0 )
	{
		Io::Err << "OSA script error " << errorNumber << " during " << step << ":" << "\n";
	}
	
	Io::Err << errorMessage << "\n";
	
	O::ThrowExitStatus( errorNumber > 0 ? errorNumber : 1 );
}

inline NN::Owned< N::ComponentInstance > OpenGenericScriptingComponent()
{
	return N::OpenDefaultComponent( kOSAComponentType,
	                                kOSAGenericScriptingComponentSubtype );
}

static std::string ReadFileData( const FSSpec& file )
{
	using N::fsRdPerm;
	
	NN::Owned< N::FSFileRefNum > fileH( N::FSpOpenDF( file, fsRdPerm ) );
	
	std::size_t fileSize = N::GetEOF( fileH );
	
	std::vector< char > data( fileSize );
	
	std::size_t bytes = N::FSRead( fileH, fileSize, &data[ 0 ] );
	
	ASSERT( bytes == fileSize );
	
	if ( data.size() >= 2  &&  data[0] == '#'  &&  data[1] == '!' )
	{
		data[0] = data[1] = '-';  // Turn she-bang line into comment 
	}
	
	return std::string( &data[ 0 ], bytes );
}

static NN::Owned< N::OSASpec > MakeCWDContext()
{
	char stupid_buffer[ 1024 ];
	char* gotcwd = getcwd( stupid_buffer, 1024 );
	
	if ( gotcwd == NULL )
	{
		Io::Err << "getcwd() returned NULL, sorry\n";
		
		O::ThrowExitStatus( 1 );
	}
	
	std::string cwd( stupid_buffer );
	
	std::string cwdProperty = "property gCurrentWorkingDirectory : \"" + cwd + "\"";
	
	
	return
	N::OSACompile( OpenGenericScriptingComponent(),
	               N::AECreateDesc< N::typeChar >( cwdProperty ),
	               N::OSAModeFlags( kOSAModeCompileIntoContext ) );
}

static NN::Owned< N::OSASpec > CompileSource( const AEDesc& source )
{
	NN::Owned< N::OSASpec > cwdContext = MakeCWDContext();
	
	NN::Shared< N::ComponentInstance > scriptingComponent = cwdContext.Get().component;
	
	try
	{
		return N::OSACompile( scriptingComponent,
							  source,
							  N::OSAModeFlags( kOSAModeAugmentContext ),
							  cwdContext );
	}
	catch ( const N::ErrOSAScriptError& )  {}
	
	ReportAndThrowScriptError( scriptingComponent, "compilation" );
	
	// Not reached
	return NN::Owned< N::OSASpec >();
}

static NN::Owned< N::OSASpec > LoadCompiledScript( const FSSpec& scriptFile )
{
	using N::fsRdPerm;
	
	NN::Owned< N::ResFileRefNum > resFileH( N::FSpOpenResFile( scriptFile,
	                                                           fsRdPerm ) );
	
	return N::OSALoad( OpenGenericScriptingComponent(),
	                   N::AECreateDesc( N::typeOSAGenericStorage,
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
			return CompileSource( N::AECreateDesc< N::typeChar >( ReadFileData( scriptFile ) ) );
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
	
	typedef std::vector< const char* >::const_iterator const_iterator;
	
	const_iterator params_begin = params.begin();
	const_iterator params_end   = params.end  ();
	
	NN::Owned< N::OSASpec > script;
	std::vector< std::string > inlineScriptPieces = options.GetStringList( optInlineScript );
	
	if ( !inlineScriptPieces.empty() )
	{
		// FIXME:  accumulate -e
		script = CompileSource( N::AECreateDesc< N::typeChar >( inlineScriptPieces[ 0 ] ) );
	}
	else
	{
		if ( !params.empty() )
		{
			script = LoadScriptFile( Div::ResolvePathToFSSpec( params[ 0 ] ) );
			++params_begin;
		}
	}
	
	NN::Owned< N::AppleEvent > runEvent = N::AECreateAppleEvent( kCoreEventClass,
	                                                             kAEOpenApplication,
	                                                             NN::Make< N::AEAddressDesc >() );
	
	// Add the list, even if there are zero parameters.
	{
		NN::Owned< N::AEDescList > list = N::AECreateList< false >();
		
		for ( const_iterator it = params_begin;  it != params_end;  ++it )
		{
			N::AEPutPtr< N::typeChar >( list, 0, *it );
		}
		
		N::AEPutParamDesc( runEvent, N::keyDirectObject, list );
	}
	
	try
	{
		NN::Owned< N::OSASpec > result = N::OSAExecuteEvent( runEvent, script );
		
		if ( result.Get().id != kOSANullScript )
		{
			// human-readable by default, like Apple osascript
			bool humanReadable = options.GetEnum( optResultFormat ) != resultsRecompilableSource;
			
			N::OSAModeFlags displayFlags( humanReadable ? kOSAModeDisplayForHumans : kOSAModeNull );
			
			std::string output = N::AEGetDescData< N::typeChar >( N::OSADisplay( result,
			                                                                     N::typeChar,
			                                                                     displayFlags ) );
			
			if ( *output.rbegin() != '\n' )
			{
				output += "\n";
			}
			
			Io::Out << output;
		}
	}
	catch ( const N::ErrOSAScriptError& )
	{
		ReportAndThrowScriptError( script.Get().component, "execution" );
	}
	
	return 0;
}

