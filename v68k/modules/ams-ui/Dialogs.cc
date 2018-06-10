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


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define SPACE  "    "

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
	
	const char*  button_title_data = 0;
	UInt8        button_title_size = 0;
	
	const char*  static_text_data = 0;
	UInt8        static_text_size = 0;
	
	while ( n_items_1-- >= 0 )
	{
		p += 4;  // placeholder for handle or proc ptr
		p += 8;  // display rect
		
		UInt8 type = *p++;
		UInt8 len  = *p++;
		
		const char* data = p;
		
		p += len;
		
		switch ( type & 0x7F )
		{
			case ctrlItem + btnCtrl:
				button_title_data = data;
				button_title_size = len;
				break;
			
			case statText:
				static_text_data = data;
				static_text_size = len;
				break;
			
			case iconItem:
				switch ( *(const short*) data )
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
	}
	
	ReleaseResource ( h );
	
	p = *icon++;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	if ( static_text_size )
	{
		write( STDERR_FILENO, STR_LEN( "  " ) );
		write( STDERR_FILENO, static_text_data, static_text_size );
	}
	
	p = *icon++;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	p = *icon;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	if ( button_title_size )
	{
		write( STDERR_FILENO, STR_LEN( "  [" ) );
		write( STDERR_FILENO, button_title_data, button_title_size );
		write( STDERR_FILENO, STR_LEN( "]" ) );
	}
	
	write( STDERR_FILENO, STR_LEN( "\n\n" ) );
	
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
