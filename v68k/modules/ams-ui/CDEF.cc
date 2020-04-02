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

// ams-common
#include "QDGlobals.hh"


const short pushbutton_corner_diameter = 10;

const short switch_edge_length = 12;
const short switch_thickness   =  1;

const short switch_interior_length = switch_edge_length - switch_thickness * 2;

const short radio_mark_inset = 3;


static
Rect switch_button_rect( const Rect& bounds )
{
	const short height = bounds.bottom - bounds.top;
	
	const short length = switch_edge_length;  // 12px
	
	const short margin = (height - length) / 2u;
	
	const short top = bounds.top + margin;
	
	Rect switch_rect = { top, bounds.left, top + length, bounds.left + length };
	
	return switch_rect;
}

static
void draw_pushbutton( const Rect&           bounds,
                      const unsigned char*  title,
                      bool                  inactive )
{
	const short diameter = pushbutton_corner_diameter;  // 10px
	
	EraseRoundRect( &bounds, diameter, diameter );
	
	if ( const short titleWidth = StringWidth( title ) )
	{
		const short titleAscent = 9;
		
		const short h = (bounds.left + bounds.right - titleWidth) / 2u;
		const short v = (bounds.top + bounds.bottom + titleAscent) / 2u;
		
		MoveTo( h, v );
		
		DrawString( title );
		
		if ( inactive )
		{
			QDGlobals& qd = get_QDGlobals();
			
			PenPat( &qd.gray );
			PenMode( patBic );
			
			PaintRoundRect( &bounds, diameter, diameter );
			
			PenNormal();
		}
	}
	
	FrameRoundRect( &bounds, diameter, diameter );
}

static
void gray_out( const Rect& rect )
{
	QDGlobals& qd = get_QDGlobals();
	
	PenPat( &qd.gray );
	PenMode( patBic );
	
	PaintRect( &rect );
}

static
void hilite_pushbutton( const Rect& bounds )
{
	const short diameter = pushbutton_corner_diameter;  // 10px
	
	InvertRoundRect( &bounds, diameter, diameter );
}

static
void draw_checkbox( const Rect& button, bool selected )
{
	EraseRect( &button );
	FrameRect( &button );
	
	if ( selected )
	{
		const short inset = switch_thickness;  // 1px
		
		const short dx = switch_interior_length;  // 10px
		const short dy = switch_interior_length;
		
		Rect interior = button;
		
		InsetRect( &interior, inset, inset );
		
		MoveTo( interior.left, interior.top );
		Line( dx, dy );
		
		MoveTo( interior.right - inset, interior.top );
		Line( -dx, dy );
	}
}

static
void hilite_check( Rect rect, bool selected )
{
	const short inset = switch_thickness;  // 1px
	
	InsetRect( &rect, inset, inset );
	
	PenMode( patXor );
	
	Rect top    = rect;
	Rect left   = rect;
	Rect right  = rect;
	Rect bottom = rect;
	
	top.left  += selected;
	top.right -= selected;
	
	bottom.left  += selected;
	bottom.right -= selected;
	
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
void draw_radiobutton( const Rect& button, bool selected )
{
	EraseOval( &button );
	FrameOval( &button );
	
	if ( selected )
	{
		const short inset = radio_mark_inset;  // 3px
		
		Rect mark = button;
		
		InsetRect( &mark, inset, inset );
		
		PaintOval( &mark );
	}
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
void draw_switch_title( const Rect& bounds, const unsigned char* title )
{
	const short margin = 4;
	const short ascent = 9;
	
	const short h = bounds.left + switch_edge_length + margin;
	const short v = (bounds.top + bounds.bottom + ascent) / 2u;
	
	MoveTo( h, v );
	
	DrawString( title );
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
	
	const bool inactive = param == 255;
	const bool hiliting = param != 0  &&  ! inactive;
	
	const unsigned char* title = control[0]->contrlTitle;
	const short          value = control[0]->contrlValue;
	
	switch ( varCode << 1 | hiliting )
	{
		case pushButProc << 1 | draw:
			draw_pushbutton( button_rect, title, inactive );
			break;
		
		case pushButProc << 1 | hilite:
			hilite_pushbutton( button_rect );
			break;
		
		case checkBoxProc << 1 | draw:
			draw_checkbox( button_rect, value );
			draw_switch_title( bounds, title );
			
			if ( inactive )
			{
				gray_out( bounds );
			}
			
			break;
		
		case checkBoxProc << 1 | hilite:
			hilite_check( button_rect, value );
			break;
		
		case radioButProc << 1 | draw:
			draw_radiobutton( button_rect, value );
			draw_switch_title( bounds, title );
			
			if ( inactive )
			{
				gray_out( bounds );
			}
			
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

static
long CDEF_0_CalcRgns( short varCode, ControlRef control, long param )
{
	const bool indicator_only = param < 0;
	
	*(Byte*) param = 0;
	
	RgnHandle rgn = (RgnHandle) param;
	
	// TODO:  Calculate the region for real
	RectRgn( rgn, &control[0]->contrlRect );
	
	return 0;
}

pascal
long CDEF_0( short varCode, ControlRecord** control, short message, long param )
{
	switch ( message )
	{
		case drawCntl:
			return CDEF_0_Draw( varCode, control, param );
		
		case testCntl:
			return CDEF_0_Hit( varCode, control, *(Point*) &param );
		
		case calcCRgns:
			return CDEF_0_CalcRgns( varCode, control, param );
		
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
