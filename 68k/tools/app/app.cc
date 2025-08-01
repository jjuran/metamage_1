/*
	app.cc
	------
*/

// Mac OS
#ifndef __PACKAGES__
#include <Packages.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>

// mac-glue-utils
#include "mac_glue/OSUtils.hh"

// gear
#include "gear/parse_decimal.hh"

// chars
#include "conv/mac_utf8.hh"

// command
#include "command/get_option.hh"


#pragma exceptions off


UInt32 Ticks : 0x016A;
short SysMap : 0x0A58;


enum
{
	Opt_last_byte = 255,
	
	Opt_welcome,
};

static command::option options[] =
{
	{ "welcome", Opt_welcome, command::Param_required },
	
	NULL,
};

struct LaunchParamBlockRec
{
	void* appName;
	short reserved2;
	// ...
};

static QDGlobals qd;

static UInt32 welcome_ticks;

static inline
short asm Launch( void* pb : __A0 ) : __D0
{
	DC.W     0xA9F2  // _Launch
}

static inline
void paint_desktop()
{
	InitGraf( &qd.thePort );
	
	GrafPort port;
	
	OpenPort( &port );
	
	FillRoundRect( &port.portRect, 16, 16, &qd.gray );
	
	ClosePort( &port );
}

static
char* const* get_options( char** argv )
{
	using command::global_result;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		using gear::parse_unsigned_decimal;
		
		switch ( opt )
		{
			case Opt_welcome:
				welcome_ticks = parse_unsigned_decimal( &global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( argn < 1 )
	{
		return 0;
	}
	
	using conv::mac_from_utf8_nothrow;
	
	const char* path = args[ 0 ];
	
	const size_t len = strlen( path );
	
	paint_desktop();
	
	UInt32 initial_Ticks = Ticks;  // probably zero
	
	if ( welcome_ticks )
	{
		SysMap = OpenResFile( "\p" "AMS Resources" );
		
		SysError( dsGreeting );
	}
	
	InitResources();
	InitAllPacks();
	
	UInt32 current_Ticks = Ticks;
	UInt32 elapsed_ticks = current_Ticks - initial_Ticks;
	
	if ( welcome_ticks > elapsed_ticks )
	{
		mac::glue::delay( welcome_ticks - elapsed_ticks );
	}
	
	Byte appName[ 33 ];
	
	size_t n = mac_from_utf8_nothrow( (char*) appName + 1, 32, path, len );
	
	if ( n > 31 )
	{
		return 2;
	}
	
	appName[ 0 ] = n;
	
	LaunchParamBlockRec pb = { appName };
	
	Launch( &pb );
	
	return 1;
}
