/*
	ams-seg.cc
	----------
*/

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __SEGLOAD__
#include <SegLoad.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// POSIX
#include <unistd.h>

// iota
#include "iota/freestore_free.hh"

// command
#include "command/get_option.hh"

// ams-common
#include "callouts.hh"

// ams-seg
#include "Segments.hh"
#include "options.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-seg"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_linger = 'L',  // linger on ExitToShell
	Opt_open   = 'O',  // open a document
	Opt_type   = 't',  // type of subsequently opened documents
};

static command::option options[] =
{
	{ "linger",  Opt_linger },
	{ "open",    Opt_open,  command::Param_required },
	{ "type",    Opt_type,  command::Param_required },
	
	NULL,
};


Handle AppParmHandle : 0x0AEC;

void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)

enum
{
	_UnloadSeg = _UnLoadSeg,
};


static void install_SegmentLoader()
{
	TBTRAP( LoadSeg     );  // A9F0
	TBTRAP( UnloadSeg   );  // A9F1
	TBTRAP( Launch      );  // A9F2
	TBTRAP( ExitToShell );  // A9F4
	TBTRAP( GetAppParms );  // A9F5
}

struct AppFilesHeader
{
	short message;
	short count;
};

static OSType fileType;

static char* const* get_options( char** argv )
{
	using command::global_result;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_linger:
				linger_on_exit = true;
				break;
			
			case Opt_open:
			{
				AppFile file;
				
				file.vRefNum = 0;
				file.fType   = fileType;
				file.versNum = 0;
				
				size_t n = strlen( global_result.param );
				
				if ( n > 255 )
				{
					break;
				}
				
				uint8_t* p = file.fName;
				
				*p++ = n;
				
				fast_memcpy( p, global_result.param, n );
				
				/*
					To get from the name length to the actual size of an entry,
					first add the size of the members preceding the name.
					Then add one for the length byte, and one more for padding
					if the number of name bytes (including the length byte) is
					odd.  Since n doesn't include the length byte, we need to
					add one byte of padding if n is even.
					
					What we'll do is subtract one byte if n is odd but then add
					one back unconditionally.  If n is odd, these cancel out.
					But if n is even, we still add one byte.  We also have to
					add one for the length byte, so we actually add two bytes.
				*/
				
				n &= 0xfe;
				n += sizeof file - sizeof file.fName + 2;
				
				OSErr err = PtrAndHand( &file, AppParmHandle, n );
				
				if ( err == noErr )
				{
					AppFilesHeader** header = (AppFilesHeader**) AppParmHandle;
					
					++header[0]->count;
				}
				break;
			}
			
			case Opt_type:
				if ( strlen( global_result.param ) == sizeof fileType )
				{
					fast_memcpy( &fileType, global_result.param, sizeof fileType );
				}
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	AppParmHandle = NewHandleClear( sizeof (AppFilesHeader) );
	
	if ( argc > 0 )
	{
		char* const* args = get_options( argv );
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	install_SegmentLoader();
	
	module_suspend();  // doesn't return
}
