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
	
	return result * output->numer.h / output->denom.h;
}

static
void smear( const BitMap&  srcBits,
            const BitMap&  dstBits,
            const Rect&    srcRect,
            const Rect&    dstRect,
            short          m,
            short          n )
{
	Rect r = dstRect;
	
	const short height = r.bottom - r.top;
	const short width  = r.right - r.left;
	
	const short ii = r.top  + n;
	const short jj = r.left + m;
	
	const short i0 = r.top  - 1;
	const short j0 = r.left - 1;
	
	for ( short i = i0;  i <= ii;  ++i )
	{
		r.top = i;
		r.bottom = i + height;
		
		for ( short j = j0;  j <= jj;  ++j )
		{
			r.left = j;
			r.right = j + width;
			
			CopyBits( &srcBits, &dstBits, &srcRect, &r, srcOr, NULL );
		}
	}
}

pascal void StdText_patch( short n, const char* p, Point numer, Point denom )
{
	if ( denom.v == 0  ||  denom.h == 0 )
	{
		return;
	}
	
	GrafPort& port = **get_addrof_thePort();
	
	if ( Handle h = port.picSave )
	{
		Size size = GetHandleSize( h );
		
		SetHandleSize( h, size + 3 + 3 + 3 + 6 + (Byte) n );
		
		char* dst = *h + size;
		
		*dst++ = 0x03;  // TxFont
		*dst++ = port.txFont >> 8;
		*dst++ = port.txFont;
		
		*dst++ = 0x05;  // TxMode
		*dst++ = port.txMode >> 8;
		*dst++ = port.txMode;
		
		*dst++ = 0x0D;  // TxSize
		*dst++ = port.txSize >> 8;
		*dst++ = port.txSize;
		
		*dst++ = 0x28;  // LongText
		*dst++ = port.pnLoc.v >> 8;
		*dst++ = port.pnLoc.v;
		*dst++ = port.pnLoc.h >> 8;
		*dst++ = port.pnLoc.h;
		*dst++ = n;
		
		BlockMoveData( p, dst, (Byte) n );
	}
	
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
	
	Fixed v_scale = 0x10000;
	Fixed h_scale = 0x10000;
	
	if ( (long&) output->numer != (long&) denom )
	{
		v_scale = (output->numer.v << 16) / output->denom.v;
		h_scale = (output->numer.h << 16) / output->denom.h;
		
		dst_ascent      = dst_ascent      * v_scale >> 16;
		dst_fRectHeight = dst_fRectHeight * v_scale >> 16;
		
		dst_widMax  = dst_widMax  * h_scale >> 16;
		dst_kernMax = dst_kernMax * h_scale >> 16;
	}
	
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
		
		const int8_t character_offset = *offset_width++ * h_scale >> 16;
		const int8_t character_width  = *offset_width   * h_scale >> 16;
		
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
			
			width = width * h_scale >> 16;
			
			dstRect.left  = dstLeft;
			dstRect.right = dstLeft + width;
			
			short mode = port.txMode;
			
			UInt8 bold = output->boldPixels;
			UInt8 edge = output->shadowPixels;
			
			if ( edge )
			{
				smear( srcBits, dstBits, srcRect, dstRect, edge + bold, edge );
				
				mode = srcBic;
			}
			
			CopyBits( &srcBits, &dstBits, &srcRect, &dstRect, mode, NULL );
			
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
