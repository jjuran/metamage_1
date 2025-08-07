/*
	Fonts.cc
	--------
*/

#include "Fonts.hh"

// Mac OS
#ifndef __FONTS__
#include <Fonts.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "c_string.hh"


#pragma exceptions off


#define LENGTH(array)  (sizeof (array) / sizeof *(array))

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define SYSTEM_FONT_NOT_LOADED "warning: couldn't load the system font"

FontRec** ROMFont0 : 0x0980;
short     ApFontID : 0x0984;

const Point OneOne : 0x0A02;

Byte FScaleDisable : 0x0A63;


static inline
short resID_for_font_and_size( short font, short size )
{
	return font * 128 + size;
}

static inline
short specific_font( short font )
{
	return font == applFont ? ApFontID : font;
}

const short default_font_size = 12;

const short system_font_resID = resID_for_font_and_size( 0, default_font_size );

static short the_current_font_resID;

static FontRec** the_current_font_record;

static FMOutput the_current_FMOutput;

static ConstStr255Param basic_font_names[] =
{
	"\p" "Chicago",
	"\p",  // application font
	"\p" "New York",
	"\p" "Geneva",
	"\p" "Monaco",
};


static inline
Handle get_NFNT_or_FONT( short id )
{
	Handle h = GetResource( 'NFNT', id );
	
	if ( ! h )
	{
		h = GetResource( 'FONT', id );
	}
	
	return h;
}

static inline
void set_font_family_names()
{
	short id = 0;
	
	for ( int i = 0;  i < LENGTH( basic_font_names );  ++i, id += 128 )
	{
		if ( Handle h = GetResource( 'FONT', id ) )
		{
			SetResInfo( h, id, basic_font_names[ i ] );
		}
	}
}

pascal void InitFonts_patch()
{
	ROMFont0 = NULL;
	
	the_current_font_record = NULL;
	the_current_font_resID  = 0;
	
	set_font_family_names();
	
	const FMInput input = { 0, 0, 0, true, 0, OneOne, OneOne };
	
	FMSwapFont( &input );
	
	ROMFont0 = the_current_font_record;
	
	if ( the_current_font_record == NULL )
	{
		write( STDERR_FILENO, STR_LEN( SYSTEM_FONT_NOT_LOADED "\n" ) );
	}
}

pascal void GetFName_patch( short num, unsigned char* name )
{
	if ( num == applFont )
	{
		num = ApFontID;
	}
	
	if ( (UInt16) num < LENGTH( basic_font_names ) )
	{
		ConstStr255Param basic_name = basic_font_names[ num ];
		
		fast_memcpy( name, basic_name, 1 + basic_name[ 0 ] );
		return;
	}
	
	name[ 0 ] = 0;
}

static inline
short family_from_FOND( Handle h )
{
	const short family = ((short*) *h)[ 1 ];
	
	return family;
}

pascal void GetFNum_patch( const unsigned char* name, short* num )
{
	if ( Handle h = GetNamedResource( 'FOND', name ) )
	{
		*num = family_from_FOND( h );
		return;
	}
	
	if ( Handle font = GetNamedResource( 'FONT', name ) )
	{
		GetResInfo( font, num, NULL, NULL );
		
		*num /= 128u;
		return;
	}
	
	WARNING = "GetFNum: unknown ", CSTR( name );
	
	*num = 10;
}

static
Handle find_FOND( short family )
{
	short i = 0;
	
	while ( Handle h = GetIndResource( 'FOND', ++i ) )
	{
		if ( family_from_FOND( h ) == family )
		{
			return h;
		}
	}
	
	return NULL;
}

static
const AsscEntry* largest_of_family( short family )
{
	const AsscEntry* entry = NULL;
	
	if ( Handle fond = find_FOND( family ) )
	{
		short max_size = 0;
		
		const short* p = (const short*) (*fond + 0x34);
		
		short count_minus_1 = *p++;
		
		do
		{
			const short size = *p;
			
			if ( size > max_size )
			{
				max_size = size;
				
				entry = (const AsscEntry*) p;
			}
			
			p += 3;
		}
		while ( --count_minus_1 >= 0 );
	}
	
	return entry;
}

