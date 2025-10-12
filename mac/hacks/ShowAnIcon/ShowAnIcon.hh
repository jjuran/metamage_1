/*
	ShowAnIcon.hh
	-------------
	
	A library of routines for an INIT to display an icon at startup.
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Synopsis:
	
		int main()
		{
			using show_an_icon::advance_location;
			using show_an_icon::draw_X;
			using show_an_icon::get_icon_rect;
			using show_an_icon::temporary_graphics_port;
			
			temporary_graphics_port port;
			
			Rect r;
			
			get_icon_rect( r );
			
			// Draw the icon into Rect r here.
			
			if ( int err = install_patches() )
			{
				draw_X( r );
			}
			
			advance_location();
			
			return 0;
		}
	
*/

#ifndef SHOWANICON_HH
#define SHOWANICON_HH

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// iota
#include "iota/class.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"


namespace show_an_icon
{

enum
{
	kIconWidth  = 32,
	kIconHeight = 32,
	
	kIconBreadth = kIconHeight << 16 | kIconWidth,
	
	kIconRowBytes = kIconWidth / 8,  // 4
	
	kMargin = 8,
	
	kInitialX = kMargin,
	kInitialY = kMargin + kIconHeight,
};

struct INITLocation
{
	short vcheck;
	Point where;
	short hcheck;
};

enum
{
	addr_of_CurApName = 0x0910,
	end_of_CurApName  = addr_of_CurApName + 32,
	addr_of_INITLoc   = end_of_CurApName - sizeof (INITLocation),
};

Rect CrsrPin : 0x0834;

INITLocation INITLoc : addr_of_INITLoc;

BitMap IconBitmap : 0x0A0E;


inline
asm
UInt16 ROL_W( UInt16 x : __D0 )
{
	ROL.W    #1,D0
}

inline
short checksum( short coordinate )
{
	return ROL_W( coordinate ) ^ 0x1021;
}

inline
void recover_location()
{
	if ( checksum( INITLoc.where.h ) != INITLoc.hcheck )
	{
		INITLoc.where.h = kInitialX;
	}
	
	if ( checksum( INITLoc.where.v ) != INITLoc.vcheck )
	{
		INITLoc.where.v = CrsrPin.bottom - kInitialY;
	}
}

inline
void advance_location()
{
	INITLoc.where.h += kIconWidth + kMargin;
	
	INITLoc.hcheck = checksum( INITLoc.where.h );
}

inline
void set_rect( Rect& r, UInt32 topLeft, UInt32 botRight )
{
	UInt32* p = (UInt32*) &r;
	
	*p++ = topLeft;
	*p++ = botRight;
}

inline
void get_icon_rect( Rect& r )
{
	recover_location();
	
	UInt32 loc = *(UInt32*) &INITLoc.where;
	
	set_rect( r, loc, loc + kIconBreadth );
}

inline
void draw_X( const Rect& r, short pen = 3 )
{
	ForeColor( redColor );
	
	PenSize( pen, pen );
	
	short h0 = r.left;
	short v0 = r.top;
	
	short h1 = r.right  - pen;
	short v1 = r.bottom - pen;
	
	MoveTo( h0, v0 );
	LineTo( h1, v1 );
	
	MoveTo( h1, v0 );
	LineTo( h0, v1 );
}

class temporary_graphics_port
{
	NON_COPYABLE( temporary_graphics_port )
	NO_NEW_DELETE
	
	private:
		QDGlobals  qd;
		GrafPort   port;
	
	public:
		temporary_graphics_port();
		
		~temporary_graphics_port();
};

inline
temporary_graphics_port::temporary_graphics_port()
{
	InitGraf( &qd.thePort );
	
	if ( mac::sys::has_ColorQuickDraw() )
	{
		OpenCPort( (CGrafPtr) &port );
	}
	else
	{
		OpenPort( &port );
	}
}

inline
temporary_graphics_port::~temporary_graphics_port()
{
	if ( port.portBits.rowBytes < 0 )
	{
		CloseCPort( (CGrafPtr) &port );
	}
	else
	{
		ClosePort( &port );
	}
}

}  // namespace show_an_icon

#endif

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
