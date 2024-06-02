/*
	uname.cc
	--------
*/

// Standard C
#include <string.h>

// POSIX
#include "sys/utsname.h"

// more-libc
#include "more/string.h"

// iota
#include "iota/char_types.hh"
#include "iota/strings.hh"

// Debug
#include "debug/assert.hh"

// relix
#include "relix/syscall/registry.hh"

// Genie
#include "Genie/Utilities/GetWorkstationName.hh"


#pragma exceptions off


#define SYSNAME  "Relix"
#define RELEASE  "0.7.2-alpha.1"
#define VERSION  "MacRelix"

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


namespace Genie
{

static
void string_copy( char* dest, const char* str, size_t len )
{
	ASSERT( len < kNameLength );
	
	mempcpy( dest, str, len + 1 );  // copy length byte too
}

static
int uname( struct utsname *uts )
{
	GetWorkstationName( uts->nodename, sizeof uts->nodename );
	
	for ( char *p = uts->nodename;  *p != '\0';  ++p )
	{
		const char c = *p;
		
		if ( ! iota::is_alnum( c ) )
		{
			*p = '-';
		}
		else if ( c >= 'A'  &&  c <= 'Z' )
		{
			*p = c | ' ';
		}
	}
	
	string_copy( uts->sysname,  STR_LEN( SYSNAME ) );
	string_copy( uts->release,  STR_LEN( RELEASE ) );
	string_copy( uts->version,  STR_LEN( VERSION ) );
	string_copy( uts->machine,  STR_LEN( HARDWARE_CLASS ) );
	
	return 0;
}

#pragma force_active on

REGISTER_SYSTEM_CALL( uname );

#pragma force_active reset

}
