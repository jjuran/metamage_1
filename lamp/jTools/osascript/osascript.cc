/*	============
 *	osascript.cc
 *	============
 */

// Standard C++
#include <functional>
#include <numeric>
#include <string>
#include <vector>

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/Exception.h"
#include "Nucleus/NAssert.h"
#include "Nucleus/Owned.h"
#include "Nucleus/TheExceptionBeingHandled.h"

// Io
#include "io/slurp.hh"

// POSeven
#include "POSeven/Open.hh"
#include "POSeven/Stat.hh"

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/OSA.h"
#include "Nitrogen/Resources.h"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace Nitrogen
{
	
	static const AEKeyword kOSAErrorNumber  = AEKeyword( ::kOSAErrorNumber  );
	static const AEKeyword kOSAErrorMessage = AEKeyword( ::kOSAErrorMessage );
	
	template < DescType desiredType >
	inline
	typename DescType_Traits< desiredType >::Result
	OSAScriptError( ComponentInstance scriptingComponent, AEKeyword keyword )
	{
		return AEGetDescData< desiredType >( OSAScriptError( scriptingComponent,
		                                                     keyword,
		                                                     desiredType ) );
	}
	
}

namespace N = Nitrogen;
namespace NN = Nucleus;
namespace p7 = poseven;
namespace Div = Divergence;
namespace O = Orion;


static void ReportAndThrowScriptError( N::ComponentInstance comp, const char* step )
{
	SInt16       errorNumber  = N::OSAScriptError< N::typeSInt16 >( comp, N::kOSAErrorNumber  );
	std::string  errorMessage = N::OSAScriptError< N::typeChar   >( comp, N::kOSAErrorMessage );
	
	if ( errorNumber < 0 )
	{
		std::fprintf( stderr, "OSA script error %d during %s:\n", errorNumber, step );
	}
	
	errorMessage += "\n";
	
	p7::write( p7::stderr_fileno, errorMessage.data(), errorMessage.size() );
	
	O::ThrowExitStatus( errorNumber > 0 ? errorNumber : 1 );
}

inline NN::Owned< N::ComponentInstance > OpenGenericScriptingComponent()
{
	return N::OpenDefaultComponent( N::kOSAComponentType,
	                                N::kOSAGenericScriptingComponentSubtype );
}

static std::string ReadUntilEOF( p7::fd_t stream )
{
	std::string result;
	
	const std::size_t block_size = 4096;
	
	char buffer[ block_size ];
	
	while ( std::size_t bytes = p7::read( stream, buffer, block_size ) )
	{
		result.append( buffer, bytes );
	}
	
	return result;
}

static std::string ReadFileData( const std::string& file )
{
	std::string result;
	
	try
	{
		result = io::slurp_file< NN::StringFlattener< std::string > >( file );
	}
	catch ( const p7::errno_t& err )
	{
		if ( err != ESPIPE )
		{
			throw;	
		}
		
		result = ReadUntilEOF( io::open_for_reading( file ) );
	}
	
	if ( result.size() >= 2  &&  result[0] == '#'  &&  result[1] == '!' )
	{
		result[0] = result[1] = '-';  // Turn she-bang line into comment 
	}
	
	// AppleScript 1.4.3 (in 9.0.4) hates linefeeds (1.8.3 in 9.2 is ok)
	std::replace( result.begin(),
	              result.end(),
	              '\n',
	              '\r' );
	
	return result;
}

static NN::Owned< N::OSASpec > MakeCWDContext( const NN::Shared< N::ComponentInstance >& scriptingComponent )
{
	char stupid_buffer[ 1024 ];
	char* gotcwd = getcwd( stupid_buffer, 1024 );
	
	if ( gotcwd == NULL )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "getcwd() returned NULL, sorry\n" ) );
		
		O::ThrowExitStatus( 1 );
	}
	
	std::string cwd( stupid_buffer );
	
	std::string cwdProperty = "property gCurrentWorkingDirectory : \"" + cwd + "\"";
	
	
	return
	N::OSACompile( scriptingComponent,
	               N::AECreateDesc< N::typeChar >( cwdProperty ),
	               N::kOSAModeCompileIntoContext );
}

static NN::Owned< N::OSASpec > CompileSource( const AEDesc& source, bool useCWD )
{
	NN::Shared< N::ComponentInstance > scriptingComponent = OpenGenericScriptingComponent();
	
	const char* step;
	
	try
	{
		N::OSAModeFlags mode = N::kOSAModeCompileIntoContext;
		
		NN::Owned< N::OSASpec > scriptContext;
		
		if ( useCWD )
		{
			step = "context compilation";
			
			scriptContext = MakeCWDContext( scriptingComponent );
			
			mode = N::kOSAModeAugmentContext;
		}
		
		step = "compilation";
		
		return N::OSACompile( scriptingComponent,
		                      source,
		                      mode,
		                      scriptContext );
	}
	catch ( const N::ErrOSAScriptError& err )
	{
	#ifdef __MWERKS__
		
		if ( err.Get() != errOSAScriptError )
		{
			throw;
		}
		
	#endif
	}
	
	ReportAndThrowScriptError( scriptingComponent, step );
	
	// Not reached
	return NN::Owned< N::OSASpec >();
}

