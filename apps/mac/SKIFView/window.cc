/*
	window.cc
	---------
*/

#include "window.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-config
#include "mac_config/color-quickdraw.hh"

// iota
#include "iota/endian.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/load.hh"


using raster::raster_desc;


struct window_state
{
	raster_load  load;
	PixMap       pixmap;
	short        mode;
};

const Fixed fixed_72dpi = 72 << 16;

static inline
bool has_Color_QuickDraw()
{
	if ( ! CONFIG_COLOR_QUICKDRAW )
	{
		return false;
	}
	
	return CONFIG_COLOR_QUICKDRAW_GRANTED  ||  mac::sys::has_ColorQuickDraw();
}

static
Rect get_window_bounds( const raster_desc& desc )
{
	Rect bounds = mac::qd::main_display_bounds();
	
	bounds.top    += 40;  // approximate menu bar height + title bar height
	bounds.bottom -=  2;  // approximate window frame thickness
	
	short desk_height = bounds.bottom - bounds.top;
	short desk_width  = bounds.right - bounds.left;
	
	unsigned window_height = desc.height;
	unsigned window_width  = desc.width;
	
	while ( window_width < 128 )
	{
		window_height *= 2u;
		window_width  *= 2u;
	}
	
	while ( window_height > desk_height  ||  window_width > desk_width )
	{
		window_height /= 2u;
		window_width  /= 2u;
	}
	
	short v_margin = (desk_height - window_height) / 2u;
	short h_margin = (desk_width  - window_width ) / 2u;
	
	bounds.top += v_margin;
	bounds.left += h_margin;
	bounds.right -= h_margin;
	bounds.bottom -= v_margin;
	
	return bounds;
}

static
void convert_16bit( window_state& state )
{
	using namespace raster;
	
	Ptr baseAddr = (Ptr) state.load.addr;
	Size n_bytes =       state.load.size;
	
	const Ptr end_of_file = baseAddr + n_bytes;
	
	const raster_metadata& meta = *state.load.meta;
	const raster_desc&     desc = meta.desc;
	
	PixMap& pixmap = state.pixmap;
	
	const UInt32 xRGB1555_BE = iota::big_u32( 0x5A555000 );
	const UInt32 ARGB1555_BE = iota::big_u32( 0x5A55501F );
	const UInt32 RGB565_BE   = iota::big_u32( 0x5B655000 );
	
	UInt32 layout_BE = desc.magic ? desc.layout.per_pixel
	                 : desc.model == Model_RGB ? RGB565_BE
	                 :                           xRGB1555_BE;
	
	const bool BE_data = ((short*) end_of_file)[ -1 ];
	
#if ! OLDPIXMAPSTRUCT
	
	if ( layout_BE == RGB565_BE )
	{
		pixmap.pixelFormat = BE_data ? k16BE565PixelFormat
		                             : k16LE565PixelFormat;
	}
	
	if ( ! BE_data  &&  layout_BE == xRGB1555_BE )
	{
		pixmap.pixelFormat = k16LE555PixelFormat;
	}
	
	if ( ! BE_data  &&  layout_BE == ARGB1555_BE )
	{
		pixmap.pixelFormat = k16LE5551PixelFormat;
	}
	
#endif
}

