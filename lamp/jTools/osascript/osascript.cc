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

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/OSA.hh"
#include "Nitrogen/Resources.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace Div = Divergence;
	namespace o = orion;
	
	
	static void ReportAndThrowScriptError( N::ComponentInstance comp, const char* step )
	{
		SInt16       errorNumber  = N::OSAScriptError< N::kOSAErrorNumber  >( comp );
		std::string  errorMessage = N::OSAScriptError< N::kOSAErrorMessage >( comp );
		
		if ( errorNumber < 0 )
		{
			std::fprintf( stderr, "OSA script error %d during %s:\n", errorNumber, step );
		}
		
		errorMessage += "\n";
		
		p7::write( p7::stderr_fileno, errorMessage );
		
		throw errorNumber > 0 ? p7::exit_t( errorNumber )
		                      : p7::exit_failure;
	}
	
	static inline n::owned< N::ComponentInstance > OpenGenericScriptingComponent()
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
			result = p7::slurp( file );
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
	
	static n::owned< N::OSAID > MakeCWDContext( const n::shared< N::ComponentInstance >& scriptingComponent )
	{
		char stupid_buffer[ 1024 ];
		char* gotcwd = getcwd( stupid_buffer, 1024 );
		
		if ( gotcwd == NULL )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "getcwd() returned NULL, sorry\n" ) );
			
			throw p7::exit_failure;
		}
		
		std::string cwd( stupid_buffer );
		
		std::string cwdProperty = "property gCurrentWorkingDirectory : \"" + cwd + "\"";
		
		
		return
		N::OSACompile( scriptingComponent,
		               N::AECreateDesc< N::typeChar >( cwdProperty ),
		               N::kOSAModeCompileIntoContext );
	}
	
	static n::owned< N::OSAID > CompileSource( const AEDesc& source, bool useCWD )
	{
		n::shared< N::ComponentInstance > scriptingComponent = OpenGenericScriptingComponent();
		
		const char* step;
		
		try
		{
			N::OSAModeFlags mode = N::kOSAModeCompileIntoContext;
			
			n::owned< N::OSAID > scriptContext;
			
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
		catch ( const N::OSStatus& err )
		{
			if ( err != errOSAScriptError )
			{
				throw;
			}
		}
		
		ReportAndThrowScriptError( scriptingComponent, step );
		
		// Not reached
		return n::owned< N::OSAID >();
	}
	
	static n::owned< N::OSAID > LoadCompiledScript( const FSSpec& scriptFile )
	{
		using N::fsRdPerm;
		
		n::owned< N::ResFileRefNum > resFileH( N::FSpOpenResFile( scriptFile,
		                                                          fsRdPerm ) );
		
		return N::OSALoad( OpenGenericScriptingComponent(),
		                   N::AECreateDesc< N::AEDesc_Data >( N::typeOSAGenericStorage,
		                                    N::Get1Resource( N::kOSAScriptResourceType,
		                                                     N::ResID( 128 ) ) ) );
	}
	
	static n::owned< N::OSAID > LoadScriptFile( const char* pathname, bool useCWD )
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
	
	
	static inline std::size_t total_string_size( std::size_t total, const std::string& string )
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
	
	
	int Main( int argc, iota::argv_t argv )
	{
		std::vector< std::string > inlineScriptPieces;
		
		// human-readable by default, like Apple osascript
		bool humanReadable = true;
		bool getsCWDProperty = false;
		
		o::bind_option_to_variable( "-e", inlineScriptPieces );
		
		o::bind_option_to_variable( "-h", humanReadable, true  );
		o::bind_option_to_variable( "-s", humanReadable, false );
		
		o::bind_option_to_variable( "--cwd", getsCWDProperty );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		typedef char const *const *const_iterator;
		
		const_iterator params_begin = free_args;
		const_iterator params_end   = free_args + o::free_argument_count();
		
		n::owned< N::OSAID > script;
		
		if ( !inlineScriptPieces.empty() )
		{
			script = CompileSource( N::AECreateDesc< N::typeChar >( JoinScriptPieces( inlineScriptPieces ) ), getsCWDProperty );
		}
		else
		{
			const char* pathname = "/dev/fd/0";
			
			if ( *free_args != NULL )
			{
				pathname = free_args[ 0 ];
				
				++params_begin;
			}
			
			script = LoadScriptFile( pathname, getsCWDProperty );
		}
		
		n::owned< N::AppleEvent > runEvent = N::AECreateAppleEvent( N::kCoreEventClass,
		                                                            N::kAEOpenApplication,
		                                                            NN::Make< N::AEAddressDesc >() );
		
		// Add the list, even if there are zero parameters.
		{
			n::owned< N::AEDescList_Data > list = N::AECreateList< N::AEDescList_Data >( false );
			
			for ( const_iterator it = params_begin;  it != params_end;  ++it )
			{
				N::AEPutPtr< N::typeChar >( list, 0, *it );
			}
			
			N::AEPutParamDesc( runEvent, N::keyDirectObject, list );
		}
		
		try
		{
			n::owned< N::OSAID > result = N::OSAExecuteEvent( runEvent, script );
			
			if ( result.get() != N::kOSANullScript )
			{
				N::OSAModeFlags displayFlags( humanReadable ? N::kOSAModeDisplayForHumans : N::kOSAModeNull );
				
				std::string output = N::AEGetDescData< N::typeChar >( N::OSADisplay( result,
				                                                                     N::typeChar,
				                                                                     displayFlags ) );
				
				if ( *output.rbegin() != '\n' )
				{
					output += "\n";
				}
				
				p7::write( p7::stdout_fileno, output );
			}
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != errOSAScriptError )
			{
				throw;
			}
			
			ReportAndThrowScriptError( script.disposer().Component(), "execution" );
		}
		
		return 0;
	}

}

