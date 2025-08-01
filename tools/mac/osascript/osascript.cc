/*
	osascript.cc
	------------
*/

// Standard C++
#include <algorithm>

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <unistd.h>

// iota
#include "iota/convert_string.hh"
#include "iota/strings.hh"

// more-libc
#include "more/string.h"

// command
#include "command/get_option.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"

// plus
#include "plus/mac_utf8.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Nitrogen
#include "Mac/AppleEvents/Types/AEKeyword_scribe_text.hh"

#include "Nitrogen/OSA.hh"
#include "Nitrogen/Resources.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_inline_script   = 'e',
	Option_human_readable  = 'h',
	Option_script_readable = 's',
	
	Option_last_byte = 255,
	
	Option_pass_cwd,
};

static command::option options[] =
{
	{ "cwd", Option_pass_cwd },
	
	{ "", Option_human_readable  },
	{ "", Option_script_readable },
	
	{ "", Option_inline_script, Param_required },
	
	{ NULL }
};

// human-readable by default, like Apple osascript
static bool humanReadable = true;
static bool getsCWDProperty = false;

static const char** inline_script_begin;
static const char** inline_script_end;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	inline_script_begin =
	inline_script_end   = (const char**) argv;
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_pass_cwd:
				getsCWDProperty = true;
				break;
			
			case Option_human_readable:
				humanReadable = true;
				break;
			
			case Option_script_readable:
				humanReadable = false;
				break;
			
			case Option_inline_script:
				*inline_script_end++ = command::global_result.param;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	template < Mac::DescType desiredType >
	static inline
	typename Mac::DescType_result< desiredType >::type
	OSAScriptError( ComponentInstance scriptingComponent, Mac::AEKeyword keyword )
	{
		return N::AEGetDescData< desiredType >( N::OSAScriptError( scriptingComponent,
		                                                           keyword,
		                                                           desiredType ) );
	}
	
	static inline OSStatus
	//
	OSAScriptError_number( ComponentInstance comp )
	{
		const Mac::AEKeyword key = Mac::kOSAErrorNumber;
		const Mac::DescType type = Mac::typeSInt32;
		
		return OSAScriptError< type >( comp, key );
	}
	
	static inline nucleus::mutable_string
	//
	OSAScriptError_string( ComponentInstance comp )
	{
		const Mac::AEKeyword key = Mac::kOSAErrorMessage;
		const Mac::DescType type = Mac::typeText;
		
		return OSAScriptError< type >( comp, key );
	}
	
	static void ReportAndThrowScriptError( ComponentInstance comp, const char* step )
	{
		SInt16                   errorNumber  = OSAScriptError_number( comp );
		nucleus::mutable_string  errorMessage = OSAScriptError_string( comp );
		
		if ( errorNumber < 0 )
		{
			fprintf( stderr, "OSA script error %d during %s:\n", errorNumber, step );
		}
		
		errorMessage += "\n";
		
		p7::write( p7::stderr_fileno, errorMessage.data(), errorMessage.size() );
		
		throw errorNumber > 0 ? p7::exit_t( errorNumber )
		                      : p7::exit_failure;
	}
	
	static inline n::owned< ComponentInstance > OpenGenericScriptingComponent()
	{
		return N::OpenDefaultComponent( N::kOSAComponentType,
		                                N::kOSAGenericScriptingComponentSubtype );
	}
	
	static nucleus::string ReadFileData( const char* file )
	{
		plus::string slurped = p7::slurp( file );
		
		try
		{
			slurped = plus::mac_from_utf8( slurped );
		}
		catch ( ... )
		{
			// Invalid UTF-8, probably MacRoman
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
	
	static n::owned< N::OSAID > MakeCWDContext( const n::shared< ComponentInstance >& scriptingComponent )
	{
		char stupid_buffer[ 1024 ];
		char* gotcwd = getcwd( stupid_buffer, 1024 );
		
		if ( gotcwd == NULL )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "getcwd() returned NULL, sorry\n" ) );
			
			throw p7::exit_failure;
		}
		
		nucleus::string cwd( stupid_buffer );
		
		nucleus::mutable_string cwdProperty;
		
		cwdProperty += "property gCurrentWorkingDirectory : \"";
		cwdProperty += cwd;
		cwdProperty += "\"";
		
		
		return
		N::OSACompile( scriptingComponent,
		               N::AECreateDesc< Mac::typeChar >( cwdProperty ),
		               N::kOSAModeCompileIntoContext );
	}
	
	static n::owned< N::OSAID > CompileSource( const AEDesc& source, bool useCWD )
	{
		n::shared< ComponentInstance > scriptingComponent = OpenGenericScriptingComponent();
		
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
		catch ( const Mac::OSStatus& err )
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
		using Mac::fsRdPerm;
		
		n::owned< N::ResFileRefNum > resFileH( N::FSpOpenResFile( scriptFile,
		                                                          fsRdPerm ) );
		
		return N::OSALoad( OpenGenericScriptingComponent(),
		                   N::AECreateDesc< Mac::AEDesc_Data >( Mac::typeOSAGenericStorage,
		                                    N::Get1Resource( Mac::kOSAScriptResourceType,
		                                                     N::ResID( 128 ) ) ) );
	}
	
	static n::owned< N::OSAID > LoadScriptFile( const char* pathname, bool useCWD )
	{
		using mac::relix::FSSpec_from_existing_path;
		
		using mac::sys::Error;
		
		FSSpec scriptFile;
		FInfo  fInfo;
		
		Error err = FSSpec_from_existing_path( pathname, scriptFile );
		
		if ( ! err )
		{
			err = (Error) FSpGetFInfo( &scriptFile, &fInfo );
		}
		
		OSType type = err ? '\0\0\0\0' : fInfo.fdType;
		
		try
		{
			if ( type == kOSAFileType )
			{
				return LoadCompiledScript( scriptFile );
			}
		}
		catch ( ... )
		{
		}
		
		return CompileSource( N::AECreateDesc< Mac::typeChar >( ReadFileData( pathname ) ), useCWD );
	}
	
	
	static
	plus::string JoinScriptPieces( const char** pieces, int count )
	{
		ASSERT( count > 0 );
		
		if ( count == 1 )
		{
			return pieces[0];
		}
		
		size_t total_length = count;  // add 1 byte for each CR
		
		for ( size_t i = 0;  i < count;  ++i )
		{
			total_length += strlen( pieces[ i ] );
		}
		
		plus::string result;
		
		char* there = result.reset( total_length );
		
		for ( size_t i = 0;  i < count;  ++i )
		{
			const char* string = pieces[ i ];
			
			const size_t length = strlen( string );
			
			there = (char*) mempcpy( there, string, length );
			
			*there++ = '\r';
		}
		
		return result;
	}
	
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		char const *const *free_args = args;
		
		typedef char const *const *const_iterator;
		
		const_iterator params_begin = free_args;
		const_iterator params_end   = free_args + argn;
		
		n::owned< N::OSAID > script;
		
		if ( int count = inline_script_end - inline_script_begin )
		{
			const char** begin = inline_script_begin;
			
			plus::string joined_script = JoinScriptPieces( begin, count );
			
			joined_script = plus::mac_from_utf8( joined_script );
			
			nucleus::string converted_string = iota::convert_string< nucleus::string >( joined_script );
			
			script = CompileSource( N::AECreateDesc< Mac::typeChar >( converted_string ), getsCWDProperty );
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
		
		n::owned< Mac::AppleEvent > runEvent = N::AECreateAppleEvent( Mac::kCoreEventClass,
		                                                              Mac::kAEOpenApplication,
		                                                              n::make< Mac::AEAddressDesc >() );
		
		// Add the list, even if there are zero parameters.
		{
			n::owned< Mac::AEDescList_Data > list = N::AECreateList< Mac::AEDescList_Data >( false );
			
			for ( const_iterator it = params_begin;  it != params_end;  ++it )
			{
				N::AEPutPtr< Mac::typeChar >( list, 0, *it );
			}
			
			N::AEPutParamDesc( runEvent, Mac::keyDirectObject, list );
		}
		
		try
		{
			n::owned< N::OSAID > result = N::OSAExecuteEvent( runEvent, script );
			
			if ( result.get() != N::kOSANullScript )
			{
				N::OSAModeFlags displayFlags( humanReadable ? N::kOSAModeDisplayForHumans : N::kOSAModeNull );
				
				nucleus::mutable_string output = N::AEGetDescData< Mac::typeChar >( N::OSADisplay( result,
				                                                                                   Mac::typeChar,
				                                                                                   displayFlags ) );
				
				if ( *(output.end() - 1) != '\n' )
				{
					output += "\n";
				}
				
				plus::string converted = iota::convert_string< plus::string >( output );
				
				plus::string utf8_output = plus::utf8_from_mac( converted );
				
				p7::write( p7::stdout_fileno, utf8_output );
			}
		}
		catch ( const Mac::OSStatus& err )
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
