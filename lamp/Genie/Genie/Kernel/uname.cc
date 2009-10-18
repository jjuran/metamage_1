/*	========
 *	uname.cc
 *	========
 */

// Standard C
#include <ctype.h>
#include <string.h>

// POSIX
#include "sys/utsname.h"

// Iota
#include "iota/strings.hh"

// Debug
#include "debug/assert.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Utilities/GetWorkstationName.hh"


namespace Genie
{
	
	#if TARGET_CPU_68K
		
		#define HARDWARE_CLASS "m68k"
		
	#elif TARGET_CPU_PPC
		
		#define HARDWARE_CLASS "powerpc"
		
	#endif
	
	// Genie relies on CFM or classic 68K, so don't compile for Intel (below)
	
	
#if defined(_UTSNAME_LENGTH)
	
	static const unsigned kNameLength = _UTSNAME_LENGTH;
	
#elif defined(_SYS_NAMELEN)
	
	static const unsigned kNameLength = _SYS_NAMELEN;
	
#endif
	
	static void string_copy( char* dest, const char* str, size_t len )
	{
		ASSERT( len < kNameLength );
		
		memcpy( dest, str, len + 1 );  // copy length byte too
	}
	
	static int uname( struct utsname *uts )
	{
		SystemCallFrame frame( "uname" );
		
		std::string nodename;
		
		try
		{
			nodename = GetWorkstationName();
		}
		catch ( ... )
		{
		}
		
		char *const begin = &*nodename.begin();
		char *const end   = &*nodename.end();
		
		for ( char *p = begin;  p != end;  ++p )
		{
			const char c = *p;
			
			if ( !isalnum( c ) )
			{
				*p = '-';
			}
			else if ( c >= 'A'  &&  c <= 'Z' )
			{
				*p = c | ' ';
			}
		}
		
		string_copy( uts->sysname,  STR_LEN( "Genie" ) );
		string_copy( uts->nodename, nodename.c_str(), nodename.length() );
		string_copy( uts->release,  STR_LEN( "0.7something" ) );
		string_copy( uts->version,  STR_LEN( "verbose version string" ) );
		string_copy( uts->machine,  STR_LEN( HARDWARE_CLASS ) );
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( uname );
	
	#pragma force_active reset
	
}

