// ==========
// Options.cc
// ==========

#include "Options.hh"

// Standard C++
#include <set>


struct OptionData
{
	const char* name;
};

static const char* const gOptionNames[] =
{
	"braceexpand",
	"errexit",
	"interactive",
	"interactive-comments",
	"noclobber",
	
	"~"
};

static const unsigned gCountOfOptions = sizeof gOptionNames / sizeof (const char*) - 1;

/*
static bool operator==( const char* option, const char* name )
{
	return 
}
*/

ShellOption LookupShellOption( const char* name )
{
	const char* const* end = gOptionNames + gCountOfOptions;
	
	const char* const* it = std::find( gOptionNames, end, std::string( name ) );
	
	if ( it == end )
	{
		throw NotAnOption();
	}
	
	return ShellOption( it - gOptionNames );
}

bool gOptions[ gCountOfOptions ];

bool GetOption( ShellOption option )
{
	return gOptions[ option ];
}

void SetOption( ShellOption option, bool state )
{
	gOptions[ option ] = state;
}

