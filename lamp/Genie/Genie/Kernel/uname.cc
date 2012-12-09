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

// plus
#include "plus/var_string.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Utilities/GetWorkstationName.hh"


#define SYSNAME  "Relix"
#define RELEASE  "0.7.1"
#define VERSION  "MacRelix"


namespace Genie
{
	
	#ifdef __MC68K__
		
		#define HARDWARE_CLASS "m68k"
		
	#endif
		
	#ifdef __POWERPC__
		
		#define HARDWARE_CLASS "powerpc"
		
	#endif
	
	#ifdef __i386__
		
		#define HARDWARE_CLASS "x86"
		
	#endif
	
	
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
		plus::var_string nodename;
		
		try
		{
			nodename = GetWorkstationName( false );
		}
		catch ( ... )
		{
		}
		
		char *const begin = &*nodename.begin();
		char *const end   = &*nodename.end();
		
		for ( char *p = begin;  p != end;  ++p )
		{
			const char c = *p;
			
			if ( c & 0x80  ||  !isalnum( c ) )
			{
				*p = '-';
			}
			else if ( c >= 'A'  &&  c <= 'Z' )
			{
				*p = c | ' ';
			}
		}
		
		string_copy( uts->sysname,  STR_LEN( SYSNAME ) );
		string_copy( uts->nodename, nodename.c_str(), nodename.length() );
		string_copy( uts->release,  STR_LEN( RELEASE ) );
		string_copy( uts->version,  STR_LEN( VERSION ) );
		string_copy( uts->machine,  STR_LEN( HARDWARE_CLASS ) );
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( uname );
	
	#pragma force_active reset
	
}

