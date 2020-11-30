/*
	mxcpp.cc
	--------
	
	DONE:
		continued lines
		line comments
		block comments
		directive recognition
		basic macro management (#define, #undef, test definedness)
		conditional stack
		#if expression evaluation
//		#include
		simple macro expansion
		defined()
		macro reexpansion
		function-like macro expansion
		options (-D, -U, -I)
		include search
		__option()
		accurate #line directive output
		#line directive input
		#pragma
		#pragma once
		Metrowerks options/pragmata with static defaults
		Metrowerks options/pragmata with defaults depending on circumstances
		pragma-tracking macros
		predefined macros (can't be undefined, expansion may vary)
			Standard C
			__DATE__:  varies by translation
			__TIME__:  varies by translation
			__FILE__:  varies by source file
			__LINE__:  varies by line
			__STDC__:  opposes __cplusplus
			C++
			__cplusplus:  affected by #pragma cplusplus
			68K
			__A5__
			__fourbyteints__
			__IEEEdoubles__
			__MC68K__
			__MC68020__
			__MC68881__
			PowerPC
			__powerc (not powerc)
			__POWERPC__
			Metrowerks
			__MWERKS__
			__MWBROWSER__:  0
			__profile__
			Macintosh
			__MACOS__ (not macintosh if at all possible)
			Other
			__BIG_ENDIAN__:  1
	
	TODO:
		#warning/error
*/

/*
	Rationale
	---------
	
	* ccache/distcc require standalone cpp
	* Metrowerks built-in preprocessor in standalone mode fails
		* omits #pragma directives, needed for codegen -- game over
	* cpp must support Metrowerks pragmata and __option()
*/

// Standard C
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// plus
#include "plus/string/concat.hh"

// poseven
#include "poseven/functions/dup2.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/rename.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"

// mxcpp
#include "config.hh"
#include "include.hh"
#include "macro.hh"
#include "preprocess.hh"


using namespace command::constants;

enum
{
	Opt_include = 'I',
	Opt_define  = 'D',
	Opt_undef   = 'U',
	
	Opt_Unix_lines = 'L',
	Opt_Mac_lines  = 'M',
	Opt_no_lines   = 'N',
	
	Opt_debug   = 'd',
	Opt_output  = 'o',
	
	Opt_last_byte = 255,
	
	Opt_A4,
	Opt_68K,
	Opt_CFM,
	Opt_PPC,
	
	Opt_cplusplus,
	Opt_precompile,
	Opt_CR_newlines,
};

static command::option options[] =
{
	{ "a4",  Opt_A4  },
	{ "68k", Opt_68K },
	{ "cfm", Opt_CFM },
	{ "ppc", Opt_PPC },
	
	{ "cr", Opt_CR_newlines },
	{ "c++", Opt_cplusplus },
	{ "precompile", Opt_precompile },
	
	{ "unix-lines", Opt_Unix_lines },
	{ "mac-lines",  Opt_Mac_lines  },
	{ "no-lines",   Opt_no_lines   },
	
	{ "", Opt_debug },
	
	{ "", Opt_include, Param_required },
	{ "", Opt_output,  Param_required },
	{ "", Opt_define,  Param_required },
	{ "", Opt_undef,   Param_required },
	
	{ NULL }
};

static const char* output_path = NULL;

static bool output_carriage_returns = false;

using namespace tool;

static inline void set_line_style( line_style style )
{
	global_line_style = style;
}

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			using command::global_result;
			
			case Opt_A4:   global_config_a4  = true;  break;
			case Opt_68K:  global_config_68k = true;  break;
			case Opt_CFM:  global_config_cfm = true;  break;
			case Opt_PPC:  global_config_powerpc = true;  break;
			
			case Opt_debug:       global_config_debug      = true;  break;
			case Opt_cplusplus:   global_config_cplusplus  = true;  break;
			case Opt_precompile:  global_config_precompile = true;  break;
			case Opt_CR_newlines:  output_carriage_returns = true;  break;
			
			case Opt_Unix_lines:  set_line_style( unix_line_directives ); break;
			case Opt_Mac_lines:   set_line_style( mac_line_directives  ); break;
			case Opt_no_lines:    set_line_style( no_line_directives   ); break;
			
			case Opt_include:
				global_include_search_paths.push_back( global_result.param );
				break;
			
			case Opt_output:
				output_path = global_result.param;
				break;
			
			case Opt_define:
				if ( const char* eq = strchr( global_result.param, '=' ) )
				{
					plus::string name( global_result.param, eq );
					
					const char* param = eq + 1;
					
					define_macro( name.c_str(), param );
				}
				else
				{
					define_macro( global_result.param, "1" );
				}
				break;
			
			case Opt_undef:
				undef_macro( global_result.param );
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static void write_line( char* buffer, size_t length, char newline )
	{
		buffer[ length++ ] = newline;
		
		p7::write( p7::stdout_fileno, buffer, length );
		
	}
	
	int Main( int argc, char* argv[] )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( global_config_powerpc )
		{
			global_config_cfm = true;
			
			if ( global_config_68k )
			{
				// complain
			}
		}
		
		if ( output_carriage_returns )
		{
			global_newline_char = '\r';
		}
		
		plus::string temp_path;
		
		if ( output_path )
		{
			temp_path = plus::concat( output_path, strlen( output_path ), STR_LEN( "~" ) );
			
			p7::dup2( p7::open( temp_path,
			                    p7::o_wronly | p7::o_creat,
			                    p7::_666 ),
			          p7::stdout_fileno );
		}
		
		char pragma_cplusplus[] = "#pragma cplusplus off";
		
		char* p = pragma_cplusplus + sizeof pragma_cplusplus - 1;
		
		if ( global_config_cplusplus )
		{
			p -= STRLEN( "ff" );
			
			*p++ = 'n';
		}
		
		write_line( pragma_cplusplus, p - pragma_cplusplus, global_newline_char );
		
		if ( global_config_cplusplus )
		{
			char pragma_bool      [] = "#pragma bool on";
			char pragma_wchar_type[] = "#pragma wchar_type off";
			
			write_line( pragma_bool,       sizeof pragma_bool       - 1, global_newline_char );
			write_line( pragma_wchar_type, sizeof pragma_wchar_type - 1, global_newline_char );
		}
		
		for ( unsigned i = 0;  i < argn;  ++i )
		{
			preprocess_file( args[ i ] );
		}
		
		if ( output_path )
		{
			p7::rename( temp_path, output_path );
		}
		
		return 0;
	}

}
