/*
	Memory.cc
	---------
	
	Memory DA for Mac OS
	
	Copyright 2022, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif


#pragma exceptions off


static OSErr DRVR_open   ( DCtlEntry* dce : __A1 );
static OSErr DRVR_close  ( DCtlEntry* dce : __A1 );
static OSErr DRVR_control( DCtlEntry* dce : __A1, CntrlParam* pb : __A0 );

extern "C" void _DRVR_header();

asm void _DRVR_header()
{
	DC.W     dNeedTimeMask | dCtlEnableMask
	DC.W      120  // delay
	DC.W     updateMask
	DC.W        0  // menu
	
	DC.W     0x1C  // Open
	DC.W       -1  // Prime (unimplemented)
	DC.W     0x24  // Control
	DC.W       -1  // Status (unimplemented)
	DC.W     0x20  // Close
	
	DC.B     "\p\0" "Memory"
	DC.W     0
	
	JMP      DRVR_open
	JMP      DRVR_close
	JMP      DRVR_control
}

static
WindowRef make_window()
{
	const short width  = 124;
	const short height = 18;
	
	const short top = 40;
	const short left = 2;
	
	const Rect bounds = { top, left, top + height, left + width };
	
	WindowRef window = NewWindow( NULL,
	                              &bounds,
	                              "\p" "Memory",
	                              false,
	                              rDocProc,
	                              (WindowRef) -1,
	                              true,
	                              0 );
	
	if ( window )
	{
		GrafPtr savedPort;
		
		GetPort( &savedPort );
		SetPort( window );
		
		TextFont( kFontIDGeneva );
		TextSize( 9 );
		
		SetPort( savedPort );
	}
	
	return window;
}

static
void draw_window( DCtlEntry* dce : __A1, const Rect& rect )
{
	EraseRect( &rect );
	
	MoveTo( 6, 12 );
	
	TextFace( bold );
	
	DrawString( "\p" "Free mem: " );
	
	TextFace( normal );
	
	UInt8 s[ 16 ];
	
	NumToString( dce->dCtlPosition, s );
	
	DrawString( s );
}

static
OSErr DRVR_open( DCtlEntry* dce : __A1 )
{
	dce->dCtlPosition = 0;
	
	if ( WindowRef window = make_window() )
	{
		WindowPeek w = (WindowPeek) window;
		w->windowKind = dce->dCtlRefNum;
		
		dce->dCtlWindow = window;
		
		return noErr;
	}
	
	return memFullErr;
}

static
OSErr DRVR_close( DCtlEntry* dce : __A1 )
{
	if ( WindowRef window = dce->dCtlWindow )
	{
		dce->dCtlWindow = NULL;
		
		DisposeWindow( window );
	}
	
	return noErr;
}

static
OSErr DRVR_control( DCtlEntry* dce : __A1, CntrlParam* pb : __A0 )
{
	short message = pb->csCode;
	
	switch ( message )
	{
		case accRun:
		case accEvent:
			break;
		
		case accCursor:
			InitCursor();
			return 0;
		
		default:
			return paramErr;
	}
	
	WindowRef window = dce->dCtlWindow;
	
	GrafPtr savedPort;
	
	GetPort( &savedPort );
	SetPort( window );
	
	switch ( message )
	{
		case accRun:
			long freemem;
			
			if ( (freemem = FreeMem()) != dce->dCtlPosition )
			{
				dce->dCtlPosition = freemem;
				
				InvalRect( &window->portRect );
			}
			break;
		
		case accEvent:
			const EventRecord* event;
			event = *(const EventRecord**) pb->csParam;
			
			switch ( event->what )
			{
				case updateEvt:
					BeginUpdate( window );
					
					draw_window( dce, window->portRect );
					
					EndUpdate( window );
					break;
				
				default:
					break;
			}
			break;
		
		default:
			break;
	}
	
	SetPort( savedPort );
	
	return 0;
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
