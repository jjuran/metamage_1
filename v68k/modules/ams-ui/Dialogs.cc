/*
	Dialogs.cc
	----------
*/

#include "Dialogs.hh"

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __DIALOGS__
#include <Dialogs.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define SPACE  "    "


StringHandle DAStrings[ 4 ] : 0x0AA0;


static const char* no_icon[ 3 ] =
{
	"      ",
	"      ",
	"      "
};

static const char* stop_icon[ 3 ] =
{
	"  !!  ",
	"  !!  ",
	"  ..  "
};

static const char* note_icon[ 3 ] =
{
	"  **  ",
	" **** ",
	"  **  "
};

static const char* caution_icon[ 3 ] =
{
	"  /\\  ",
	" /  \\ ",
	"/____\\"
};

static
Handle NewHandleOrBust( Size size )
{
	Handle h = NewHandle( size );
	
	if ( h == NULL )
	{
		SysError( 25 );
	}
	
	return h;
}

static
void set_param( short i, const unsigned char* text )
{
	StringHandle& h = DAStrings[ i ];
	
	if ( text == NULL  ||  text[ 0 ] == 0 )
	{
		if ( h != NULL )
		{
			**h = 0;
		}
		
		return;
	}
	
	if ( h == NULL )
	{
		h = (StringHandle) NewHandleOrBust( 256 );
	}
	
	memcpy( *h, text, 1 + text[ 0 ] );
}


pascal void InitDialogs_patch( void* proc )
{
}

static
short basic_Alert( short alertID, ModalFilterUPP filterProc, const char** icon )
{
	Handle h = GetResource( 'ALRT', alertID );
	
	if ( h == NULL )
	{
		return ResError();
	}
	
	AlertTHndl alert = (AlertTHndl) h;
	
	const short itemsID = alert[0]->itemsID;
	
	ReleaseResource( h );
	
	h = GetResource( 'DITL', itemsID );
	
	if ( h == NULL )
	{
		return ResError();
	}
	
	const char* p = *h;
	
	short n_items_1 = *((const UInt16*) p)++;  // item count minus one
	
	const unsigned char*  button_title = NULL;
	const unsigned char*  static_text  = NULL;
	
	while ( n_items_1-- >= 0 )
	{
		p += 4;  // placeholder for handle or proc ptr
		p += 8;  // display rect
		
		UInt8 type = *p++;
		
		const unsigned char* data = (const unsigned char*) p;
		
		UInt8 len = *p++;
		
		switch ( type & 0x7F )
		{
			case ctrlItem + btnCtrl:
				button_title = data;
				break;
			
			case statText:
				static_text = data;
				break;
			
			case iconItem:
				switch ( *(const short*) p )
				{
					case stopIcon:     icon = stop_icon;     break;
					case noteIcon:     icon = note_icon;     break;
					case cautionIcon:  icon = caution_icon;  break;
					
					default:
						break;
				}
				break;
			
			default:
				break;
		}
		
		p += len;
	}
	
	p = *icon++;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	if ( static_text )
	{
		write( STDERR_FILENO, STR_LEN( "  " ) );
		write( STDERR_FILENO, static_text + 1, static_text[ 0 ] );
	}
	
	p = *icon++;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	p = *icon;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	if ( button_title )
	{
		write( STDERR_FILENO, STR_LEN( "  [" ) );
		write( STDERR_FILENO, button_title + 1, button_title[ 0 ] );
		write( STDERR_FILENO, STR_LEN( "]" ) );
	}
	
	write( STDERR_FILENO, STR_LEN( "\n\n" ) );
	
	ReleaseResource ( h );
	
	return 0;
}

pascal short Alert_patch( short alertID, ModalFilterUPP filterProc )
{
	return basic_Alert( alertID, filterProc, no_icon );
}

pascal short StopAlert_patch( short alertID, ModalFilterUPP filterProc )
{
	return basic_Alert( alertID, filterProc, stop_icon );
}

pascal short NoteAlert_patch( short alertID, ModalFilterUPP filterProc )
{
	return basic_Alert( alertID, filterProc, note_icon );
}

pascal short CautionAlert_patch( short alertID, ModalFilterUPP filterProc )
{
	return basic_Alert( alertID, filterProc, caution_icon );
}

pascal void ParamText_patch( const unsigned char*  p1,
                             const unsigned char*  p2,
                             const unsigned char*  p3,
                             const unsigned char*  p4 )
{
	set_param( 0, p1 );
	set_param( 1, p2 );
	set_param( 2, p3 );
	set_param( 3, p4 );
}