static NN::Owned< N::OSASpec > LoadCompiledScript( const FSSpec& scriptFile )
{
	using N::fsRdPerm;
	
	NN::Owned< N::ResFileRefNum > resFileH( N::FSpOpenResFile( scriptFile,
	                                                           fsRdPerm ) );
	
	return N::OSALoad( OpenGenericScriptingComponent(),
	                   N::AECreateDesc< N::AEDesc_Data >( N::typeOSAGenericStorage,
	                                    N::Get1Resource( N::kOSAScriptResourceType,
	                                                     N::ResID( 128 ) ) ) );
}

static NN::Owned< N::OSASpec > LoadScriptFile( const char* pathname, bool useCWD )
{
	try
	{
		FSSpec scriptFile = Div::ResolvePathToFSSpec( pathname );
		
		OSType type = N::FSpGetFInfo( scriptFile ).fdType;
		
		if ( type == kOSAFileType )
		{
			return LoadCompiledScript( scriptFile );
		}
	}
	catch ( ... )
	{
	}
	
	return CompileSource( N::AECreateDesc< N::typeChar >( ReadFileData( pathname ) ), useCWD );
}


inline std::size_t total_string_size( std::size_t total, const std::string& string )
{
	return total + string.size();
}

static std::string JoinScriptPieces( const std::vector< std::string >& pieces )
{
	ASSERT( !pieces.empty() );
	
	std::size_t total_length = std::accumulate( pieces.begin(),
	                                            pieces.end(),
	                                            pieces.size(),  // add 1 byte for each CR
	                                            std::ptr_fun( total_string_size ) );
	
	std::string result;
	
	result.resize( total_length );
	
	std::string::iterator there = result.begin();
	
	typedef std::vector< std::string >::const_iterator Iter;
	
	std::vector< std::string >::const_iterator it = pieces.begin();
	
	for ( Iter it  = pieces.begin();  it != pieces.end();  ++it )
	{
		const std::string& string = *it;
		
		std::copy( string.begin(), string.end(), there );
		
		there += string.size();
		
		*there++ = '\r';
	}
	
	return result;
}


int O::Main( int argc, argv_t argv )
{
	NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
	
	std::vector< std::string > inlineScriptPieces;
	
	// human-readable by default, like Apple osascript
	bool humanReadable = true;
	bool getsCWDProperty = false;
	
	O::BindOption( "-e", inlineScriptPieces );
	
	O::BindOption( "-h", humanReadable, true  );
	O::BindOption( "-s", humanReadable, false );
	
	O::BindOption( "--cwd", getsCWDProperty );
	
	O::GetOptions( argc, argv );
	
	char const *const *freeArgs = O::FreeArguments();
	
	typedef char const *const *const_iterator;
	
	const_iterator params_begin = freeArgs;
	const_iterator params_end   = freeArgs + O::FreeArgumentCount();
	
	NN::Owned< N::OSASpec > script;
	
	if ( !inlineScriptPieces.empty() )
	{
		script = CompileSource( N::AECreateDesc< N::typeChar >( JoinScriptPieces( inlineScriptPieces ) ), getsCWDProperty );
	}
	else
	{
		const char* pathname = "/dev/fd/0";
		
		if ( *freeArgs != NULL )
		{
			pathname = freeArgs[ 0 ];
			
			++params_begin;
		}
		
		script = LoadScriptFile( pathname, getsCWDProperty );
	}
	
	NN::Owned< N::AppleEvent > runEvent = N::AECreateAppleEvent( N::kCoreEventClass,
	                                                             N::kAEOpenApplication,
	                                                             NN::Make< N::AEAddressDesc >() );
	
	// Add the list, even if there are zero parameters.
	{
		NN::Owned< N::AEDescList_Data > list = N::AECreateList< N::AEDescList_Data >( false );
		
		for ( const_iterator it = params_begin;  it != params_end;  ++it )
		{
			N::AEPutPtr< N::typeChar >( list, 0, *it );
		}
		
		N::AEPutParamDesc( runEvent, N::keyDirectObject, list );
	}
	
	try
	{
		NN::Owned< N::OSASpec > result = N::OSAExecuteEvent( runEvent, script );
		
		if ( result.Get().id != N::kOSANullScript )
		{
			N::OSAModeFlags displayFlags( humanReadable ? N::kOSAModeDisplayForHumans : N::kOSAModeNull );
			
			std::string output = N::AEGetDescData< N::typeChar >( N::OSADisplay( result,
			                                                                     N::typeChar,
			                                                                     displayFlags ) );
			
			if ( *output.rbegin() != '\n' )
			{
				output += "\n";
			}
			
			p7::write( p7::stdout_fileno, output.data(), output.size() );
		}
	}
	catch ( const N::ErrOSAScriptError& err )
	{
	#ifdef __MWERKS__
		
		if ( err.Get() != errOSAScriptError )
		{
			throw;
		}
		
	#endif
		
		ReportAndThrowScriptError( script.Get().component, "execution" );
	}
	
	return 0;
}

