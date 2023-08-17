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
#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif
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

// mac-config
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#include "mac_sys/has/ColorQuickDraw.hh"

// mac-qd-utils
#include "mac_qd/copy_bits.hh"
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/load.hh"

// CGSKIF
#include "CGSKIF.hh"


/*
	Compositing mode is introduced in Mac OS X 10.2,
	but doesn't work without major problems until 10.3.
*/

#ifdef MAC_OS_X_VERSION_10_3
#define CONFIG_COMPOSITING  1
#else
#define CONFIG_COMPOSITING  0
#endif

using raster::raster_desc;


struct window_state
{
	raster_load  load;
	CGImageRef   image;
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
pascal OSStatus window_ControlDraw( EventHandlerCallRef  handler,
                                    EventRef             event,
                                    void*                userData )
{
	OSStatus err;
	
	ControlRef control;
	CGContextRef context = NULL;
	
	(err = GetEventParameter( event,
	                          kEventParamDirectObject,
	                          typeControlRef,
	                          NULL,
	                          sizeof control,
	                          NULL,
	                          &control ))  ||
	(err = GetEventParameter( event,
	                          kEventParamCGContextRef,
	                          typeCGContextRef,
	                          NULL,
	                          sizeof context,
	                          NULL,
	                          &context ));
	
	CGImageRef image = (CGImageRef) userData;
	
#ifdef MAC_OS_X_VERSION_10_2
	
	if ( err == noErr )
	{
		HIRect bounds;
		HIViewGetBounds( control, &bounds );
		
		HIViewDrawCGImage( context, &bounds, image );
	}
	
#endif
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, window_ControlDraw )

static EventTypeSpec controlDraw_event[] =
{
	{ kEventClassControl, kEventControlDraw },
};

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
void convert_32bit( window_state& state )
{
	using namespace raster;
	
	Ptr baseAddr = (Ptr) state.load.addr;
	Size n_bytes =       state.load.size;
	
	const Ptr end_of_file = baseAddr + n_bytes;
	
	const raster_metadata& meta = *state.load.meta;
	const raster_desc&     desc = meta.desc;
	
	PixMap& pixmap = state.pixmap;
	
	const UInt32 BGRx_BE = iota::big_u32( BGRx );
	const UInt32 RGBx_BE = iota::big_u32( RGBx );
	const UInt32 xBGR_BE = iota::big_u32( xBGR );
	
	UInt32 layout = desc.magic ? desc.layout.per_pixel
	              : (desc.model | 2) == Model_RGBA ? RGBx
	              :                                  xRGB;
	
	// Having `! X` instead of `X == 0` confused MWCPPC (but not MWC68K).
	
	const bool swapped = (short) ((int*) end_of_file)[ -1 ] == 0;
	
	if ( ! desc.magic  &&  swapped )
	{
		layout = iota::swap_4_bytes( layout );
	}
	
	layout &= ~(0x01000001 * Channel_alpha);
	
#ifdef __LITTLE_ENDIAN__
	
	if ( layout == BGRx_BE )  pixmap.pixelFormat = k32BGRAPixelFormat;
	if ( layout == RGBx_BE )  pixmap.pixelFormat = k32RGBAPixelFormat;
	if ( layout == xBGR_BE )  pixmap.pixelFormat = k32ABGRPixelFormat;
	
	return;
	
#endif
	
	UInt32* pix = (UInt32*)  baseAddr;
	UInt32* end = (UInt32*) (baseAddr + desc.height * desc.stride);
	
	if ( layout == BGRx_BE )
	{
		while ( pix < end )
		{
			UInt32 pixBGRA = *pix;
			UInt32 pixRABG = pixBGRA << 16 | pixBGRA >> 16;
			UInt32 pixARGB = (pixRABG << 8 & 0xFF00FF00)   // A_G_
			               | (pixRABG >> 8 & 0x00FF00FF);  // _R_B
			
			*pix++ = pixARGB;
		}
	}
	
	if ( layout == RGBx_BE )
	{
		while ( pix < end )
		{
			UInt32 pixRGBA = *pix;
			UInt32 pixARGB = (pixRGBA << 24)   // A___
			               | (pixRGBA >>  8);  // _RGB
			
			*pix++ = pixARGB;
		}
	}
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
	
#ifdef __LITTLE_ENDIAN__
	
	if ( ! BE_data  &&  layout_BE == xRGB1555_BE )
	{
		pixmap.pixelFormat = k16LE555PixelFormat;
		return;
	}
	
	if ( ! BE_data  &&  layout_BE == ARGB1555_BE )
	{
		pixmap.pixelFormat = k16LE5551PixelFormat;
		return;
	}
	
#endif
	
	UInt16* pix = (UInt16*)  baseAddr;
	UInt16* end = (UInt16*) (baseAddr + desc.height * desc.stride);
	
	if ( layout_BE == RGB565_BE )
	{
		/*
			Little-endian 565 to big-endian 1555:
			
			GGgB BBBB RRRR RGGG -> xRRR RRGG GGGB BBBB
			
			Little-endian 565 to little-endian 1555:
			
			RRRR RGGG GGgB BBBB -> xRRR RRGG GGGB BBBB
		*/
		
		while ( pix < end )
		{
			UInt16 pix565 = *pix;
			
		#ifndef __LITTLE_ENDIAN__
			
			UInt16 pix555 = (pix565 << 7 & 0x7F80)  // 0RRR RRGG G000 0000
			              | (pix565 >> 9 & 0x0060)  // 0000 0000 0GG0 0000
			              | (pix565 >> 8 & 0x001F); // 0000 0000 000B BBBB
			
		#else
			
			pixmap.pixelFormat = k16LE555PixelFormat;
			
			UInt16 pix555 = (pix565 >> 1 & 0x7FE0)  // 0RRR RRGG GGG0 0000
			              | (pix565 >> 0 & 0x001F); // 0000 0000 000B BBBB
			
		#endif
			
			*pix++ = pix555;
		}
	}
	
	if ( ! BE_data  &&  (layout_BE & 0xFFFFFF00) == xRGB1555_BE )
	{
		/*
			Little-endian 1555:
			
			GGGBBBBB xRRRRRGG -> xRRRRRGG GGGBBBBB
		*/
		
		while ( pix < end )
		{
			UInt16 pixLE = *pix;
			UInt16 pixBE = pixLE << 8 | pixLE >> 8;
			
			*pix++ = pixBE;
		}
	}
}

static
void populate_pixmap( window_state& state )
{
	using namespace raster;
	
	const bool has_color = has_Color_QuickDraw();
	
	const raster_metadata& meta = *state.load.meta;
	const raster_desc&     desc = meta.desc;
	
	state.mode = desc.model == Model_monochrome_light ? notSrcCopy
	                                                  :    srcCopy;
	
	if ( has_color )
	{
		state.mode |= ditherCopy;
	}
	
	const uint8_t depth = desc.weight;
	
	PixMap& pixmap = state.pixmap;
	
	Rect& bounds = pixmap.bounds;
	
	pixmap.baseAddr      = (Ptr) state.load.addr;
	pixmap.rowBytes      = desc.stride;
	pixmap.bounds.bottom = desc.height;
	pixmap.bounds.right  = desc.width;
	
	pixmap.pixelSize = depth;
	
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
	
	if ( has_color )
	{
		pixmap.rowBytes |= 0x8000;
		
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
			
			convert_32bit( state );
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
					ctab[0]->ctSeed += 1024;
					
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
		
		if ( TARGET_API_MAC_OSX )
		{
			state->image = CGSKIFCreateImageFromRaster( load );
			
			if ( CONFIG_COMPOSITING  &&  state->image != NULL )
			{
			#ifdef MAC_OS_X_VERSION_10_2
				
				OSStatus err;
				HIViewRef root = HIViewGetRoot( window );
				
				HIViewRef content;
				err = HIViewFindByID( root, kHIViewWindowContentID, &content );
				
				err = InstallControlEventHandler( content,
				                                  UPP_ARG( window_ControlDraw ),
				                                  1,
				                                  controlDraw_event,
				                                  state->image,
				                                  NULL );
				
				mac::ui::invalidate_window( window );
				
			#endif
				
			}
		}
		else
		{
			populate_pixmap( *state );
		}
		
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
	
	if ( TARGET_API_MAC_CARBON )
	{
		const WindowAttributes attrs = kWindowCloseBoxAttribute
		                             | kWindowCollapseBoxAttribute
		                           #if CONFIG_COMPOSITING
		                             | kWindowCompositingAttribute
		                           #endif
		                           #ifdef MAC_OS_X_VERSION_10_3
		                             | kWindowAsyncDragAttribute
		                           #endif
		                           #ifdef MAC_OS_X_VERSION_10_7
		                             | kWindowHighResolutionCapableAttribute
		                           #endif
		                             ;
		
		OSStatus err;
		WindowRef window;
		err = CreateNewWindow( kDocumentWindowClass, attrs, &bounds, &window );
		
		if ( err != noErr )
		{
			return NULL;
		}
		
		SetWTitle( window, name );
		
		ShowWindow( window );
		
		return window;
	}
	
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
		if ( TARGET_API_MAC_OSX )
		{
			CGImageRelease( state->image );
		}
		
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
		
		const Rect& bounds = mac::qd::get_portRect( port );
		
		if ( TARGET_API_MAC_OSX  &&  state->image )
		{
			CGContextRef context;
			CreateCGContextForPort( port, &context );
			
			CGRect rect = CGRectMake( 0, 0, bounds.right, bounds.bottom );
			
			CGContextDrawImage( context, rect, state->image );
			
			CGContextFlush  ( context );  // required in Mac OS X 10.4
			CGContextRelease( context );
			return;
		}
		
		SetPort( (GrafPtr) port );
		
		mac::qd::copy_bits( state->pixmap,
		                    port,
		                    state->pixmap.bounds,
		                    bounds,
		                    state->mode );
	}
}
