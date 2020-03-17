/*
	Text.cc
	-------
*/

#include "Text.hh"

// Mac OS
#ifndef __FONTS__
#include <Fonts.h>
#endif

// ams-common
#include "QDGlobals.hh"
#include "redraw_lock.hh"


const Point OneOne : 0x0A02;


pascal void TextFont_patch( short font )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.txFont = font;
}

pascal void TextFace_patch( short face )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.txFace = face;
}

pascal void TextMode_patch( short mode )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.txMode = mode;
}

pascal void TextSize_patch( short size )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.txSize = size;
}

pascal void SpaceExtra_patch( Fixed extra )
{
	GrafPort& port = **get_addrof_thePort();
	
	port.spExtra = extra;
}

pascal void DrawChar_patch( short c )
{
	DrawText_patch( (const char*) &c, 1, 1 );
}

pascal void DrawString_patch( const unsigned char* s )
{
	DrawText_patch( (const char*) s, 1, s[ 0 ] );
}

pascal void DrawText_patch( const char* buffer, short offset, short n )
{
	StdText( n, buffer + offset, OneOne, OneOne );
}

pascal short CharWidth_patch( short c )
{
	return TextWidth_patch( (const char*) &c, 1, 1 );
}

pascal short StringWidth_patch( const unsigned char* s )
{
	return TextWidth_patch( (const char*) s, 1, s[ 0 ] );
}

pascal short TextWidth_patch( const char* buffer, short offset, short n )
{
	Point numer = OneOne;
	Point denom = OneOne;
	
	FontInfo info;
	
	return StdTxMeas( n, buffer + offset, &numer, &denom, &info );
}

static
FMOutPtr get_FontInfo( FontInfo* info, Point numer, Point denom )
{
	GrafPort& port = **get_addrof_thePort();
	
	const FMInput input =
	{
		port.txFont,
		port.txSize,
		port.txFace,
		false,
		0,
		numer,
		denom,
	};
	
	const FMOutPtr output = FMSwapFont( &input );
	
	info->ascent  = output->ascent;
	info->descent = output->descent;
	info->widMax  = output->widMax;
	info->leading = output->leading;
	
	return output;
}

pascal void GetFontInfo_patch( FontInfo* info )
{
	get_FontInfo( info, OneOne, OneOne );
}

pascal short StdTxMeas_patch( short        n,
                              const char*  p,
                              Point*       numer,
                              Point*       denom,
                              FontInfo*    info )
{
	const FMOutPtr output = get_FontInfo( info, *numer, *denom );
	
	if ( output->fontHandle == NULL )
	{
		return 0;
	}
	
	const FontRec& rec = **(FontRec**) output->fontHandle;
	
	const short* owTable = (short*) &rec.owTLoc + rec.owTLoc;
	
	short result = 0;
	
	while ( --n >= 0 )
	{
		uint8_t c = *p++;
		
		if ( c > rec.lastChar )
		{
			c = rec.lastChar + 1;  // missing character glyph
		}
		
		const int8_t character_width = owTable[ c ];
		
		result += character_width + output->extra;
	}
	
	return result;
}

pascal void StdText_patch( short n, const char* p, Point numer, Point denom )
{
	if ( denom.v == 0  ||  denom.h == 0 )
	{
		return;
	}
	
	GrafPort& port = **get_addrof_thePort();
	
	if ( port.pnVis < 0 )
	{
		return;
	}
	
	const FMInput input =
	{
		port.txFont,
		port.txSize,
		port.txFace,
		port.pnVis >= 0,
		0,
		numer,
		denom,
	};
	
	const FMOutPtr output = FMSwapFont( &input );
	
	if ( output->fontHandle == NULL )
	{
		return;
	}
	
	const FontRec& rec = **(FontRec**) output->fontHandle;
	
	const short* owTable = (short*) &rec.owTLoc + rec.owTLoc;
	
	const short rowBytes = rec.rowWords * 2;
	
	Ptr src = (Ptr) &rec + sizeof (FontRec);
	
	const short* locTable = (short*) (src + rec.fRectHeight * rowBytes);
	
	BitMap srcBits = { src, rowBytes, { 0, 0, rec.fRectHeight, rowBytes * 8 } };
	
	const BitMap& dstBits = port.portBits;
	
	Rect srcRect;
	Rect dstRect;
	
	short dst_ascent      = rec.ascent;
	short dst_fRectHeight = rec.fRectHeight;
	short dst_widMax      = rec.widMax;
	short dst_kernMax     = rec.kernMax;
	
	srcRect.top    = 0;
	srcRect.bottom = rec.fRectHeight;
	
	dstRect.top    = port.pnLoc.v - dst_ascent;
	dstRect.bottom = port.pnLoc.v - dst_ascent + dst_fRectHeight;
	
	// Worst case, for checking cursor intersection only
	dstRect.left  = port.portBits.bounds.left;
	dstRect.right = port.portBits.bounds.right;
	
	redraw_lock lock( port.portBits, dstRect );
	
	while ( --n >= 0 )
	{
		uint8_t c = *p++;
		
		if ( c > rec.lastChar )
		{
			c = rec.lastChar + 1;  // missing character glyph
		}
		
		// FIXME:  Check for -1
		const int8_t* offset_width = (int8_t*) &owTable[ c ];
		
		const int8_t character_offset = *offset_width++;
		const int8_t character_width  = *offset_width;
		
		const short this_offset = locTable[ c ];
		const short next_offset = locTable[ c + 1 ];
		
		if ( port.txMode == srcCopy )
		{
			dstRect.left  = port.pnLoc.h;
			dstRect.right = port.pnLoc.h + dst_widMax;
			
			EraseRect( &dstRect );
		}
		
		if ( short width = next_offset - this_offset )
		{
			srcRect.left  = this_offset;
			srcRect.right = next_offset;
			
			const short dstLeft = port.pnLoc.h + character_offset + dst_kernMax;
			
			dstRect.left  = dstLeft;
			dstRect.right = dstLeft + width;
			
			const short mode = port.txMode;
			
			CopyBits( &srcBits, &dstBits, &srcRect, &dstRect, mode, NULL );
			
			UInt8 bold = output->boldPixels;
			
			while ( bold-- > 0 )
			{
				++dstRect.left;
				++dstRect.right;
				
				CopyBits( &srcBits, &dstBits, &srcRect, &dstRect, mode, NULL );
			}
		}
		
		port.pnLoc.h += character_width + output->extra;
	}
}
