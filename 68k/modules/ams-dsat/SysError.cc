/*
	SysError.cc
	-----------
*/

#include "SysError.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// POSIX
#include <unistd.h>

// math
#include "math/fractions.hh"


BitMap IconBitmap : 0x0A0E;

static const UInt32 icon[] =
{
	0x00000808,
	0x00000010,
	0x00000420,
	0x00008040,
	0x00002200,
	0x00000800,
	0x001f002a,
	0x0020c700,
	0x00403800,
	0x00400020,
	0x00400910,
	0x03f81008,
	0x03f82104,
	0x03f80000,
	0x0ffe0100,
	0x3fff8000,
	0x3fff8000,
	0x7fffc000,
	0x7fffc000,
	0xfffde000,
	0xffffe000,
	0xfffee000,
	0xfffee000,
	0xfffee000,
	0xffffe000,
	0xfffde000,
	0x7fffc000,
	0x7ff7c000,
	0x3fff8000,
	0x3fff8000,
	0x0ffe0000,
	0x03f80000,
};

void SysError_patch( short error : __D0 )
{
	using math::fractions::half;
	
	const short width  = 428;
	const short height = 112;
	
	QDGlobals qd;
	GrafPort port;
	
	InitGraf( &qd.thePort );
	
	InitCursor();
	
	OpenPort( &port );
	
	short left = half(port.portRect.right  - width );
	short top  =     (port.portRect.bottom - height) / 3 - 4;
	
	Rect bounds = port.portRect;
	
	bounds.top  += top;
	bounds.left += left;
	bounds.bottom = bounds.top + height;
	bounds.right  = bounds.left + width;
	
	PenNormal();
	EraseRect( &bounds );
	FrameRect( &bounds );
	
	InsetRect( &bounds, 3, 3 );
	PenSize( 2, 2 );
	FrameRect( &bounds );
	PenNormal();
	
	top  += 8;
	left += 8;
	
	const short bottom = top + height - 16;
	const short right  = left + width - 16;
	
	const short icon_h = left + 23;
	const short icon_v = top  + 12;
	
	const Rect icon_rect = { icon_v, icon_h, icon_v + 32, icon_h + 32 };
	
	IconBitmap.baseAddr = (Ptr) icon;
	IconBitmap.rowBytes = 4;
	IconBitmap.bounds.top  = 0;
	IconBitmap.bounds.left = 0;
	IconBitmap.bounds.bottom = 32;
	IconBitmap.bounds.right  = 32;
	
	StdBits( &IconBitmap,
	         &IconBitmap.bounds,
	         &icon_rect,
	         srcXor,
	         NULL );
	
	short button_bottom = top + height - 16 - 12;
	
	Rect restart_bounds =
	{
		button_bottom - 20, left + 23,
		button_bottom,      left + 83,
	};
	
	FrameRoundRect( &restart_bounds, 16, 16 );
	
	MoveTo( left + 23 + 4, button_bottom - 6 );
	
	DrawString( "\p" "Restart" );
	
	const short text_h = -left + icon_h * 2 + 32;
	const short text_v = top  + 12 + 15 + 12;
	
	MoveTo( text_h, text_v );
	
	DrawString( "\p" "Sorry, a system error occurred." );
	
	if ( (UInt16) error < 100 )
	{
		const short id_h = right  - 80;
		const short id_v = bottom - 20;
		
		MoveTo( id_h, id_v );
		
		unsigned char id[] = "\p" "ID = 00";
		
		id[ 6 ] += error / 10;
		id[ 7 ] += error % 10;
		
		DrawString( id );
	}
	
	InsetRect( &restart_bounds, 1, 1 );
	
	bool tracking = false;
	bool hiliting = false;
	
	const short eventMask = mDownMask | mUpMask | osMask;
	
	RgnHandle tmpRgn   = NULL;
	RgnHandle mouseRgn = NULL;
	
	while ( true )
	{
		EventRecord event;
		
		if ( WaitNextEvent( eventMask, &event, 0xFFFFFFFF, mouseRgn ) )
		{
			bool inside = PtInRect( event.where, &restart_bounds );
			
			if ( inside  &&  event.what == mouseDown )
			{
				tracking = true;
				
				if ( tmpRgn == NULL )
				{
					tmpRgn = NewRgn();
				}
				
				if ( tmpRgn )
				{
					mouseRgn = tmpRgn;
					
					Rect& r = mouseRgn[0]->rgnBBox;
					
					r.top  = event.where.v;
					r.left = event.where.h;
					r.bottom = r.top + 1;
					r.right = r.left + 1;
				}
			}
			
			if ( tracking  &&  inside != hiliting )
			{
				hiliting = inside;
				
				InvertRoundRect( &restart_bounds, 16, 16 );
			}
			
			if ( event.what == mouseUp )
			{
				if ( tracking  &&  inside )
				{
					break;
				}
				
				mouseRgn = NULL;
				tracking = false;
			}
		}
	}
	
	_exit( error );
}