static
short new_resID_for_font_and_size( short font, short size )
{
	if ( Handle h = find_FOND( font ) )
	{
		const short* p = (const short*) (*h + 0x34);
		
		short count_minus_1 = *p++;
		
		short id = 0;
		
		do
		{
			if ( *p == size )
			{
				id = p[ 2 ];
				break;
			}
			
			p += 3;
		}
		while ( --count_minus_1 >= 0 );
		
		if ( id )
		{
			return id;
		}
	}
	
	return resID_for_font_and_size( font, size );
}

pascal Boolean RealFont_patch( short num, short size )
{
	const short id = new_resID_for_font_and_size( num, size );
	
	return get_NFNT_or_FONT( id ) != NULL;
}

pascal void SetFontLock_patch( Boolean locked )
{
}

static const Byte characterization_table[] =
{
	80, 80,    // dpi for scaling between devices
	0, 1, 1,   // bold
	1, 8, 0,   // italic
	0, 0, 0,   // not used
	5, 1, 1,   // outline
	5, 2, 2,   // shadow
	0, 0, -1,  // condensed
	0, 0, 1,   // extended
	
	1, 1, 1,   // underline
};

static
void apply_characterization( Style face, FMOutput& output )
{
	if ( face & underline )
	{
		face &= ~underline;
		
		const Byte* p = characterization_table + 23;
		
		Byte* q = &output.ulOffset;
		
		*q++ += *p++;
		*q++ += *p++;
		*q++ += *p++;
	}
	
	const Byte* p = characterization_table + 2;
	
	Byte* characteristics = &output.boldPixels;
	
	while ( face != 0 )
	{
		if ( face & 1 )
		{
			const UInt8 i = *p++;
			
			characteristics[ i ]  = *p++;
			output.extra         += *p++;
		}
		else
		{
			p += 3;
		}
		
		face >>= 1;
	}
}

pascal FMOutPtr FMSwapFont_patch( const FMInput* input )
{
	const short fontNum  = specific_font( input->family );
	const short fontSize = input->size ? input->size : default_font_size;
	
	const short resID = new_resID_for_font_and_size( fontNum, fontSize );
	
	if ( resID != the_current_font_resID )
	{
		the_current_FMOutput.numer = input->numer;
		the_current_FMOutput.denom = input->denom;
		
		FontRec** newFont = ROMFont0;
		
		if ( resID != system_font_resID  ||  newFont == NULL )
		{
			if ( Handle h = get_NFNT_or_FONT( resID ) )
			{
				newFont = (FontRec**) h;
			}
			else if ( newFont == NULL )
			{
				return &the_current_FMOutput;
			}
			else
			{
				short found_font_size = default_font_size;
				
				if ( const AsscEntry* entry = largest_of_family( fontNum ) )
				{
					if ( Handle h = get_NFNT_or_FONT( entry->fontID ) )
					{
						newFont = (FontRec**) h;
						
						found_font_size = entry->fontSize;
					}
				}
				
				the_current_FMOutput.numer.v *= fontSize;
				the_current_FMOutput.numer.h *= fontSize;
				the_current_FMOutput.denom.v *= found_font_size;
				the_current_FMOutput.denom.h *= found_font_size;
			}
		}
		
		the_current_font_resID  = resID;
		the_current_font_record = newFont;
		
		FontRec& rec = **newFont;
		
		the_current_FMOutput.fontHandle = (Handle) newFont;
		the_current_FMOutput.ascent     = rec.ascent;
		the_current_FMOutput.descent    = rec.descent;
		the_current_FMOutput.widMax     = rec.widMax;
		the_current_FMOutput.leading    = rec.leading;
	}
	
	fast_memset( &the_current_FMOutput.boldPixels, '\0', 7 );
	
	if ( input->face )
	{
		apply_characterization( input->face, the_current_FMOutput );
	}
	
	return &the_current_FMOutput;
}

pascal void SetFScaleDisable_patch( Boolean disable )
{
	FScaleDisable = disable;
}
