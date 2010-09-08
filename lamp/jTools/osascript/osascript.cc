/*	============
 *	osascript.cc
 *	============
 */

// Standard C++
#include <functional>
#include <numeric>
#include <vector>

// Standard C/C++
#include <cstdio>

// Standard C
#include <string.h>

// POSIX
#include <unistd.h>

// iota
#include "iota/convert_string.hh"
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

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
	namespace p7 = poseven;
	namespace Div = Divergence;
	namespace o = orion;
	
	
	static void ReportAndThrowScriptError( N::ComponentInstance comp, const char* step )
	{
		SInt16                   errorNumber  = N::OSAScriptError< N::kOSAErrorNumber  >( comp );
		nucleus::mutable_string  errorMessage = N::OSAScriptError< N::kOSAErrorMessage >( comp );
		
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
	
	static plus::string ReadUntilEOF( p7::fd_t stream )
	{
		plus::var_string result;
		
		const std::size_t block_size = 4096;
		
		char buffer[ block_size ];
		
		while ( std::size_t bytes = p7::read( stream, buffer, block_size ) )
		{
			result.append( buffer, bytes );
		}
		
		return result;
	}
	
	static nucleus::string ReadFileData( const char* file )
	{
		plus::string slurped;
		
		try
		{
			slurped = p7::slurp( file );
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ESPIPE )
			{
				throw;	
			}
			
			slurped = ReadUntilEOF( p7::open( file, p7::o_rdonly ) );
		}
		
		nucleus::mutable_string result = iota::convert_string< nucleus::string >( slurped );
		
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
		
		nucleus::string cwd( stupid_buffer );
		
		nucleus::string cwdProperty = "property gCurrentWorkingDirectory : \"" + cwd + "\"";
		
		
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
	
	
	static inline std::size_t total_string_size( std::size_t total, const char* s )
	{
		return total + strlen( s );
	}
	
	static plus::string JoinScriptPieces( const std::vector< const char* >& pieces )
	{
		ASSERT( !pieces.empty() );
		
		if ( pieces.size() == 1 )
		{
			return pieces[0];
		}
		
		std::size_t total_length = std::accumulate( pieces.begin(),
		                                            pieces.end(),
		                                            pieces.size(),  // add 1 byte for each CR
		                                            std::ptr_fun( total_string_size ) );
		
		plus::var_string result;
		
		result.resize( total_length );
		
		plus::var_string::iterator there = result.begin();
		
		typedef std::vector< const char* >::const_iterator Iter;
		
		for ( Iter it  = pieces.begin();  it != pieces.end();  ++it )
		{
			const char* string = *it;
			
			const size_t length = strlen( string );
			
			std::copy( string, string + length, there );
			
			there += length;
			
			*there++ = '\r';
		}
		
		return result;
	}
	
	
	int Main( int argc, char** argv )
	{
		std::vector< const char* > inlineScriptPieces;
		
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
			plus::string joined_script = JoinScriptPieces( inlineScriptPieces );
			
			nucleus::string converted_string = iota::convert_string< nucleus::string >( joined_script );
			
			script = CompileSource( N::AECreateDesc< N::typeChar >( converted_string ), getsCWDProperty );
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
		                                                            n::make< N::AEAddressDesc >() );
		
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
				
				nucleus::mutable_string output = N::AEGetDescData< N::typeChar >( N::OSADisplay( result,
				                                                                                 N::typeChar,
				                                                                                 displayFlags ) );
				
				if ( *(output.end() - 1) != '\n' )
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

