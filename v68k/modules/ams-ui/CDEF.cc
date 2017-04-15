/*
	CDEF.cc
	-------
*/

#include "CDEF.hh"

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __CONTROLS__
#include <Controls.h>
#endif


const short pushbutton_corner_diameter = 10;

const short switch_edge_length = 12;
const short switch_thickness   =  1;

const short switch_interior_length = switch_edge_length - switch_thickness * 2;


static
Rect switch_button_rect( const Rect& bounds )
{
	const short height = bounds.bottom - bounds.top;
	
	const short length = switch_edge_length;  // 12px
	
	const short margin = (height - length) / 2;
	
	const short top = bounds.top + margin;
	
	Rect switch_rect = { top, bounds.left, top + length, bounds.left + length };
	
	return switch_rect;
}

static
void draw_pushbutton( const Rect& bounds, const unsigned char* title )
{
	const short diameter = pushbutton_corner_diameter;  // 10px
	
	EraseRoundRect( &bounds, diameter, diameter );
	FrameRoundRect( &bounds, diameter, diameter );
	
	if ( const short titleWidth = StringWidth( title ) )
	{
		const short titleAscent = 9;
		
		const short h = (bounds.left + bounds.right - titleWidth) / 2;
		const short v = (bounds.top + bounds.bottom + titleAscent) / 2;
		
		MoveTo( h, v );
		
		DrawString( title );
	}
}

static
void hilite_pushbutton( const Rect& bounds )
{
	const short diameter = pushbutton_corner_diameter;  // 10px
	
	InvertRoundRect( &bounds, diameter, diameter );
}

static
void draw_checkbox( const Rect& button )
{
	EraseRect( &button );
	FrameRect( &button );
}

static
void hilite_check( Rect rect )
{
	const short inset = switch_thickness;  // 1px
	
	InsetRect( &rect, inset, inset );
	
	PenMode( patXor );
	
	Rect top    = rect;
	Rect left   = rect;
	Rect right  = rect;
	Rect bottom = rect;
	
	top.bottom = top.top       + inset;
	bottom.top = bottom.bottom - inset;
	
	++left.top;
	++right.top;
	--left.bottom;
	--right.bottom;
	
	left.right = left.left   + inset;
	right.left = right.right - inset;
	
	PaintRect( &top    );
	PaintRect( &left   );
	PaintRect( &right  );
	PaintRect( &bottom );
}

static
void draw_radiobutton( const Rect& button )
{
	EraseOval( &button );
	FrameOval( &button );
}

static
void hilite_radio( Rect rect )
{
	const short inset = switch_thickness;  // 1px
	
	InsetRect( &rect, inset, inset );
	
	PenMode( patXor );
	
	FrameOval( &rect );
}

static
long CDEF_0_Draw( short varCode, ControlRef control, long param )
{
	if ( ! control[0]->contrlVis )
	{
		return 0;
	}
	
	GrafPtr port = control[0]->contrlOwner;
	
	const short font = port->txFont;
	const short size = port->txSize;
	
	TextFont( systemFont );
	TextSize( 0 );
	
	PenState penState;
	GetPenState( &penState );
	
	PenSize( 1, 1 );
	
	const Rect& bounds = control[0]->contrlRect;
	
	Rect button_rect = varCode == pushButProc ? bounds
	                                          : switch_button_rect( bounds );
	
	const bool draw   = false;
	const bool hilite = true;
	
	const bool hiliting = param != 0;
	
	const unsigned char* title = control[0]->contrlTitle;
	
	switch ( varCode << 1 | hiliting )
	{
		case pushButProc << 1 | draw:
			draw_pushbutton( button_rect, title );
			break;
		
		case pushButProc << 1 | hilite:
			hilite_pushbutton( button_rect );
			break;
		
		case checkBoxProc << 1 | draw:
			draw_checkbox( button_rect );
			break;
		
		case checkBoxProc << 1 | hilite:
			hilite_check( button_rect );
			break;
		
		case radioButProc << 1 | draw:
			draw_radiobutton( button_rect );
			break;
		
		case radioButProc << 1 | hilite:
			hilite_radio( button_rect );
			break;
		
		default:
			break;
	}
	
	SetPenState( &penState );
	
	TextFont( font );
	TextSize( size );
	
	return 0;
}

static
long CDEF_0_Hit( short varCode, ControlRef control, Point where )
{
	if ( ! control[0]->contrlVis )
	{
		return 0;
	}
	
	if ( PtInRect( where, &control[0]->contrlRect ) )
	{
		return 1;
	}
	
	return 0;
}

long CDEF_0( short varCode, ControlRecord** control, short message, long param )
{
	switch ( message )
	{
		case drawCntl:
			return CDEF_0_Draw( varCode, control, param );
		
		case testCntl:
			return CDEF_0_Hit( varCode, control, *(Point*) &param );
		
		case calcCRgns:
		case initCntl:
		case dispCntl:
			break;
		
		case posCntl:
		case thumbCntl:
		case dragCntl:
		case autoTrack:
		default:
			break;
	}
	
	return 0;
}
