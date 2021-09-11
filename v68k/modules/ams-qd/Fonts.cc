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
#include "c_string.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)
#define PSTR_LEN( s )  "\p" s, (sizeof s)

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


pascal void InitFonts_patch()
{
	ROMFont0 = NULL;
	
	the_current_font_record = NULL;
	the_current_font_resID  = 0;
	
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
	
	switch ( num )
	{
		case systemFont:
			fast_memcpy( name, PSTR_LEN( "Chicago" ) );
			return;
		
		case kFontIDNewYork:
			fast_memcpy( name, PSTR_LEN( "New York" ) );
			return;
		
		case kFontIDGeneva:
			fast_memcpy( name, PSTR_LEN( "Geneva" ) );
			return;
		
		case kFontIDMonaco:
			fast_memcpy( name, PSTR_LEN( "Monaco" ) );
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
		
		ReleaseResource( h );
		return;
	}
	
	if ( Handle font = GetNamedResource( 'FONT', name ) )
	{
		GetResInfo( font, num, NULL, NULL );
		
		ReleaseResource( font );
		
		*num /= 128u;
		return;
	}
	
	if ( memcmp( name, "\p" STR_LEN( "Geneva" ) ) == 0 )
	{
		*num = kFontIDGeneva;
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
		
		ReleaseResource( h );
	}
	
	return NULL;
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
		
		ReleaseResource( h );
		
		if ( id )
		{
			return id;
		}
	}
	
	return resID_for_font_and_size( font, size );
}

pascal unsigned char RealFont_patch( short num, short size )
{
	const short id = new_resID_for_font_and_size( num, size );
	
	// Don't bother releasing the resource for now.
	
	return GetResource( 'NFNT', id )  ||  GetResource( 'FONT', id );
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
			if ( Handle h = GetResource( 'NFNT', resID ) )
			{
				newFont = (FontRec**) h;
			}
			else if ( Handle h = GetResource( 'FONT', resID ) )
			{
				newFont = (FontRec**) h;
			}
			else if ( newFont == NULL )
			{
				return &the_current_FMOutput;
			}
			else
			{
				the_current_FMOutput.numer.v *= fontSize;
				the_current_FMOutput.numer.h *= fontSize;
				the_current_FMOutput.denom.v *= default_font_size;
				the_current_FMOutput.denom.h *= default_font_size;
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
