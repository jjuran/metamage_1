/*
	CDEF-1.cc
	---------
*/

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __CONTROLS__
#include <Controls.h>
#endif

// ams-common
#include "QDGlobals.hh"


static
pascal
long CDEF_1( short varCode, ControlRef control, short message, long param );

asm int main()
{
	JMP      CDEF_1
}

#define PACK8( _7, _6, _5, _4,  \
               _3, _2, _1, _0 ) \
	(  \
		+ ( _7 <<  7)  \
		| ( _6 <<  6)  \
		| ( _5 <<  5)  \
		| ( _4 <<  4)  \
		| ( _3 <<  3)  \
		| ( _2 <<  2)  \
		| ( _1 <<  1)  \
		| ( _0 <<  0)  \
	)

#define PACK16( _15, _14, _13, _12,  \
                _11, _10,  _9,  _8,  \
                 _7,  _6,  _5,  _4,  \
                 _3,  _2,  _1,  _0 ) \
	(  \
		+ (_15 << 15)  \
		| (_14 << 14)  \
		| (_13 << 13)  \
		| (_12 << 12)  \
		| (_11 << 11)  \
		| (_10 << 10)  \
		| ( _9 <<  9)  \
		| ( _8 <<  8)  \
		| ( _7 <<  7)  \
		| ( _6 <<  6)  \
		| ( _5 <<  5)  \
		| ( _4 <<  4)  \
		| ( _3 <<  3)  \
		| ( _2 <<  2)  \
		| ( _1 <<  1)  \
		| ( _0 <<  0)  \
	)

#define _ 0
#define X 1

static const UInt8 error_pattern[] =
{
	PACK8( _,_,_,X,_,_,_,X ),
	PACK8( _,_,X,_,_,_,X,_ ),
	PACK8( _,X,_,_,_,X,_,_ ),
	PACK8( X,_,_,_,X,_,_,_ ),
	PACK8( _,_,_,X,_,_,_,X ),
	PACK8( _,_,X,_,_,_,X,_ ),
	PACK8( _,X,_,_,_,X,_,_ ),
	PACK8( X,_,_,_,X,_,_,_ ),
};

static const UInt16 up_bits[] =
{
	PACK16( X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X ),
	PACK16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,X,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,X,_,X,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,X,_,_,_,X,_,_,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,X,_,_,_,_,X ),
	PACK16( X,_,_,X,_,_,_,_,_,_,_,X,_,_,_,X ),
	PACK16( X,_,X,_,_,_,_,_,_,_,_,_,X,_,_,X ),
	PACK16( X,X,X,X,X,_,_,_,_,_,X,X,X,X,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,X,_,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,X,_,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,X,_,_,_,_,X ),
	PACK16( X,_,_,_,X,X,X,X,X,X,X,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X ),
	PACK16( X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X ),
};

static const UInt16 down_bits[] =
{
	PACK16( X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X ),
	PACK16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,X,X,X,X,X,X,X,_,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,X,_,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,X,_,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,X,_,_,_,_,X ),
	PACK16( X,X,X,X,X,_,_,_,_,_,X,X,X,X,_,X ),
	PACK16( X,_,X,_,_,_,_,_,_,_,_,_,X,_,_,X ),
	PACK16( X,_,_,X,_,_,_,_,_,_,_,X,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,X,_,_,_,_,X ),
	PACK16( X,_,_,_,_,X,_,_,_,X,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,X,_,X,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,X,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X ),
	PACK16( X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X ),
};

static const UInt16 left_bits[] =
{
	PACK16( X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X ),
	PACK16( X,_,_,_,_,_,_,X,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,X,X,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,X,_,X,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,X,X,X,X,X,_,_,_,X ),
	PACK16( X,_,_,X,_,_,_,_,_,_,_,X,_,_,_,X ),
	PACK16( X,_,X,_,_,_,_,_,_,_,_,X,_,_,_,X ),
	PACK16( X,X,_,_,_,_,_,_,_,_,_,X,_,_,_,X ),
	PACK16( X,_,X,_,_,_,_,_,_,_,_,X,_,_,_,X ),
	PACK16( X,_,_,X,_,_,_,_,_,_,_,X,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,X,X,X,X,X,_,_,_,X ),
	PACK16( X,_,_,_,_,X,_,X,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,X,X,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,X,_,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X ),
	PACK16( X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X ),
};

