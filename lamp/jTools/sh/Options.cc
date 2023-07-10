// ==========
// Options.cc
// ==========

#include "Options.hh"

// Standard C
#include <string.h>

// Standard C++
#include <algorithm>
#include <set>

// Debug
#include "debug/assert.hh"


namespace tool
{
	
	struct OptionData
	{
		const char*  name;
		ShellOption  selector;
	};
	
	static const OptionData gOptionNames[] =
	{
		{ "braceexpand",          kOptionBraceExpansion           },
		{ "errexit",              kOptionExitOnError              },
		//{ "interactive",          kOptionInteractive              },
		{ "interactive-comments", kOptionInteractiveComments      },
		{ "monitor",              kOptionMonitor                  },
		{ "noclobber",            kOptionNonClobberingRedirection },
		
		{ "~", kOptionCount }
	};
	
	static const unsigned gCountOfOptions = sizeof gOptionNames / sizeof (OptionData) - 1;
	
	static bool operator==( const OptionData& option, const char* name )
	{
		return strcmp( option.name, name ) == 0;
	}
	
	ShellOption LookupShellOption( const char* name )
	{
		const OptionData* end = gOptionNames + gCountOfOptions;
		
		const OptionData* it = std::find( gOptionNames, end, name );
		
		if ( it == end )
		{
			throw NotAnOption();
		}
		
		return it->selector;
	}
	
	static bool gOptions[ kOptionCount ];
	
	static void CheckOption( ShellOption option )
	{
		ASSERT( option >=            0 );
		ASSERT( option <  kOptionCount );
	}
	
	bool GetOption( ShellOption option )
	{
		CheckOption( option );
		
		return gOptions[ option ];
	}
	
	void SetOption( ShellOption option, bool state )
	{
		CheckOption( option );
		
		gOptions[ option ] = state;
	}
	
}
