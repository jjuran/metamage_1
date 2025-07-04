/*
	predefined.cc
	-------------
*/

#include "predefined.hh"

// Standard C++
#include <algorithm>

// Standard C
#include <string.h>
#include <time.h>

// more-libc
#include "more/string.h"

// iota
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"

// vxo
#include "vxo/lib/quote.hh"

// mxcpp
#include "config.hh"
#include "pragma.hh"
#include "preprocess.hh"


namespace tool
{
	
	typedef const char* (*predefined_handler)( const void* param );
	
	struct predefined_t
	{
		const char*         name;
		predefined_handler  handler;
		const void*         param;
	};
	
	static inline bool operator==( const predefined_t& p, const plus::string& s )
	{
		return p.name == s;
	}
	
	
	static
	const char* predefined_string( const void* param )
	{
		return (const char*) param;
	}
	
	static
	const char* predefined_option( const void* param )
	{
		const char* name = (const char*) param;
		
		return check_option( name ) ? "1" : "";
	}
	
	static
	const char* predefined_68k_option( const void* param )
	{
		const char* name = (const char*) param;
		
		return   !global_config_68k   ? ""
		       : check_option( name ) ? "1"
		       :                        "0";
	}
	
	static
	const char* predefined_bool( const void* param )
	{
		return *(const bool*) param ? "1" : "";
	}
	
	static
	const char* predefined_not( const void* param )
	{
		return !*(const bool*) param ? "1" : "";
	}
	
	static
	const char* predefined_cfm68k( const void* )
	{
		return global_config_68k && global_config_cfm ? "1" : "";
	}
	
	static
	const char* predefined_a5( const void* )
	{
		return   global_config_a4  ? "0"
		       : global_config_68k ? "1"
		       :                     "";
	}
	
	static const time_t& get_time()
	{
		static time_t t = time( NULL );
		
		return t;
	}
	
	static const plus::string& get_ctime()
	{
		static plus::string s = ctime( &get_time() );
		
		return s;
	}
	
	static plus::string make_date()
	{
		char buffer[ STRLEN( "'Mar 23 2010'" ) ];
		
		const size_t partial_length = STRLEN( "Mar 23 " );
		
		char* p = buffer;
		
		*p++ = '"';
		
		p = (char*) mempcpy( p, &get_ctime()[  4 ], partial_length   );
		p = (char*) mempcpy( p, &get_ctime()[ 20 ], STRLEN( "2010" ) );
		
		*p++ = '"';
		
		return plus::string( buffer, sizeof buffer );
	}
	
	static plus::string make_time()
	{
		char buffer[ STRLEN( "'11:16:35'" ) ];
		
		buffer[ 0                 ] =
		buffer[ sizeof buffer - 1 ] = '"';
		
		mempcpy( &buffer[ 1 ], &get_ctime()[ 11 ], sizeof buffer - 2 );
		
		return plus::string( buffer, sizeof buffer );
	}
	
	static
	const char* predefined_date( const void* )
	{
		static plus::string date = make_date();
		
		return date.c_str();
	}
	
	static
	const char* predefined_time( const void* )
	{
		static plus::string time = make_time();
		
		return time.c_str();
	}
	
	static plus::string quote_string( const plus::string& s )
	{
		const char*  s_data = s.data();
		const size_t s_size = s.size();
		
		const size_t length = vxo::quoted_length( s_data, s_size );
		
		plus::string result;
		
		char* p = result.reset( length );
		
		vxo::quote_string( s_data, s_size, p );
		
		return result;
	}
	
	static
	const char* predefined_file( const void* )
	{
		static plus::string file = quote_string( get_current_FILE() );
		
		return file.c_str();
	}
	
	static
	const char* predefined_line( const void* )
	{
		return gear::inscribe_unsigned_decimal( get_current_LINE() );
	}
	
	static const predefined_t predefined_macros[] =
	{
		{ "__A5__",           &predefined_a5                                   },
		{ "__BIG_ENDIAN__",   &predefined_string,     "1"                      },
		{ "__CFM68K__",       &predefined_cfm68k                               },
		{ "__DATE__",         &predefined_date                                 },
		{ "__FILE__",         &predefined_file                                 },
		{ "__IEEEdoubles__",  &predefined_68k_option, "IEEEdoubles"            },
		{ "__LINE__",         &predefined_line                                 },
		{ "__MACOS__",        &predefined_string,     "1"                      },
		{ "__MC68020__",      &predefined_68k_option, "code68020"              },
		{ "__MC68881__",      &predefined_68k_option, "code68881"              },
		{ "__MC68K__",        &predefined_bool,       &global_config_68k       },
		{ "__MWBROWSER__",    &predefined_string,     "0"                      },
	#ifdef __MC68K__
		// Assume CodeWarrior Pro 4
		{ "__MWERKS__",       &predefined_string,     "0x2200"                 },
	#else
		// Assume CodeWarrior Pro 6.3
		{ "__MWERKS__",       &predefined_string,     "0x2401"                 },
	#endif
		{ "__POWERPC__",      &predefined_bool,       &global_config_powerpc   },
		{ "__TIME__",         &predefined_time                                 },
		{ "__STDC__",         &predefined_not,        &global_config_cplusplus },
		{ "__cplusplus",      &predefined_bool,       &global_config_cplusplus },
		{ "__fourbyteints__", &predefined_68k_option, "fourbyteints"           },
		{ "__powerc",         &predefined_bool,       &global_config_powerpc   },
		{ "__profile__",      &predefined_option,     "profile"                },
	};
	
	static const predefined_t* find_predefined( const plus::string& token )
	{
		const predefined_t* begin = predefined_macros;
		const predefined_t* end   = begin + sizeof predefined_macros / sizeof predefined_macros[0];
		
		const predefined_t* it = std::find( begin, end, token );
		
		return it != end ? it : NULL;
	}
	
	plus::string eval_predefined_macro( const plus::string& name )
	{
		if ( const predefined_t* predef = find_predefined( name ) )
		{
			return predef->handler( predef->param );
		}
		
		return name;
	}
	
}