static const UInt16 right_bits[] =
{
	PACK16( X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X ),
	PACK16( X,_,_,_,_,_,_,_,X,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,X,X,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,X,_,X,_,_,_,_,X ),
	PACK16( X,_,_,_,X,X,X,X,X,_,_,X,_,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,_,_,X,_,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,_,_,_,X,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,_,_,_,_,X,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,_,_,_,X,_,X ),
	PACK16( X,_,_,_,X,_,_,_,_,_,_,_,X,_,_,X ),
	PACK16( X,_,_,_,X,X,X,X,X,_,_,X,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,X,_,X,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,X,X,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,X,_,_,_,_,_,_,X ),
	PACK16( X,_,_,_,_,_,_,_,_,_,_,_,_,_,_,X ),
	PACK16( X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X ),
};

static inline
short thumb_offset_from_value( short gap, short value, short min, short max )
{
	const short divisor = max - min;
	
	return divisor ? (value - min) * gap / divisor : 0;
}

static
long CDEF_1_Draw( short varCode, ControlRef control, long param )
{
	if ( ! control[0]->contrlVis )
	{
		return 0;
	}
	
	Rect bounds = control[0]->contrlRect;
	
	FrameRect( &bounds );
	
	const short height = bounds.bottom - bounds.top;
	const short width  = bounds.right - bounds.left;
	
	const short aspect = height - width;  // x > 0: vertical, x < 0: horizontal
	
	const short gap = (aspect > 0 ? height : width) - 3 * 16;  // x - 48px
	
	if ( aspect == 0  ||  gap < 2 )
	{
		InsetRect( &bounds, 1, 1 );
		
		FillRect( &bounds, (Pattern*) &error_pattern );
		return 0;
	}
	
	const short min = control[0]->contrlMin;
	const short max = control[0]->contrlMax;
	
	const bool inactive = min >= max  ||  param == kControlInactivePart;
	const bool hiliting = param != 0  &&  param != kControlInactivePart;
	
	if ( param == kDrawControlIndicatorOnly )
	{
		goto indicator;
	}
	
	if ( hiliting )
	{
		goto hilite;
	}
	
	BitMap* dstBits = &control[0]->contrlOwner->portBits;
	
	BitMap arrow =
	{
		(Ptr) (aspect > 0 ? &up_bits : &left_bits),
		2,
		{ 0, 0, 16, 16 },
	};
	
	bounds = control[0]->contrlRect;
	
	bounds.bottom = bounds.top + 16;
	bounds.right = bounds.left + 16;
	
	CopyBits( &arrow, dstBits, &arrow.bounds, &bounds, srcCopy, NULL );
	
	arrow.baseAddr = (Ptr) (aspect > 0 ? &down_bits : &right_bits);
	
	bounds = control[0]->contrlRect;
	
	bounds.top = bounds.bottom - 16;
	bounds.left = bounds.right - 16;
	
	CopyBits( &arrow, dstBits, &arrow.bounds, &bounds, srcCopy, NULL );
	
	bounds = control[0]->contrlRect;
	
indicator:
	
	if ( true )
	{
		if ( aspect > 0 )
		{
			InsetRect( &bounds, 1, 16 );
		}
		else
		{
			InsetRect( &bounds, 16, 1 );
		}
		
		QDGlobals& qd = get_QDGlobals();
		
		FillRect( &bounds, inactive ? &qd.white : &qd.ltGray );
		
		if ( inactive )
		{
			return 0;
		}
		
		const short value = control[0]->contrlValue;
		const short delta = thumb_offset_from_value( gap, value, min, max );
		
		if ( aspect > 0 )
		{
			bounds.top += delta;
			bounds.bottom = bounds.top + 16;
		}
		else
		{
			bounds.left += delta;
			bounds.right = bounds.left + 16;
		}
		
		EraseRect( &bounds );
		FrameRect( &bounds );
		
		if ( param )
		{
			return 0;
		}
		
		param = control[0]->contrlHilite;
	}
	
hilite:
	
	bounds = control[0]->contrlRect;
	
	if ( param == kControlUpButtonPart )
	{
		bounds.bottom = bounds.top + 16;
		bounds.right = bounds.left + 16;
	}
	else if ( param == kControlDownButtonPart )
	{
		bounds.top = bounds.bottom - 16;
		bounds.left = bounds.right - 16;
	}
	else
	{
		return 0;
	}
	
	InsetRect( &bounds, 1, 1 );
	InvertRect( &bounds );
	
	return 0;
}

