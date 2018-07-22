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
#define BLANK  "      "


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

struct expansion_storage
{
	Handle text;
	UInt16 size;  // Worst case is 127 * 255 + 1 == 32386
};

static
const expansion_storage& expand_param_text( const unsigned char* text )
{
	static expansion_storage expanded = { NewHandleOrBust( 0 ) };
	
	static UInt16 capacity = 0;
	
	UInt16 expanded_size = 0;
	
	const unsigned char* begin = text + 1;
	const unsigned char* end   = begin + text[ 0 ];
	
	for ( const unsigned char* it = begin;  it < end; )
	{
		unsigned char c = *it++;
		
		if ( c != '^'  ||  it == end )
		{
			++expanded_size;
			continue;
		}
		
		c = *it++ - '0';
		
		if ( c > 3 )
		{
			expanded_size += 2;
			continue;
		}
		
		if ( StringHandle h = DAStrings[ c ] )
		{
			expanded_size += **h;
		}
	}
	
	expanded.size = expanded_size;
	
	if ( capacity < expanded_size )
	{
		capacity = expanded_size;
		
		DisposeHandle( expanded.text );
		
		expanded.text = NewHandleOrBust( capacity );
	}
	
	char* p = *expanded.text;
	
	for ( const unsigned char* q = begin;  q < end; )
	{
		unsigned char c = *q++;
		
		if ( c != '^'  ||  q == end )
		{
			*p++ = c;
			continue;
		}
		
		c = *q++ - '0';
		
		if ( c > 3 )
		{
			*p++ = '^';
			*p++ = c + '0';
			continue;
		}
		
		if ( StringHandle h = DAStrings[ c ] )
		{
			memcpy( p, *h + 1, **h );
			
			p += **h;
		}
	}
	
	return expanded;
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
		
		p += len + (len & 1);
	}
	
	p = *icon++;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	const char* text = NULL;
	size_t      size = 0;
	
	bool multiple_lines = false;
	
	if ( static_text )
	{
		const expansion_storage& expansion = expand_param_text( static_text );
		
		text = *expansion.text;
		size =  expansion.size;
		
		const char* eol = (char*) memchr( text, '\r', size );
		
		const size_t n = eol ? eol - text : size;
		
		write( STDERR_FILENO, STR_LEN( "  " ) );
		write( STDERR_FILENO, text, n );
		
		size -= n + !! eol;
		text += n + !! eol;
		
		multiple_lines = size != 0;
	}
	
	p = *icon++;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	if ( size )
	{
		const char* eol = (char*) memchr( text, '\r', size );
		
		const size_t n = eol ? eol - text : size;
		
		write( STDERR_FILENO, STR_LEN( "  " ) );
		write( STDERR_FILENO, text, n );
		
		size -= n + !! eol;
		text += n + !! eol;
	}
	
	p = *icon;
	
	write( STDERR_FILENO, STR_LEN( "\n" SPACE ) );
	write( STDERR_FILENO, p, strlen( p ) );
	
	while ( size )
	{
		const char* eol = (char*) memchr( text, '\r', size );
		
		const size_t n = eol ? eol - text : size;
		
		write( STDERR_FILENO, STR_LEN( "  " ) );
		write( STDERR_FILENO, text, n );
		
		size -= n + !! eol;
		text += n + !! eol;
		
		write( STDERR_FILENO, STR_LEN( "\n" SPACE BLANK ) );
	}
	
	if ( button_title )
	{
		if ( multiple_lines )
		{
			write( STDERR_FILENO, STR_LEN( "\n" SPACE BLANK ) );
		}
		
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
