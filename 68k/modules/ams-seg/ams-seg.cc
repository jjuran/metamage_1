/*
	ams-seg.cc
	----------
*/

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
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

// Standard C
#include <string.h>

// chars
#include "conv/mac_utf8.hh"

// command
#include "command/get_option.hh"

// ams-common
#include "callouts.hh"

// ams-seg
#include "Segments.hh"
#include "options.hh"


#pragma exceptions off


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


QHdr   VCBQHdr       : 0x0356;
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

static
short documents_vRefNum()
{
	static short vRefNum;
	
	if ( vRefNum == 0 )
	{
		VCB* vcb = (VCB*) VCBQHdr.qHead;
		
		while ( vcb != NULL )
		{
			if ( vcb->vcbFSID == (short) 0xD0C5 )
			{
				vRefNum = vcb->vcbVRefNum;
				break;
			}
			
			vcb = (VCB*) vcb->qLink;
		}
	}
	
	return vRefNum;
}

struct AppFile_padded : AppFile
{
	Byte padding;
};

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
				/*
					TODO:  What should go in the vRefNum field?
					
					Assume the Documents volume for now, which
					is correct in most cases.
				*/
				
				using conv::mac_from_utf8_nothrow;
				
				/*
					Use a padded AppFile structure so we can legally
					write one byte beyond the end of fName, if needed.
				*/
				
				AppFile_padded file;
				
				file.vRefNum = documents_vRefNum();
				file.fType   = fileType;
				file.versNum = 0;
				
				const char* utf8_name = global_result.param;
				
				size_t n = mac_from_utf8_nothrow( (char*) file.fName + 1,
				                                  256,
				                                  utf8_name,
				                                  strlen( utf8_name ) );
				
				if ( n > 255 )
				{
					break;
				}
				
				Byte* p = file.fName;
				
				*p++ = n;
				
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
