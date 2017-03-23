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
void draw_pushbutton( const Rect& bounds )
{
	const short diameter = pushbutton_corner_diameter;  // 10px
	
	EraseRoundRect( &bounds, diameter, diameter );
	FrameRoundRect( &bounds, diameter, diameter );
}

static
void draw_checkbox( const Rect& button )
{
	EraseRect( &button );
	FrameRect( &button );
}

static
void draw_radiobutton( const Rect& button )
{
	EraseOval( &button );
	FrameOval( &button );
}

static
long CDEF_0_Draw( short varCode, ControlRef control, long param )
{
	if ( ! control[0]->contrlVis )
	{
		return 0;
	}
	
	PenState penState;
	GetPenState( &penState );
	
	PenSize( 1, 1 );
	
	const Rect& bounds = control[0]->contrlRect;
	
	Rect button_rect = varCode == pushButProc ? bounds
	                                          : switch_button_rect( bounds );
	
	switch ( varCode )
	{
		case pushButProc:
			draw_pushbutton( button_rect );
			break;
		
		case checkBoxProc:
			draw_checkbox( button_rect );
			break;
		
		case radioButProc:
			draw_radiobutton( button_rect );
			break;
		
		default:
			break;
	}
	
	SetPenState( &penState );
	
	return 0;
}

long CDEF_0( short varCode, ControlRecord** control, short message, long param )
{
	switch ( message )
	{
		case drawCntl:
			return CDEF_0_Draw( varCode, control, param );
		
		case testCntl:
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