static
long CDEF_1_Hit( short varCode, ControlRef control, Point where )
{
	if ( ! control[0]->contrlVis )
	{
		return 0;
	}
	
	const short min = control[0]->contrlMin;
	const short max = control[0]->contrlMax;
	
	if ( min >= max )
	{
		return 0;
	}
	
	const Rect& bounds = control[0]->contrlRect;
	
	if ( ! PtInRect( where, &bounds ) )
	{
		return 0;
	}
	
	const short height = bounds.bottom - bounds.top;
	const short width  = bounds.right - bounds.left;
	
	const short aspect = height - width;  // x > 0: vertical, x < 0: horizontal
	
	const short gap = (aspect > 0 ? height : width) - 3 * 16;  // x - 48px
	
	if ( aspect == 0  ||  gap < 2 )
	{
		return 0;
	}
	
	const short coord = aspect > 0 ? where.v - bounds.top
	                               : where.h - bounds.left;
	
	const short end = aspect > 0 ? height : width;
	
	if ( coord <= 16 )
	{
		return kControlUpButtonPart;
	}
	
	if ( end - coord <= 16 )
	{
		return kControlDownButtonPart;
	}
	
	const short value = control[0]->contrlValue;
	const short delta = 16 + thumb_offset_from_value( gap, value, min, max );
	
	if ( coord < delta )
	{
		return kControlPageUpPart;
	}
	
	if ( coord > delta + 16 )
	{
		return kControlPageDownPart;
	}
	
	return 0;
}

static
long CDEF_1_CalcRgns( short varCode, ControlRef control, long param )
{
	const bool indicator_only = param < 0;
	
	*(Byte*) &param = 0;
	
	RgnHandle rgn = (RgnHandle) param;
	
	Rect bounds = control[0]->contrlRect;
	
	RectRgn( rgn, &bounds );
	
	return 0;
}

static
long CDEF_1_Thumb( short varCode, ControlRef control, long param )
{
	IndicatorDragConstraint& drag = *(IndicatorDragConstraint*) param;
	
	const Rect& bounds = control[0]->contrlRect;
	
	const short height = bounds.bottom - bounds.top;
	const short width  = bounds.right - bounds.left;
	
	const short aspect = height - width;
	
	const short gap = (aspect > 0 ? height : width) - 3 * 16;  // x - 48px
	
	const short min = control[0]->contrlMin;
	const short max = control[0]->contrlMax;
	
	const short value = control[0]->contrlValue;
	
	const short thumb = thumb_offset_from_value( gap, value, min, max );
	
	const short vertical = aspect > 0;
	
	short start = ((short*) param)[ ! vertical ];
	
	drag.limitRect = bounds;
	
	short* coords = (short*) &drag.limitRect;
	
	coords += ! vertical;
	
	coords[ 0 ] = start - thumb;
	coords[ 2 ] = start - thumb + gap;
	
	drag.slopRect = drag.limitRect;
	
	InsetRect( &drag.slopRect, -32, -32 );
	
	drag.axis = (aspect > 0) + 1;
	
	return 0;
}

static
long CDEF_1_Position( short varCode, ControlRef control, long param )
{
	const Rect& bounds = control[0]->contrlRect;
	
	const short height = bounds.bottom - bounds.top;
	const short width  = bounds.right - bounds.left;
	
	const short aspect = height - width;
	
	const short gap = (aspect > 0 ? height : width) - 3 * 16;  // x - 48px
	
	if ( gap > 0 )
	{
		const short min = control[0]->contrlMin;
		const short max = control[0]->contrlMax;
		
		const short value = control[0]->contrlValue;
		
		const short thumb = thumb_offset_from_value( gap, value, min, max );
		
		const short delta = aspect > 0 ? param >> 16 : param;
		
		const short range = max - min;
		
		const short fudge = gap / (range + 1);
		
		control[0]->contrlValue = ((thumb + delta) * range + fudge) / gap + min;
		
		CDEF_1_Draw( varCode, control, kDrawControlIndicatorOnly );
	}
	
	return 0;
}

pascal
long CDEF_1( short varCode, ControlRef control, short message, long param )
{
	switch ( message )
	{
		case drawCntl:
			return CDEF_1_Draw( varCode, control, param );
		
		case testCntl:
			return CDEF_1_Hit( varCode, control, *(Point*) &param );
		
		case calcCRgns:
			return CDEF_1_CalcRgns( varCode, control, param );
		
		case initCntl:
		case dispCntl:
			break;
		
		case posCntl:
			return CDEF_1_Position( varCode, control, param );
		
		case thumbCntl:
			return CDEF_1_Thumb( varCode, control, param );
		
		case dragCntl:
		case autoTrack:
		default:
			break;
	}
	
	return 0;
}