static
void populate_pixmap( window_state& state )
{
	using namespace raster;
	
	const bool has_color = has_Color_QuickDraw();
	
	Ptr baseAddr = (Ptr) state.load.addr;
	Size n_bytes =       state.load.size;
	
	const raster_metadata& meta = *state.load.meta;
	const raster_desc&     desc = meta.desc;
	
	state.mode = has_color ? ditherCopy : srcCopy;
	
	const uint8_t depth = desc.weight;
	
	PixMap& pixmap = state.pixmap;
	
	Rect& bounds = pixmap.bounds;
	
	pixmap.baseAddr      = baseAddr;
	pixmap.rowBytes      = desc.stride;
	pixmap.bounds.bottom = desc.height;
	pixmap.bounds.right  = desc.width;
	
	if ( depth == 1 )
	{
		return;
	}
	
	if ( ! has_color )
	{
		bounds.right *= depth;
		return;
	}
	
	pixmap.vRes = fixed_72dpi;
	pixmap.hRes = fixed_72dpi;
	
	pixmap.pixelSize = depth;
	
	if ( has_color )
	{
		pixmap.rowBytes |= 0x8000;
		
		const Ptr end = baseAddr + n_bytes;
		
		if ( depth == 16 )
		{
			pixmap.pixelType = RGBDirect;
			pixmap.cmpCount  = 3;
			pixmap.cmpSize   = 5;
			
			convert_16bit( state );
		}
		else if ( depth == 32 )
		{
			pixmap.pixelType = RGBDirect;
			pixmap.cmpCount  = 3;
			pixmap.cmpSize   = 8;
			
			const UInt32 BGRx_BE = iota::big_u32( BGRx );
			const UInt32 RGBx_BE = iota::big_u32( RGBx );
			const UInt32 xBGR_BE = iota::big_u32( xBGR );
			
			UInt32 layout = desc.magic ? desc.layout.per_pixel
			              : (desc.model | 2) == Model_RGBA ? RGBx
			              :                                  xRGB;
			
			const bool swapped = ! (short) ((int*) end)[ -1 ];
			
			if ( ! desc.magic  &&  swapped )
			{
				layout = iota::swap_4_bytes( layout );
			}
			
			layout &= ~(0x01000001 * Channel_alpha);
			
		#if ! OLDPIXMAPSTRUCT
			
			if ( layout == BGRx_BE )  pixmap.pixelFormat = k32BGRAPixelFormat;
			if ( layout == RGBx_BE )  pixmap.pixelFormat = k32RGBAPixelFormat;
			if ( layout == xBGR_BE )  pixmap.pixelFormat = k32ABGRPixelFormat;
			
		#endif
		}
		else if ( depth >= 2 )
		{
			pixmap.cmpCount = 1;
			pixmap.cmpSize  = depth;
			
			const raster_note* clut_note = NULL;
			
			short clut_id = depth;
			
			if ( desc.model == Model_palette )
			{
				clut_note = find_note( meta, Note_clut );
			}
			else if ( depth > 2 )
			{
				clut_id |= 32;
			}
			
			CTabHandle ctab = GetCTable( clut_id );
			
			pixmap.pmTable = ctab;
			
			if ( clut_note )
			{
				const clut_data* clut = (const clut_data*) (clut_note + 1);
				
				if ( ctab[0]->ctSize >= clut->max )
				{
					const Size n = (clut->max + 1) * sizeof (ColorSpec);
					
					BlockMoveData( clut->palette, ctab[0]->ctTable, n );
				}
			}
		}
	}
	else if ( depth > 1 )
	{
		bounds.right *= depth;
	}
}

static
WindowRef populate_window_nonnull( WindowRef window, const raster_load& load )
{
	if ( window_state* state = (window_state*) NewPtrClear( sizeof *state ) )
	{
		state->load = load;
		
		populate_pixmap( *state );
		
		SetWRefCon( window, (long) state );
		
		return window;
	}
	
	return NULL;
}

static inline
WindowRef populate_window( WindowRef window, const raster_load& load )
{
	if ( ! window )
	{
		return NULL;
	}
	
	return populate_window_nonnull( window, load );
}

static inline
WindowRef title( WindowRef window, const HFSUniStr255& name )
{
	CFStringRef s;
	
	if ( window  &&  (s = CFStringCreateWithCharacters( NULL, name.unicode,
	                                                          name.length )) )
	{
		SetWindowTitleWithCFString( window, s );
		
		CFRelease( s );
	}
	
	return window;
}

static inline
WindowRef new_window( const raster_desc& desc, ConstStr255Param name )
{
	Rect bounds = get_window_bounds( desc );
	
	const WindowRef behind = (WindowRef) -1;
	const short     procid = noGrowDocProc;
	
	if ( ! has_Color_QuickDraw() )
	{
		return NewWindow( NULL, &bounds, name, true, procid, behind, true, 0 );
	}
	
	return NewCWindow( NULL, &bounds, name, true, procid, behind, true, 0 );
}

WindowRef create_window( const raster_load& load, ConstStr255Param name )
{
	const raster_desc& desc = load.meta->desc;
	
	return populate_window( new_window( desc, name ), load );
}

WindowRef create_window( const raster_load& load, const HFSUniStr255& name )
{
	const raster_desc& desc = load.meta->desc;
	
	return populate_window( title( new_window( desc, NULL ), name ), load );
}

void destroy_window( WindowRef window )
{
	if ( window_state* state = (window_state*) GetWRefCon( window ) )
	{
		if ( CTabHandle ctab = state->pixmap.pmTable )
		{
			DisposeCTable( ctab );
		}
		
		DisposePtr( (Ptr) state->load.addr );
		
		DisposePtr( (Ptr) state );
	}
	
	DisposeWindow( window );
}

void draw_window( WindowRef window )
{
	if ( window_state* state = (window_state*) GetWRefCon( window ) )
	{
		CGrafPtr port = GetWindowPort( window );
		
		SetPort( (GrafPtr) port );
		
		const Rect& bounds = mac::qd::get_portRect( port );
		
		CopyBits( (const BitMap*) &state->pixmap,
		          GetPortBitMapForCopyBits( port ),
		          &state->pixmap.bounds,
		          &bounds,
		          state->mode,
		          NULL );
	}
}
